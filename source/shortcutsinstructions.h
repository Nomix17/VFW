#ifndef SHORTCUTS_INST
#define SHORTCUTS_INST

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>

#include <QDialog>
#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ShortcutsInst:public QDialog{
  Q_OBJECT;
  private:
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QGridLayout *tablelayout = new QGridLayout;
    QHBoxLayout *donebuttonlayout = new QHBoxLayout;
    QPushButton *donebutton = new QPushButton("OK");

  public:
    ShortcutsInst(QWidget *parent,std::string StyleDirectory,std::string ConfigDirectory):QDialog(parent){
      this->setWindowTitle("Shortcuts");

      //load style file
      std::filesystem::path styleFullPath(std::filesystem::path(StyleDirectory) / "shortcutsinstructions.css");
      std::ifstream stylefile(styleFullPath);
      if(stylefile){
        std::ostringstream sstr;
        std::string script;
        sstr<<stylefile.rdbuf();
        script = sstr.str();
        this->setStyleSheet(QString::fromStdString(script));
        stylefile.close();
      }
     
      std::filesystem::path fullShortcutFilename(std::filesystem::path(ConfigDirectory) / "Shortcuts_Instructions");
      std::ifstream shortcutfile(fullShortcutFilename.string());
      std::string shortcutsfilecontent;
      std::vector <std::string> elements;

      //checking if the file exist
      if (shortcutfile) {
        std::string line;
        //we read the file line by line and add the configs into the vector 
        while(getline(shortcutfile,line)){
          elements.push_back(line);
        }

      } else {
        //if it's not found we create a new one 
        std::ofstream defaultshortcutsfile(fullShortcutFilename.string());

        //put the default configs into the vector
        elements={
            "Functionality:Shortcut",
            "FullScreen/Unfullscreen:F",
            "Unfullscreen:Escape",
            "Pause/Unpause:Space",
            "Go Back:LeftArowkey",
            "Go Forward:RightArowkey",
            "Mute/Unmute:M",
            "Volume Up:UpArrowKey",
            "Volume Down:DownArrowKey",
            "Reduce Video Delay:G",
            "Increase Video Delay:H"
        };

        //pushing the default config in the file
        for(size_t i=0;i<elements.size();i++) defaultshortcutsfile << elements[i]+'\n';
        defaultshortcutsfile.close();
      }

      //looping the elements of the vectore to create widgets based on them
      for(size_t i=0;i<elements.size();i++){
        std::stringstream line(elements[i]);
        std::string shortcutFunctionality;
        std::string shortcutKeys;

        std::getline(line, shortcutFunctionality,':');
        std::getline(line, shortcutKeys,':');

        QLabel *functionalitylabel = new QLabel(QString::fromStdString(shortcutFunctionality));
        QLabel *keyslabel = new QLabel(QString::fromStdString(shortcutKeys));

        if (i==0) {
          functionalitylabel->setObjectName("tablehead");
          keyslabel->setObjectName("tablehead");
        }

        tablelayout->addWidget(functionalitylabel,i,0);
        tablelayout->addWidget(keyslabel,i,1);
      }
      
      connect(donebutton,&QPushButton::clicked,[this](){
        QDialog::accept();
      });
      donebuttonlayout->addWidget(donebutton);

      mainlayout->addLayout(tablelayout);
      mainlayout->addLayout(donebuttonlayout);
      setLayout(mainlayout);
    }
};


#endif
