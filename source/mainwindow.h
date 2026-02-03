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
#include "jumptotime.h"
#include "playlistmanager.h"
#include "shortcutsinstructions.h"
#include "ChangeThemeWindow.h"
#include "mediaurl.h"
#include "subWindow.h"
#include "subconfig.h"
#include "CustomObjects.h"
#include "topBar.h"
#include "bottomControlPanel.h"
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
  void setupShortcuts();
  void createTopLayout();
  void createBottomLayout();
  void topBarButtonsHandler(int buttonindex);
  void controlButtonsHandler(int buttonindex);
  void setVideoSliderRange(qint64 playbackPosition);
  void syncSubtitles(qint64 playbackPosition);
  void playbackPositionUpdated(qint64 playbackPosition);
  void determineNextVideo();
  void updateTimeLabels();
  void setPlayerDefaultState();
  void playNextVideoInPlaylist();
  void startVideoPlayer(QString url);
  void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
  void setVolumeSliderPosition(qreal position);
  void closeVideo();
  void toggleSubtitles();
  void toggleChaptersIndicators();
  void moveToNextChapter();
  void moveToPrevChapter();
  void SubFileParsing(std::string subpath);
  void parsingSrtLikeSubsFile(std::string subpath);
  void parsingAssSubsFile(std::string subpath);

  void changingposition(int newpos);
  void resizelements(std::string elementtorezise="all",int animationTime = 0);

  void resizeEvent(QResizeEvent * event) override;
  void mouseDoubleClickEvent(QMouseEvent * event)override;
  bool eventFilter(QObject *obj, QEvent *event) override;

  void FullScreen();
  bool mouseInsideFloatingPanel(QEvent* event);

  void showingthings(std::string texttoshow, int xposition, int yposition,int animationduration);
  void topbarlayoutvisibility(std::string status);
  void updateButtonsIcon(std::string button_name = "all");
  void savevideoposition();
  void getlastsavedposition();
  void ExtranctingChapterData(QString currenturl);
  void LoadingInDirectorySubtitles(QString currenturl);
  void ExtractingBuiltInSubs(QString currenturl);
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
