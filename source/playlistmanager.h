#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QString>
#include <QUrl>
#include <QPixmap>

class PlaylistManager:public QDialog{
  Q_OBJECT;
  private:
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QScrollArea *scrollarea = new QScrollArea;
    QGridLayout *medialayout = new QGridLayout;
    QVBoxLayout *scrollarealayoutholder = new QVBoxLayout;
    QHBoxLayout *doneButtonHolder = new QHBoxLayout;
    QPushButton *doneButton = new QPushButton("Exit");
    QWidget *holderwidget = new QWidget;

  public:
    int new_video_index = -1;

    PlaylistManager(QWidget *parent,std::string StyleDirectory,std::string IconDirectory,std::vector<QUrl> playlist_vector,QString currenturl):QDialog(parent){

      //load style file
      std::ifstream stylefile(StyleDirectory+"playlistmanager.css");
      if(stylefile){
        std::string script;
        std::ostringstream sstr;
        sstr<<stylefile.rdbuf();
        script = sstr.str();
        this->setStyleSheet(QString::fromStdString(script));
      }

      //calling function to create buttons
      createbuttons(IconDirectory,playlist_vector,currenturl);

      //configuring the scrollarea
      this->setFixedSize(400,500);
      scrollarea->setWidgetResizable(true);
      scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
      holderwidget->setLayout(medialayout);
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


    void createbuttons(std::string IconDirectory,std::vector<QUrl> playlist_vector,QString currenturl){

      //clearing old buttons
      for(int i=0;i<medialayout->count();i++){
        QLayoutItem *item = medialayout->itemAt(i);
        if(item->widget()){
          delete item->widget();
        }
      }
      //create buttons based on the medias loaded into the playlist
      for(size_t i=0;i<playlist_vector.size();i++){
        QString media_url = playlist_vector[i].toLocalFile();

        //getting the video title
        int slashPosition = media_url.toStdString().rfind("/");
        std::string video_title = media_url.toStdString().substr(slashPosition+1);
        if(video_title.size() > 50){
          video_title = video_title.substr(0,47);
          video_title = video_title + "..."; 
        } 
        //creating a button that represent video
        QPushButton *Video_Button= new QPushButton(QString::fromStdString("  "+video_title));
        Video_Button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        if(media_url == currenturl){
          Video_Button->setIcon(QPixmap(QString::fromStdString(IconDirectory+"BPause.png")));
          Video_Button->setObjectName("currentlyplayingbutton");
        }else{
          Video_Button->setIcon(QPixmap(QString::fromStdString(IconDirectory+"BPlay.png")));
        }

        connect(Video_Button,&QPushButton::clicked,[this,IconDirectory,playlist_vector,currenturl,i](){
          new_video_index = i;
          QDialog::accept();
        });

        medialayout->addWidget(Video_Button,i,0);
        medialayout->setRowStretch(99, 1);
      }

      connect(doneButton,&QPushButton::clicked,[this](){
        QDialog::accept();
      });
    }
};


#endif
