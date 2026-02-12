#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QUrl>
#include <QString>
#include <QLabel>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGraphicsTextItem>
#include <QGraphicsVideoItem>
#include <QGraphicsView>

#include <vector>
#include <string>
#include <map>

#include "SRepeatWindow.h"
#include "JumpToTime.h"
#include "PlaylistManager.h"
#include "ShortcutsInstructions.h"
#include "ChangeThemeWindow.h"
#include "MediaUrl.h"
#include "LoadedSubsWindow.h"
#include "SubsConfigsWindow.h"
#include "CustomObjects.h"
#include "TopBar.h"
#include "BottomControlPanel.h"
#include "Paths.h"

struct SubObject{
  float startTime;
  std::string textContent;
  float endTime;
};

// main window class
class MainWindow: public QMainWindow{
  Q_OBJECT
public:

  enum PlayerMode {
    Single,
    Playlist
  };

  MainWindow(QWidget *parent=nullptr);
  ~MainWindow();

  // top bottom layouts
  void createTopLayout();
  void createBottomLayout();
  void topBarButtonsHandler(int buttonindex);
  void controlButtonsHandler(int buttonindex);

  void setVideoSliderRange(qint64 playbackPosition);
  void playbackPositionUpdated(qint64 playbackPosition);
  void updateTimeLabels();
  void setPlayerDefaultState();
  void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

  void startVideoPlayer(QString url);
  void determineNextVideo();
  void playNextVideoInPlaylist();
  void changePlayBackPosition(int newpos);
  void closeVideo();

  void setVolumeSliderPosition(qreal position);
  void toggleVolume();

  // subtiles logic
  void toggleSubtitles();
  void increaseSubtitlesDelay();
  void decreaseSubtitlesDelay();
  void SubFileParsing(std::string subpath);
  void parsingSrtLikeSubsFile(std::string subpath);
  void parsingAssSubsFile(std::string subpath);
  void LoadingInDirectorySubtitles(QString currenturl);
  void ExtractingBuiltInSubs(QString currenturl);
  void syncSubtitles(qint64 playbackPosition);

  // chapters logic
  void toggleChaptersIndicators();
  void moveToNextChapter();
  void moveToPrevChapter();
  void ExtranctingChapterData(QString currenturl);

  // resize stuff
  void resizeMainUiElement();
  void resizeVideoContainers();
  void repositionFloatingControllPannel(int animationTime=0);
  void repositionSubtitles();

  // update icons
  void updateButtonsIcon();
  void updatePlayPauseButtonIcon();
  void updateVolumeButtonIcon();
  void updateRepeatButtonIcon();

  // global functions
  void setupShortcuts();
  void resizeEvent(QResizeEvent * event) override;
  void mouseDoubleClickEvent(QMouseEvent * event)override;
  bool eventFilter(QObject *obj, QEvent *event) override;

  void enterFullScreen();
  void exitFullScreen();
  void toggleFullScreen();
  bool mouseInsideFloatingPanel(QEvent* event);

  void showingthings(std::string texttoshow, int xposition, int yposition,int animationduration);
  void setTopbarLayoutVisible(bool visible);

  // caching and stuff
  void savevideoposition();
  void getlastsavedposition();
  void parseSettingsFile();
  void savingNewSettings();

  //turnning off the tab focusing
  bool focusNextPrevChild(bool next) override{
    if(next){}
    return false;
  }

private:
  // playback status variables
  bool videoIsPaused=false;
  bool fullScreenEnabled = false;
  size_t currentVideoIndex=0;

  bool segmentLoopEnabled = false;
  int segmentLoopStart;
  int segmentLoopEnd;

  Qt::AspectRatioMode CurrentAspectMode = Qt::KeepAspectRatio;
  BottomControlPanel::RepeatMode  rep = BottomControlPanel::PlaylistRepeat;

  // ui Status variables
  bool ShowSubs = true;
  bool showChaptersIndicators = false;
  bool MouseIsInsideFloatingPanel = false;

  // ui variables
  QLabel* image;
  QWidget *mainwidget;
  QStackedLayout *stackedlayout;
  QVBoxLayout *mainlayout;
  TopBar *topbarlayout=nullptr;
  QGridLayout *videolayout;
  BottomControlPanel *controlbuttonslayout=nullptr;
  QGraphicsTextItem *sublabel;
  QMediaPlayer *player;
  QGraphicsVideoItem *video;
  QAudioOutput *audio;
  QGraphicsView *view;
  QGraphicsScene *scene;

  // floating control pannel ui variables 
  QWidget* floatingControlPannelWidget;
  QVBoxLayout * floatingControlPannelContainerLayout;
  QGraphicsProxyWidget * floatingControlPannelProxy;

  // running video variables
  std::string currentVideoParentDirectory;
  QString currentVideoUrl = "";
  std::string currentVideoTitle="";

  // buttons Objects
  std::vector <QAction*> TopBarButtonsObjectList= {};
  std::vector <QPushButton*> controlButtonsObjects = {}; // no longer used, might use it later, Idk I'll just leave it here

  // subtiles ui variables
  int subpadding;
  int subBottomMargin;
  int currentSubBottomSpace;
  QString htmlstyle;
  QGraphicsTextItem *toshowtext = nullptr;

  // subtiles logic variables
  int currentSubDelay=0;
  std::vector <SubObject*> currentLoadedSubList;
  QString currentLoadedSubPath = "";

  int lastPlaybackPosition=0;
  std::vector <ChapterObject> ChaptersVectors = {};

  //default Values
  std::map<std::string,float> Settings;

public:
  PATHS *SYSTEMPATHS = new PATHS();
  std::vector<QUrl> playlist;
  std::vector <QString> currentVideoSubtitlePaths = {};
  PlayerMode currentPlayerMode;
};

#endif
