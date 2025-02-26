#ifndef SHORTCUTS_INST
#define SHORTCUTS_INST
#include <QDialog>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

class ShortcutsInst:public QDialog{
  Q_OBJECT;
  private:
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QGridLayout *tablelayout = new QGridLayout;
    QHBoxLayout *donebuttonlayout = new QHBoxLayout;
    QPushButton *donebutton = new QPushButton("OK");

  public:
    ShortcutsInst(QWidget *parent,std::string stylepath,std::string configpath):QDialog(parent){
      this->setFocus();
      std::ifstream stylefile(stylepath+"shortcutsinstructions.css");
      std::ifstream shortcutfile(stylepath+"Shortcuts_Instructions");
      std::string shortcutsfilecontent;
      std::vector <std::string> elements;

      if(stylefile){
        std::ostringstream sstr;
        std::string script;
        sstr<<stylefile.rdbuf();
        script = sstr.str();
        this->setStyleSheet(QString::fromStdString(script));
        stylefile.close();
      }


      //checking if the file exist
      if(shortcutfile){
        std::string line;
        //we read the file line by line and add the configs into the vector 
        while(getline(shortcutfile,line)){
          elements.push_back(line);
        }
      }else{
        //if it's not found we create a new one 
        std::ofstream defaultshortcutsfile(configpath+"Shortcuts_Instructions");

        //put the default configs into the vector
        elements={"Functionality:Shortcut",
                  "FullScreen/Unfullscreen:F",
                  "Unfullscreen:Escape",
                  "Pause/Unpause:Space",
                  "Go Back:LeftArowkey",
                  "Go Forward:RightArowkey",
                  "Mute/Unmute:M",
                  "Volume Up:UpArrowKey",
                  "Volume Down:DownArrowKey",
                  "Reduce Video Delay:G",
                  "Increase Video Delay:H"};
              
        //pushing the default config in the file
        for(size_t i=0;i<elements.size();i++){
          defaultshortcutsfile << elements[i]+'\n';
        }
        defaultshortcutsfile.close();//closing the file
      }

      //looping the elements of the vectore to create widgets based on them
      for(size_t i=0;i<elements.size();i++){
        std::string shortcutfunctionality;
        std::string shortcutkeys;
        int counter=0;
        //looping the line to split it in too, the keys of the shortcut, and the functionality of it
        while(elements[i][counter]!=':'){
          shortcutfunctionality +=elements[i][counter];
          counter++;
        }
          counter++;
        while(elements[i][counter]!='\0'){
          shortcutkeys +=elements[i][counter];
          counter++;
        }
        QLabel *functionalitylabel = new QLabel(QString::fromStdString(shortcutfunctionality));
        QLabel *keyslabel = new QLabel(QString::fromStdString(shortcutkeys));
        if(i==0){
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