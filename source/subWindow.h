#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QString>
#include <QUrl>
#include <QPixmap>

class subWindow:public QDialog{
  Q_OBJECT

  public:
    subWindow(std::string StyleDirectory, std::string IconDirectory, std::vector <QString> subArray, QString currentLoadedSubPath,QWidget *parent = nullptr):QDialog(parent){
      this->setFixedSize(400,500);
      std::filesystem::path styleFullPath(std::filesystem::path(StyleDirectory) / "playlistmanager.css");
      std::ifstream stylefile(styleFullPath.string());
      if(stylefile){
        std::string script;
        std::ostringstream sstr;
        sstr<<stylefile.rdbuf();
        script = sstr.str();
        this->setStyleSheet(QString::fromStdString(script));
      }

      createbuttons(IconDirectory ,subArray,currentLoadedSubPath);

      scrollarea->setWidgetResizable(true);
      scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
      holderwidget->setLayout(subLayout);
      holderwidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
      holderwidget->setMinimumWidth(300);
  
      scrollarea->setWidget(holderwidget);
      // Add scroll area directly to main layout
      mainlayout->addWidget(scrollarea);
  
      // Done button setup
      doneButton->setObjectName("donebutton");
      doneButtonHolder->addStretch();
      doneButtonHolder->addWidget(doneButton);
      doneButtonHolder->addStretch();
  
      mainlayout->addLayout(doneButtonHolder);
  
      setLayout(mainlayout);
  }

  void createbuttons(std::string IconDirectory,std::vector<QString> subArray,QString currentLoadedSubPath){

      //clearing old buttons
      for(int i=0;i<subLayout->count();i++){
        QLayoutItem *item = subLayout->itemAt(i);
        if(item->widget()){
          delete item->widget();
        }
      }
      //create buttons based on the medias loaded into the playlist
      for(size_t i=0;i<subArray.size();i++){
        QString sub_path = subArray[i];

        //getting the sub title
        std::string sub_title = "sub "+std::to_string(i);

        //creating a button that represent video
        QPushButton *Sub_Button= new QPushButton(QString::fromStdString("  "+sub_title));
        Sub_Button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        std::filesystem::path iconFullPath(std::filesystem::path(IconDirectory) / "sub.png");
        Sub_Button->setIcon(QPixmap(QString::fromStdString(iconFullPath.string())));
        if(sub_path == currentLoadedSubPath){
          Sub_Button->setObjectName("currentlyplayingbutton");
        }

        connect(Sub_Button,&QPushButton::clicked,[this,sub_path](){
          clickedSubPath = sub_path;
          QDialog::accept();
        });

        subLayout->addWidget(Sub_Button,i,0);
        subLayout->setRowStretch(99, 1);
      }

      connect(doneButton,&QPushButton::clicked,[this](){
        QDialog::accept();
      });
    }
    QString clickedSubPath = "";
  private:
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QScrollArea *scrollarea = new QScrollArea;
    QGridLayout *subLayout = new QGridLayout;
    QVBoxLayout *scrollarealayoutholder = new QVBoxLayout;
    QHBoxLayout *doneButtonHolder = new QHBoxLayout;
    QPushButton *doneButton = new QPushButton("Exit");
    QWidget *holderwidget = new QWidget;

};


#endif



