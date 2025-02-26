#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H
#include <iostream>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <vector>
#include <QUrl>
#include <QString>
#include <fstream>
#include <sstream>
#include <QPushButton>
#include <QPixmap>
#include <QScrollArea>

class PlaylistManager:public QDialog{
  Q_OBJECT;
  private:
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QScrollArea *scrollarea = new QScrollArea;
    QVBoxLayout *medialayout = new QVBoxLayout;
    QVBoxLayout *scrollarealayoutholder = new QVBoxLayout;
    QVBoxLayout *doneButtonlayout = new QVBoxLayout;
    QPushButton *doneButton = new QPushButton("Exit");
    QWidget *holderwidget = new QWidget;

  public:
    int new_video_index = -1;

    PlaylistManager(QWidget *parent,std::string styledirectory,std::string icondirectory,std::vector<QUrl> playlist_vector,QString currenturl):QDialog(parent){
      this->setFixedSize(640,480);

      //load style file
      std::ifstream stylefile(styledirectory+"playlistmanager.css");
      if(stylefile){
        std::string script;
        std::ostringstream sstr;
        sstr<<stylefile.rdbuf();
        script = sstr.str();
        this->setStyleSheet(QString::fromStdString(script));
      }

      //calling function to create buttons
      createbuttons(icondirectory,playlist_vector,currenturl);


      //configuring the scrollarea
      scrollarea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      scrollarea->setWidgetResizable(true);
      scrollarea->setWidget(holderwidget);
      scrollarealayoutholder->addWidget(scrollarea);

      doneButton->setObjectName("donebutton");
      holderwidget->setLayout(medialayout);
      doneButtonlayout->addWidget(doneButton);
      mainlayout->addLayout(scrollarealayoutholder);
      mainlayout->addLayout(doneButtonlayout);
      setLayout(mainlayout);
    }


    void createbuttons(std::string icondirectory,std::vector<QUrl> playlist_vector,QString currenturl){
      
      //clearing old buttons 
      for(int i=0;i<medialayout->count();i++){
        QLayoutItem *item = medialayout->itemAt(i);
        if(item->widget()){
          delete item->widget();
        }
      }
      //create buttons based on the medias loaded into the playlist
      for(size_t i=0;i<playlist_vector.size();i++){
        QString media_url = playlist_vector[i].toString();
        
        //getting the video title
        std::string video_title=""; 
        int counter = media_url.size();
        while(media_url[counter]!='/'){
            video_title+=media_url[counter].toLatin1();
            counter--;
        }
        std::reverse(video_title.begin(),video_title.end());//reversing the text so it look correct

        //creating a button that represent video
        QPushButton *Video_Button= new QPushButton(QString::fromStdString("  "+video_title));
        if(media_url == currenturl){
          Video_Button->setIcon(QPixmap(QString::fromStdString(icondirectory+"BPause.png")));
          Video_Button->setObjectName("currentlyplayingbutton");
        }else{
          Video_Button->setIcon(QPixmap(QString::fromStdString(icondirectory+"BPlay.png")));
        }
        
        connect(Video_Button,&QPushButton::clicked,[this,icondirectory,playlist_vector,currenturl,i](){
          new_video_index = i;            
          QDialog::accept();

        });

        medialayout->addWidget(Video_Button);
      }
      medialayout->addStretch();

      connect(doneButton,&QPushButton::clicked,[this](){
        QDialog::accept();
      });
    }
};


#endif