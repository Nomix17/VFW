#ifndef BOTTOMCONTROLPANEL
#define BOTTOMCONTROLPANEL

#include "qboxlayout.h"
#include <iomanip>
#include <filesystem>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "CustomObjects.h"
#include "textTimer.h"

class BottomControlPanel: public QVBoxLayout {
  Q_OBJECT
  
  public:
    enum RepeatMode{
      PlaylistRepeat,
      VideoRepeat,
      Shuffle,
    };

    enum topbarlayout_LABEL_ELEMENTS{
      PAUSE_BUTTON,
      BACK_BUTTON,
      STOP_BUTTON,
      NEXT_BUTTON,
      FULLSCREEN_BUTTON,
      PLAYLIST_BUTTON,
      REPETITION_BUTTON,
      CONTINUE_FROM_LAST_POS_BUTTON,
      TOGGLE_VOLUME_BUTTON
    };

  signals:
     void volumeSliderMoved(int value);
     void videoSliderMoved(int value);
     void controlButtonsHandler(int buttonNumber);

  private:
    QList<QString> mcbuttons = {"BPause","BBack","BStop","BNext","BFullscreen","BPlaylist","BRepeating","BContinueFLP","BVolumeControl"};
    std::vector <QPushButton*> ControlButtonsObjectsList = {};
    QString ICONSDIRECTORY;

    CustomSlider *volumeslider;
    TextTimer *durationTimer;
    TextTimer *playbackPositionTimer;
    CustomSlider *videoslider;

  public:
    BottomControlPanel(std::string ICONSDIRECTORY, QWidget *parent = nullptr):QVBoxLayout(parent){
      this->ICONSDIRECTORY = QString::fromStdString(ICONSDIRECTORY);
      QHBoxLayout *firsthalflayout = new QHBoxLayout();
      QHBoxLayout *secondhalflayout = new QHBoxLayout();

      videoslider = new CustomSlider(Qt::Horizontal);// creating video slider
      playbackPositionTimer = new TextTimer();
      durationTimer = new TextTimer();
      volumeslider = new CustomSlider(Qt::Horizontal);// create volume slider
      volumeslider->setObjectName("volumeslider");
      volumeslider->setRange(0, 1000);

      connect(videoslider, &QSlider::sliderMoved,[this](){
        videoSliderMoved(videoslider->sliderPosition());
      });
      connect(volumeslider, &QSlider::valueChanged,[this](int value){
        updateVolumeButtonIcon();
        volumeSliderMoved(value);
      });

      firsthalflayout->addWidget(playbackPositionTimer);
      firsthalflayout->addWidget(videoslider);
      firsthalflayout->addWidget(durationTimer);

      createControlButtons(secondhalflayout);
      secondhalflayout->addWidget(volumeslider);

      this->addLayout(firsthalflayout);
      this->addLayout(secondhalflayout);

      this->setAlignment(Qt::AlignLeft);
    }

    void createControlButtons(QHBoxLayout *parentLayout) {
      for (int j = 0; j < mcbuttons.size(); j++) {
        // adding space for the style
        if (j == BACK_BUTTON || j == FULLSCREEN_BUTTON || j == CONTINUE_FROM_LAST_POS_BUTTON) {
          parentLayout->addSpacing(20);
        } else if (j == TOGGLE_VOLUME_BUTTON) {
          // adding space for the style between buttons and volume parameters
          parentLayout->addStretch(1);
        }

        QPushButton *newButton = new QPushButton();
        newButton->setObjectName(mcbuttons[j]);

        std::string iconName = mcbuttons[j].toStdString() + ".png";
        std::filesystem::path iconFullPath(std::filesystem::path(ICONSDIRECTORY.toStdString()) / std::filesystem::path(iconName));
        QPixmap pix(QString::fromStdString(iconFullPath.string()));
        newButton->setIcon(pix);
        if (mcbuttons[j] == "BVolumeControl") {
          newButton->setIconSize(QSize(24, 24));
        } else {
          newButton->setIconSize(QSize(16, 16));
        }
        if(mcbuttons[j] == "BContinueFLP"){
          newButton->hide();
        }

        connect(newButton, &QPushButton::clicked, [this, j]() {
          controlButtonsHandler(j);
        });

        ControlButtonsObjectsList.push_back(newButton);

        parentLayout->addWidget(newButton);
      }
    }

    // Volume Slider Setters
    void setVolumeSliderRange(int from, int to) {
      volumeslider->setRange(from,to);
    }
    void setVolumeSliderPosition(float position) {
      volumeslider->setSliderPosition(position);
    }

    // Video Slider Setters
    void setVideoSliderRange(int from, int to) {
      videoslider->setRange(from,to);
    }
    void setVideoSliderValue(int position) {
      videoslider->setValue(position);
    }

    int getVolumeValue(){
      return volumeslider->value();
    }

    void setChaptersMarks(std::vector <ChapterObject> ChaptersVectors, bool showChaptersIndicators){
      videoslider->setChaptersMarks(ChaptersVectors, showChaptersIndicators);
    }

    void hideSkipButton(){
      QPushButton *SkipButton = ControlButtonsObjectsList[CONTINUE_FROM_LAST_POS_BUTTON];
      SkipButton->hide();
    }
    void showSkipButton(){
      QPushButton *SkipButton = ControlButtonsObjectsList[CONTINUE_FROM_LAST_POS_BUTTON];
      SkipButton->show();
    }

    void updatePlayBackTimer(int position){
      playbackPositionTimer->setValue(position);
    }
    void updateDurationTimer(int position){
      durationTimer->setValue(position);
    }
    
    void setDefaultState(){
      durationTimer->setToDefaultState();
      playbackPositionTimer->setToDefaultState();
      volumeslider->setRange(0, 1000);
      hideSkipButton();
    }

    std::vector <QPushButton*> getControlButtonsObjects() {
      return ControlButtonsObjectsList;
    }

    void updatePausePlayButtonIcon(bool videoIsPaused) {
      QPushButton *Pause_button = ControlButtonsObjectsList[PAUSE_BUTTON];
      if (videoIsPaused) Pause_button->setIcon(QPixmap(ICONSDIRECTORY + "/BPause.png"));
      else Pause_button->setIcon(QPixmap(ICONSDIRECTORY + "/BPlay.png"));
    }

    void updateVolumeButtonIcon() {
      int volume = volumeslider->value();
      QPushButton *VolumeControlButton = ControlButtonsObjectsList[TOGGLE_VOLUME_BUTTON];
      std::string iconFileName;
      if (volume == 0) iconFileName = "BMute.png";
      else if (volume <= 333 && volume > 0) iconFileName = "BVolumeLow.png";
      else if (volume >= 333 && volume <= 666) iconFileName = "BVolumeMid.png";
      else if (volume >= 666) iconFileName = "BVolumeControl.png";

      std::filesystem::path newIconFullPath(std::filesystem::path(ICONSDIRECTORY.toStdString()) / std::filesystem::path(iconFileName));
      VolumeControlButton->setIcon(QPixmap(QString::fromStdString(newIconFullPath.string())));
    }

    void updateRepetitionButtonIcon(RepeatMode rep) {
      QPushButton *Repeatition_button = ControlButtonsObjectsList[REPETITION_BUTTON];
      std::string iconFileName;
      if (rep == PlaylistRepeat) iconFileName = "BRepeating.png";
      else if (rep == VideoRepeat) iconFileName = "BRepeatingone.png";
      else if (rep == Shuffle) iconFileName = "BSuffle.png";

      std::filesystem::path newIconFullPath(std::filesystem::path(ICONSDIRECTORY.toStdString()) / std::filesystem::path(iconFileName));
      Repeatition_button->setIcon(QPixmap(QString::fromStdString(newIconFullPath.string())));
    }

    void deletelayout(QLayout* layout){
      if(!layout) return;

      while(QLayoutItem* item = layout->takeAt(0)){
        if(QLayout* childlayout = item->layout()){
          deletelayout(childlayout);
        }

        if(QWidget* childwidget = item->widget()){
         childwidget->deleteLater();
        }

        delete item;
      }

    }

    ~BottomControlPanel(){
      deletelayout(this);
      delete this->volumeslider;
      delete this->playbackPositionTimer;
      delete this->durationTimer;
    }
};


#endif
