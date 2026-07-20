#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QUrl>
#include <QString>
#include <QKeySequence>
#include <QPointF>

#include <vector>
#include <string>
#include <map>

#include "Paths.h"
#include "../DialogWindows/SRepeatWindow.h"
#include "../DialogWindows/JumpToTime.h"
#include "../DialogWindows/AudioTracksManager.h"
#include "../DialogWindows/PlaylistManager.h"
#include "../DialogWindows/ShortcutsInstructions.h"
#include "../DialogWindows/ChangeThemeWindow.h"
#include "../DialogWindows/MediaUrl.h"
#include "../DialogWindows/LoadedSubsWindow.h"
#include "../DialogWindows/SubsConfigsWindow.h"
#include "../UiComponents/TopBar.h"
#include "../UiComponents/ContextMenu.h"
#include "../UiComponents/BottomControlPanel.h"
#include "../UiComponents/FloatingControlPannel.h"
#include "../UiComponents/SubtitlesItem.h"
#include "../utils/clearVector.h"
#include "../utils/stringTreatment.h"

class QLabel;
class QStackedLayout;
class QVBoxLayout;
class QGridLayout;
class QGraphicsVideoItem;
class QGraphicsView;
class QGraphicsScene;
class QShortcut;
class QProcess;
class QAction;
class QTimer;
class QGraphicsWidget;
class QLayout;
class QResizeEvent;
class QMouseEvent;
class QEvent;
class QAudioOutput;

struct MetaDataTrack;

struct SubObject{
  float startTime;
  std::string textContent;
  float endTime;
};

class MainWindow: public QMainWindow {
  Q_OBJECT
  static inline int MAINWINDOW_BORDERS_MARGIN = 10;
  static inline int TIME_BEFORE_HIDING_CURSOR = 800;
  static inline int TIME_BEFORE_HIDING_FLOATING_PANNEL = 800;

  public:
    enum PlayerMode {
      Single,
      Playlist
    };

    MainWindow(QWidget *parent=nullptr);
    ~MainWindow();
    void setPlayerDefaultState();
    void playNextVideoInPlaylist();

  // ui creation
  private:
    void setupWindow();
    void initMediaElements();
    void setupMainLayout();
    void createTopLayout();
    void setupSceneAndView();
    void createBottomLayout();
    void connectPlayerSignals();
    void createTimestampIndicator();

    void onToolMenuAction(int buttonindex);
    void controlButtonsHandler(int buttonindex);

    void setVideoSliderRange(qint64 playbackPosition);
    void playbackPositionUpdated(qint64 playbackPosition);
    void updateTimeLabels();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

    void startVideoPlayer(QUrl videoPath, QString videoTitle);
    void prepareVideoFile(QString path);
    void prepareVideoURL(QString videoUrl);
    void cleanSubtitles();

    void determineNextVideo();
    void changePlayBackPosition(int newpos);
    void closeVideo();

    void setVolumeSliderPosition(qreal position);
    void toggleVolume();

    // Meta Data logic
    void getSubtitleTracksFromMetaData();
    void getAudioTracksFromMetaData();

    // subtiles logic
    void loadSubtitleStyle();
    void toggleSubtitles();
    void increaseSubtitlesDelay();
    void decreaseSubtitlesDelay();
    void SubFileParsing(std::string subpath);
    void parsingSrtLikeSubsFile(std::string subpath);
    void parsingAssSubsFile(std::string subpath);
    void LoadingInDirectorySubtitles(QString currenturl);
    void ExtractingBuiltInSubs(QString currenturl);
    void setCurrentLoadedSubPath(QString fileSubPath);
    void syncSubtitles(qint64 playbackPosition);
    void onffmpegProcessFinshed(int currentProcessIndex, QString fileSubPath);
    void killAllSubtitlesExtractionProcesses();

    // chapters logic
    void toggleChaptersIndicators();
    int findChapterIndexByTime(int time);
    int findCurrentChapterIndex();
    void moveToNextChapter();
    void moveToPrevChapter();
    void ExtractingChapterData(QString currenturl);

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
    void updateFullscreenIcon();
    void updatePlaybackSpeedIcon();

    // global functions
    void setupShortcuts();
    void resizeEvent(QResizeEvent * event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    bool handleFloatingPannelDisplaying(QObject *obj, QEvent *event);
    bool handleTimestampIndicator(QEvent* event);
    void handleCursorHiding(QEvent* event);
    void handleDoubleLeftClick(QMouseEvent * event);

    void enterFullScreen();
    void exitFullScreen();
    void toggleFullScreen();
    bool mouseInsideFloatingPanel(QEvent* event);
    void closeHideMouseTimer();

    void renderOverlayText(std::string textToRender, TextPosition position, int animationduration);
    void setTopbarLayoutVisible(bool visible);
    void moveSomethingToPos(QGraphicsWidget *widget, QPointF targetPos, int animationTime);
    void setUniformMargins(QLayout *layout, int margin);

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

    // media player variables
    QMediaPlayer *player;
    QGraphicsVideoItem *video;
    QAudioOutput *audio;

    // Meta Data variables
    std::vector<MetaDataTrack*> subTracksMetaDataVector;
    std::vector<MetaDataTrack*> audioTracksMetaDataVector;
    std::vector<QProcess*> subExtractionProcessesList;

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
    bool showChaptersIndicators = true;
    bool MouseIsInsideFloatingPanel = false;
    bool contextMenuOpened = false;

    // ui variables
    QLabel* image;
    QWidget *mainwidget;
    QStackedLayout *stackedlayout;
    QVBoxLayout *mainlayout;
    TopBar *topbarlayout=nullptr;
    QGridLayout *videolayout;
    BottomControlPanel *controlbuttonslayout=nullptr;
    SubtitlesItem *subtitlesItem;
    QGraphicsView *view;
    QGraphicsScene *scene;
    TextTimer* timestampIndicator;

    // floating control pannel ui variables 
    FloatingControlPannel* floatingControlPannel;

    // running video variables
    std::string currentVideoParentDirectory;
    QString currentVideoUrl = "";
    std::string currentVideoTitle="";

    // buttons Objects
    std::vector <QAction*> toolMenuActionsObjectsList= {};

    // subtiles ui variables
    int subpadding;
    int subBottomMargin;
    QString htmlstyle;
    TextItem *overlayTextItem = nullptr;

    // subtiles logic variables
    int currentSubDelay=0;
    std::vector <SubObject*> currentLoadedSubList;
    QString currentLoadedSubPath = "";

    int lastPlaybackPosition=0;
    std::vector <ChapterObject> ChaptersVectors = {};

    //default Values
    std::map<std::string,float> Settings;

    QTimer* hideMouseTimer = nullptr;
    std::vector<QShortcut*> qShortcutsObjs;
    std::map<std::string, QKeySequence> currentShortcuts = {};

  public:
    PATHS *SYSTEMPATHS = new PATHS();
    std::vector<QUrl> playlist;
    std::vector <QString> currentVideoSubtitlePaths = {};
    PlayerMode currentPlayerMode;
};

#endif
