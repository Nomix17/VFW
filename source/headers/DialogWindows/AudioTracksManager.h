#ifndef AUDIOTRACKSMANAGER
#define AUDIOTRACKSMANAGER

#include <iostream>
#include <string>
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

struct MetaDataTrack{
  std::string title;
  std::string language;
  int index;
};

class AudioTracksManager:public QDialog{
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
    int newAudioTrackIndex = -1;

    AudioTracksManager(
        std::string StyleDirectory,
        std::string IconDirectory,
        std::vector<MetaDataTrack*> audioTracksMetaDataVector,
        int currentAudioTrackIndex,
        QWidget *parent = nullptr
    ):QDialog(parent){

      this->setWindowTitle("Audio Tracks");
      //load style file
      std::filesystem::path styleFullPath(std::filesystem::path(StyleDirectory) / "playlistmanager.css");
      std::ifstream stylefile(styleFullPath);
      if(stylefile){
        std::string script;
        std::ostringstream sstr;
        sstr<<stylefile.rdbuf();
        script = sstr.str();
        this->setStyleSheet(QString::fromStdString(script));
      }

      //calling function to create buttons
      createbuttons(IconDirectory,audioTracksMetaDataVector,currentAudioTrackIndex);

      //configuring the scrollarea
      this->setFixedSize(400,500);
      scrollarea->setWidgetResizable(true);
      scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      scrollarea->setFocusPolicy(Qt::NoFocus);

      holderwidget->setLayout(medialayout);
      holderwidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
      holderwidget->setMinimumWidth(300);
      holderwidget->setFocusPolicy(Qt::NoFocus);

      scrollarea->setWidget(holderwidget);
      // Add scroll area directly to main layout
      mainlayout->addWidget(scrollarea);

      // Done button setup
      doneButton->setObjectName("donebutton");
      doneButton->setFocusPolicy(Qt::NoFocus);
      doneButtonHolder->addStretch();
      doneButtonHolder->addWidget(doneButton);
      doneButtonHolder->addStretch();

      mainlayout->addLayout(doneButtonHolder);

      setLayout(mainlayout);
    }

    void createbuttons(std::string IconDirectory, std::vector<MetaDataTrack*> audioTracksMetaDataVector, int currentAudioTrackIndex){

      //clearing old buttons
      for(int i=0;i<medialayout->count();i++){
        QLayoutItem *item = medialayout->itemAt(i);
        if(item->widget()){
          delete item->widget();
        }
      }

      //create buttons based on the medias loaded into the playlist
      for(MetaDataTrack* audioTrack : audioTracksMetaDataVector){
        std::string title = audioTrack->title;
        std::string language = audioTrack->language;
        int index = audioTrack->index;

        QString langUpper = QString::fromStdString(language);
        QString trackTitle = QString::fromStdString(title.empty() ? "Track " + std::to_string(index) : title);
        QString buttonTitle = " " + langUpper + "  •  " + trackTitle;

        //creating a button that represent video
        QPushButton *Video_Button= new QPushButton(buttonTitle);
        Video_Button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        Video_Button->setFocusPolicy(Qt::NoFocus);

        std::string iconName;
        if(index == currentAudioTrackIndex) {
          iconName = "BPause.png";
          Video_Button->setObjectName("currentlyplayingbutton");
        } else {
          iconName = "BPlay.png";
        }

        std::filesystem::path iconFullPath(std::filesystem::path(IconDirectory) / std::filesystem::path(iconName));
        Video_Button->setIcon(QPixmap(QString::fromStdString(iconFullPath.string())));

        connect(Video_Button,&QPushButton::clicked,[this,index](){
          newAudioTrackIndex = index;
          QDialog::accept();
        });

        medialayout->addWidget(Video_Button,index,0);
        medialayout->setRowStretch(99, 1);
      }

      connect(doneButton,&QPushButton::clicked,[this](){
        QDialog::accept();
      });
    }
};


#endif
