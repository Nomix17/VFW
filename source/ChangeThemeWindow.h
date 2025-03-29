#ifndef CHANGETHEME_W
#define CHANGETHEME_W


#include <fstream>
#include <sstream>
#include <filesystem>

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QAction>

class ChangeThemeWindow:public QDialog{

  Q_OBJECT;
  public:
    std::string changetotheme="";

    ChangeThemeWindow(QWidget* parent, std::string ConfigDirectory, std::string ProjectDirectory, std::string StyleDirectory):QDialog(parent){
      
      //load style file
      std::ifstream stylefile(StyleDirectory+"/ChangeThemeWindow.css");
      if(stylefile){
        std::ostringstream sstr;
        stylefile>>sstr.rdbuf();
        std::string script = sstr.str();
        this->setStyleSheet(QString::fromStdString(script));
        stylefile.close();
      }
      
      //creating ui elements
      QGridLayout* mainlayout= new QGridLayout(this);
      QToolButton* themetoolbutton = new QToolButton;
      QPushButton* doneButton = new QPushButton("OK");
      QMenu* menu = new QMenu(this);
      themetoolbutton->setText("theme");
      themetoolbutton->setPopupMode(QToolButton::InstantPopup);
      
      //loop the files that exist in the style directory so list all the themes availble
      for(auto& file : std::filesystem::directory_iterator(ProjectDirectory)){
        std::string themename = file.path().filename().string();
        //create action for each theme
        QAction* themeactions = new QAction;
        themeactions->setText(QString::fromStdString(themename));
        menu->addAction(themeactions);
        connect(themeactions,&QAction::triggered,[this,ConfigDirectory,themename,themetoolbutton](){//changing the value of changetotheme to the new theme value
          changetotheme = themename;
          themetoolbutton->setText(QString::fromStdString(changetotheme));
        });
      }
      themetoolbutton->setMenu(menu);
      connect(doneButton,&QPushButton::clicked,[this,ConfigDirectory](){
        //if the user changed the theme
        if(changetotheme!=""){
          //open and clear the theme file
          std::ofstream themefile(ConfigDirectory+"/theme",std::ofstream::out | std::ofstream::trunc);
          themefile<<changetotheme;//write the new theme in the theme file
          themefile.close();
        }
        QDialog::accept();//close the dialog window
      });
      
      //connecting ui elements
      mainlayout->setAlignment(Qt::AlignCenter);
      mainlayout->addWidget(themetoolbutton,0,0,Qt::AlignCenter);
      mainlayout->addWidget(doneButton,1,0,Qt::AlignCenter);
      setLayout(mainlayout);
    }
};

#endif
