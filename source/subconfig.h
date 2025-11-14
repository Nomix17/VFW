#ifndef SUBCONFIG
#define SUBCONFIG

#include "json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>


#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <QList>
#include <QIntValidator>
#include <QFontDatabase>

using namespace nlohmann;

class SubConfig:public QDialog{
  Q_OBJECT

public:

  QString selected_font;
  int padding;
  int marginbottom;

private:

  std::vector <QString>labels={"Margin Bottom:","Padding:",
                      "Bg Color:","Bg Opacity:",
                      "Font Size:","Font Familly:",
                      "Font Color:","Font Opacity:"
  };
  QVBoxLayout * mainlayout;
  QGridLayout * gridlayout;
  QHBoxLayout * buttonlayout;
  
public: 
  std::string Configsdirectory = "";
  std::string StyleDirectory = "";
  QList <QString> fontPaths;

  void gui(std::string Configspath,std::string FONTSDIRECTORY,std::string StylePath){
    this->setFixedSize(480,400);
    StyleDirectory = StylePath;
    Configsdirectory = Configspath;
    mainlayout = new QVBoxLayout(this);
    gridlayout = new QGridLayout();
    buttonlayout = new QHBoxLayout();
    for(size_t i=0;i<labels.size();i++){
      QLabel *label = new QLabel;
      label->setText(labels[i]);
      gridlayout->addWidget(label,i/2,(i%2)*2);
      if(i==2||i==6){
        QPushButton *colorpicker = new QPushButton();
        colorpicker->setObjectName(labels[i].replace(":","").replace(" ",""));
        connect(colorpicker,&QPushButton::clicked,[this,colorpicker](){
          QColor color = QColorDialog::getColor(Qt::white,this,"Pick your color");
          if(color.isValid()){
            colorpicker->setStyleSheet("background-color:"+color.name()+";");
          }
        });
        gridlayout->addWidget(colorpicker,i/2,(i%2)*2+1);
      
      }else if(i==5){
        QToolButton *button = new QToolButton;
        button->setPopupMode(QToolButton::InstantPopup);
        button->setText("Font");
        button->setObjectName(labels[i]);
        loadFonts(FONTSDIRECTORY);
        QMenu *font_familly = new QMenu();
        for(int i=0;i<fontPaths.size();i++){
          int id = QFontDatabase::addApplicationFont(fontPaths[i]);
          QString fontName = QFontDatabase::applicationFontFamilies(id).at(0);

          QAction * actions = new QAction;
          actions->setText(fontName);
          font_familly->addAction(actions);
          connect(actions,&QAction::triggered,[this,i,fontName,button](){
            selected_font = fontName;
            button->setText(fontName);
          });
        }
        button->setMenu(font_familly);
        gridlayout->addWidget(button,i/2,(i%2)*2+1);
      }else{
        QLineEdit* number_picker = new QLineEdit();
        number_picker->setObjectName(labels[i]);
        number_picker->setValidator(new QIntValidator(0, 100, this)); 
        gridlayout->addWidget(number_picker,i/2,(i%2)*2+1);
      }
    }

      QPushButton *done = new QPushButton(this);
      done->setText("done");
      QPushButton *cancel = new QPushButton(this);
      cancel->setText("cancel");
      connect(done,&QPushButton::clicked,[this](){
        saveconfig();
        QDialog::accept();
      });
      connect(cancel,&QPushButton::clicked,[this](){
        QDialog::accept();
      });
      buttonlayout->addWidget(done);
      buttonlayout->addWidget(cancel);
      mainlayout->addLayout(gridlayout);
      mainlayout->addLayout(buttonlayout);
      setLayout(mainlayout);

      loadstylefiles();
      loadconfig();
  }

  void loadstylefiles(){
    //load style file
    std::ifstream stylefile(StyleDirectory+"subconfig.css");
    if(stylefile){
      std::ostringstream sstr;
      std::string script;
      sstr<<stylefile.rdbuf();
      script = sstr.str();
      this->setStyleSheet(QString::fromStdString(script));
      stylefile.close();
    }
  }

  void saveconfig(){

    json settings;

    settings["margin-bottom"]=std::stoi(this->findChild<QLineEdit*>("Margin Bottom:")->text().toStdString());
    settings["padding"]=std::stoi(this->findChild<QLineEdit*>("Padding:")->text().toStdString());

    settings["bg-color"]=this->findChild<QPushButton*>("BgColor")->palette().color(QPalette::Button).name().toStdString();
    settings["bg-opacity"]=std::stoi(this->findChild<QLineEdit*>("Bg Opacity:")->text().toStdString());

    settings["font-size"]=std::stoi(this->findChild<QLineEdit*>("Font Size:")->text().toStdString());
    settings["font-familly"]=selected_font.toStdString();

    settings["font-color"]=this->findChild<QPushButton*>("FontColor")->palette().color(QPalette::Button).name().toStdString();
    settings["font-opacity"]=std::stoi(this->findChild<QLineEdit*>("Font Opacity:")->text().toStdString());

    std::ofstream file(Configsdirectory+"subconfig.json");
    file << settings.dump(4);
    file.close();

  }
  
  void loadconfig(){
    json configfile;
    std::string subConfigPath = Configsdirectory+"subconfig.json";
    std::ifstream file(subConfigPath);
    if(file){
      file>>configfile;

      this->findChild<QLineEdit*>("Margin Bottom:")->setText(QString::number(configfile["margin-bottom"].get<int>()));
      this->findChild<QLineEdit*>("Padding:")->setText(QString::number(configfile["padding"].get<int>()));

      this->findChild<QPushButton*>("BgColor")->setStyleSheet("background-color:"+QString::fromStdString(configfile["bg-color"])+";");
      this->findChild<QLineEdit*>("Bg Opacity:")->setText(QString::number(configfile["bg-opacity"].get<int>()));
     
      this->findChild<QLineEdit*>("Font Size:")->setText(QString::number(configfile["font-size"].get<int>()));
      this->findChild<QToolButton*>("Font Familly:")->setText(QString::fromStdString(configfile["font-familly"]));
     
      this->findChild<QPushButton*>("FontColor")->setStyleSheet("background-color:"+QString::fromStdString(configfile["font-color"])+";");
      this->findChild<QLineEdit*>("Font Opacity:")->setText(QString::number(configfile["font-opacity"].get<int>()));

      selected_font = QString::fromStdString(configfile["font-familly"]);

      file.close();
    }else{
      std::cerr << "[ WARNING ] Cannot find subtitles config File: "<<subConfigPath<<"\n";
    }
  }

  QString makehtml(std::string Configspath){
    Configsdirectory = Configspath;
    std::string subConfigPath = Configspath+"subconfig.json";
    if(!std::filesystem::exists(subConfigPath)){
      createdefaultjson();
    }

    json settings;
    std::ifstream file(subConfigPath);
    if(file){
      file>>settings;

      QColor bgcolor(QString::fromStdString(settings["bg-color"]));
      QColor fontcolor(QString::fromStdString(settings["font-color"]));

      QString backgroundColorRGB = QString::number(bgcolor.red()) + ", " + QString::number(bgcolor.green()) + ", " + QString::number(bgcolor.blue());
      QString backgroundColorAlpha = QString::number(static_cast<double>(settings["bg-opacity"]) / 100, 'f', 2);
      QString textColorRGB = QString::number(fontcolor.red()) + ", " + QString::number(fontcolor.green()) + ", " + QString::number(fontcolor.blue());
      QString textColorAlpha = QString::number(static_cast<double>(settings["font-opacity"]) / 100, 'f', 2);
      QString paddingValue = QString::number(static_cast<double>(settings["padding"]), 'f', 2);

                // "text-shadow: -1px -1px 0 "+backgroundColorRGB+", 1px -1px 0 "+backgroundColorRGB+", -1px  1px 0 "+backgroundColorRGB+", 1px  1px 0 "+backgroundColorRGB+";"

    QString HtmlScript = "<table cellpadding='" + paddingValue + "' "
                         "style='"
                         "background-color: rgba(" + backgroundColorRGB + ", " + backgroundColorAlpha + "); "
                         "color: rgba(" + textColorRGB + ", " + textColorAlpha + "); "
                         "font-family: \"" + QString::fromStdString(settings["font-familly"]) + "\"; "
                         "font-size: " + QString::number(static_cast<int>(settings["font-size"])) + "px;"
                         "'>"
                         "<tr><td>";

      padding = settings["padding"];
      marginbottom = settings["margin-bottom"];

      file.close();
      return HtmlScript;
    }else{
      std::cerr << "[ WARNING ] Failed to open subtitles config File: "<<subConfigPath<<"\n";
    }
  }
  void loadFonts(std::string FONTSDIRECTORY){
    for(const auto & fileEntry : std::filesystem::directory_iterator(FONTSDIRECTORY)){
      std::filesystem::path filePath = fileEntry.path();
      std::string filePathString = filePath.string();
      std::string fontNameString = filePath.filename().stem().string();
      fontPaths.push_back(QString::fromStdString(filePathString));
      QFontDatabase::addApplicationFont(QString::fromStdString(filePathString));
    }
  }
  void createdefaultjson(){
    json defaultsettings;

    defaultsettings["bg-color"] = "#000000";
    defaultsettings["bg-opacity"]= 20;
    defaultsettings["font-color"]= "#ffffff";
    defaultsettings["font-familly"]="Noto Sans Arabic";
    defaultsettings["font-opacity"]= 100;
    defaultsettings["font-size"]= 24;
    defaultsettings["margin-bottom"]= 61;
    defaultsettings["padding"]= 26;
   
    std::string subConfigPath = Configsdirectory+"subconfig.json";
    std::ofstream file(subConfigPath);
    if(file){
      file << defaultsettings.dump(4);
      file.close();
    }else{
      std::cerr << "[ WARNING ] Cannot create a default subtitles config File: "<<subConfigPath<<"\n";
    }
  }
};

#endif
