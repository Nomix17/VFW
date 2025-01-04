#ifndef SUBCONFIG
#define SUBCONFIG

#include <iostream>
#include <QDialog>
#include "json.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QSpinBox>
#include <QColorDialog>
#include <QList>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <fstream>

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
  void gui(){
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
            QList <QString> fonts= {"Arial","Serif","Sans-Serif","Monospace","Courier New","Cursive","Comic Sans MS"};
            QMenu *font_familly = new QMenu();
            for(int i=0;i<fonts.size();i++){
              QAction * actions = new QAction;
              actions->setText(fonts[i]);
              font_familly->addAction(actions);
              button->setMenu(font_familly);
              connect(actions,&QAction::triggered,[this,i,fonts,button](){
                selected_font = fonts[i];
                button->setText(fonts[i]);
              });
            }
            gridlayout->addWidget(button,i/2,(i%2)*2+1);
          }else{
            QSpinBox* number_picker = new QSpinBox();
            number_picker->setObjectName(labels[i]);

            number_picker->setRange(0,100);
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
    QFile file("/home/pain/.config/VFW/cache/styles/subconfig.css");
      if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
          QTextStream in(&file);
          QString styleSheet = in.readAll();
          this->setStyleSheet(styleSheet);
      }
  }

  void saveconfig(){

    json settings;

    settings["margin-bottom"]=this->findChild<QSpinBox*>("Margin Bottom:")->value();
    settings["padding"]=this->findChild<QSpinBox*>("Padding:")->value();
    
    settings["bg-color"]=this->findChild<QPushButton*>("BgColor")->palette().color(QPalette::Button).name().toStdString();
    settings["bg-opacity"]=this->findChild<QSpinBox*>("Bg Opacity:")->value();
    
    settings["font-size"]=this->findChild<QSpinBox*>("Font Size:")->value();
    settings["font-familly"]=selected_font.toStdString();
    
    settings["font-color"]=this->findChild<QPushButton*>("FontColor")->palette().color(QPalette::Button).name().toStdString();
    settings["font-opacity"]=this->findChild<QSpinBox*>("Font Opacity:")->value();
   


  std::ofstream file("/home/pain/.config/VFW/cache/styles/subconfig.json");
  file << settings.dump(10);
  file.close();

  }
  
  void loadconfig(){
    json configfile;
    std::ifstream file("/home/pain/.config/VFW/cache/styles/subconfig.json");
    file>>configfile;

    this->findChild<QSpinBox*>("Margin Bottom:")->setValue(configfile["margin-bottom"]);
    this->findChild<QSpinBox*>("Padding:")->setValue(configfile["padding"]);

    this->findChild<QPushButton*>("BgColor")->setStyleSheet("background-color:"+QString::fromStdString(configfile["bg-color"])+";");
    this->findChild<QSpinBox*>("Bg Opacity:")->setValue(configfile["bg-opacity"]);

    this->findChild<QSpinBox*>("Font Size:")->setValue(configfile["font-size"]);
    selected_font = QString::fromStdString(configfile["font-familly"]);
    this->findChild<QToolButton*>("Font Familly:")->setText(selected_font);

    this->findChild<QPushButton*>("FontColor")->setStyleSheet("background-color:"+QString::fromStdString(configfile["font-color"])+";");
    this->findChild<QSpinBox*>("Font Opacity:")->setValue(configfile["font-opacity"]);

    

    file.close();
  }

  QString makehtml(){
    json settings;
    std::ifstream file("/home/pain/.config/VFW/cache/styles/subconfig.json");
    file>>settings;
    QColor bgcolor(QString::fromStdString(settings["bg-color"]));
    QColor fontcolor(QString::fromStdString(settings["font-color"]));

    QString HtmlScript = "<div style='"
              
              "background-color: rgba(" + QString::number(bgcolor.red()) + ", " + QString::number(bgcolor.green()) + ", " + QString::number(bgcolor.blue()) + ", " + QString::number(static_cast<double>(settings["bg-opacity"]) / 100, 'f', 2) + "); "
              "color: rgba(" + QString::number(fontcolor.red()) + ", " + QString::number(fontcolor.green()) + ", " + QString::number(fontcolor.blue()) + ", " + QString::number(static_cast<double>(settings["font-opacity"]) / 100, 'f', 2) + "); "
              
              "font-family: " + QString::fromStdString(settings["font-familly"]) + "; "                    
              "font-size: " + QString::number(static_cast<int>(settings["font-size"])) + "px;'>";

    padding = settings["padding"];
    marginbottom = settings["margin-bottom"];

    file.close();
    return HtmlScript;
  }
};

#endif