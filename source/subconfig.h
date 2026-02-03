#ifndef SUBCONFIG
#define SUBCONFIG

#include "json.hpp"

#include <filesystem>
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
                      "Font Color:","Font Opacity:",
                      "Font Weight:"
  };
  QVBoxLayout * mainlayout;
  QGridLayout * gridlayout;
  QHBoxLayout * buttonlayout;

public: 
  std::string fullSubConfigPath; 
  QList <QString> fontPaths;

  std::string ConfigsPath;
  std::string FONTSDIRECTORY;
  std::string StyleDirectory;

  SubConfig(std::string ConfigsPath,std::string FONTSDIRECTORY,std::string StyleDirectory){
    this->ConfigsPath = ConfigsPath;
    this->FONTSDIRECTORY = FONTSDIRECTORY;
    this->StyleDirectory = StyleDirectory;
    fullSubConfigPath = std::filesystem::path(std::filesystem::path(ConfigsPath) / "subconfig.json").string();
  }

  void gui(){
    this->setFixedSize(480,400);
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
        loadFonts();
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
    std::filesystem::path styleFullPath(std::filesystem::path(StyleDirectory) / "subconfig.css");
    std::ifstream stylefile(styleFullPath);
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
    settings["font-weight"]=std::stoi(this->findChild<QLineEdit*>("Font Weight:")->text().toStdString());

    settings["font-opacity"]=std::stoi(this->findChild<QLineEdit*>("Font Opacity:")->text().toStdString());

    std::ofstream file(fullSubConfigPath);
    file << settings.dump(4);
    file.close();

  }
  
  void loadconfig(){
    json configfile;
    std::ifstream file(fullSubConfigPath);
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
      this->findChild<QLineEdit*>("Font Weight:")->setText(QString::number(configfile["font-weight"].get<int>()));

      selected_font = QString::fromStdString(configfile["font-familly"]);

      file.close();
    }else{
      std::cerr << "[ WARNING ] Cannot find subtitles config File: "<<fullSubConfigPath<<"\n";
    }
  }

  QString makehtml(){
    if(!std::filesystem::exists(fullSubConfigPath)){
      createdefaultjson();
    }

    json settings;
    std::ifstream file(fullSubConfigPath);
    if(file){
      file>>settings;

      QColor bgcolor(QString::fromStdString(settings["bg-color"]));
      QColor fontcolor(QString::fromStdString(settings["font-color"]));

      QString backgroundColorRGB = QString::number(bgcolor.red()) + ", " + QString::number(bgcolor.green()) + ", " + QString::number(bgcolor.blue());
      QString backgroundColorAlpha = QString::number(static_cast<double>(settings["bg-opacity"]) / 100, 'f', 2);
      QString fontColorRGB = QString::number(fontcolor.red()) + ", " + QString::number(fontcolor.green()) + ", " + QString::number(fontcolor.blue());
      QString fontColorAlpha = QString::number(static_cast<double>(settings["font-opacity"]) / 100, 'f', 2);
      QString fontWeight = QString::number(static_cast<double>(settings["font-weight"]), 'f', 2);
      QString paddingValue = QString::number(static_cast<double>(settings["padding"]), 'f', 2);

      QString HtmlScript = 
        "<table cellpadding='" + paddingValue + "' "
          "style='"
            "background-color: rgba(" + backgroundColorRGB + ", " + backgroundColorAlpha + "); "
            "color: rgba(" + fontColorRGB + ", " + fontColorAlpha + "); "
            "font-family: \"" + QString::fromStdString(settings["font-familly"]) + "\"; "
            "font-size: " + QString::number(static_cast<int>(settings["font-size"])) + "px;"
            "font-weight: " + fontWeight + "'>"
            "<tr><td>";

      padding = settings["padding"];
      marginbottom = settings["margin-bottom"];

      file.close();
      return HtmlScript;
    }else{
      std::cerr << "[ WARNING ] Failed to open subtitles config File: "<<fullSubConfigPath<<"\n";
      return "";
    }
  }

  void loadFonts(){
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
    defaultsettings["font-weight"]= 300;
    defaultsettings["font-size"]= 24;
    defaultsettings["margin-bottom"]= 61;
    defaultsettings["padding"]= 26;
   
    std::ofstream file(fullSubConfigPath);
    if(file){
      file << defaultsettings.dump(4);
      file.close();
    }else{
      std::cerr << "[ WARNING ] Cannot create a default subtitles config File: "<<fullSubConfigPath<<"\n";
    }
  }
};

#endif
