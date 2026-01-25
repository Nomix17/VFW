#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CustomObjects.h"
#include <QApplication>
#include <QShortcut>
#include <QStyleHints>
#include "qboxlayout.h"
#include "qevent.h"
#include <QMainWindow>
#include <QStyleHints>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QWidget>
#include <QLabel>
#include <QList>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QKeyEvent>
#include <QUrl>
#include <QStackedLayout>
#include <vector>
#include <QGraphicsTextItem>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <string>
#include <fstream>

#ifdef _WIN32
  #include <windows.h>
#elif __APPLE__
  #include <mach-o/dyld.h>
#else
  #include <unistd.h>
#endif

// struct ChapterObject{
//   QString id;
//   QString title;
//   float startTime;
//   float endTime;
// };

struct SubObject{
  float starttime;
  std::string textContent;
  float endtime;
};

struct ChapterObject;
class CustomSlider;

// main window class
class MainWindow: public QMainWindow{
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
    CONTINUEFROMLASTPOS_BUTTON,
    BVolumeControl,
  };

  enum controlbuttonslayout_LABEL_ELEMENTS{
    Open_file,
    Open_folder,
    Open_media,
    Quit,
    JUMP_BACKWARD,
    JUMP_FORWARD,
    JUMP_TO_TIME,
    JUMP_TO_NEXT_CHAP,
    JUMP_TO_PREV_CHAP,
    TOGGLE_LOOPSEGMENT,
    FULL_VOLUME,
    MUTE,
    TOGGLE_ASPRadio,
    TOGGLE_SUB,
    LOADSUBTITLES,
    TOGGLE_SUBDISPLAY,
    ADDDELAY,
    REDUCEDELAY,
    SUBSETTINGS,
    TITLE,
    TOGGLE_CHAPTERSINDICATORS,
    THEME,
    SHORTCUTS,
  };

  MainWindow(QWidget *parent=nullptr);
  ~MainWindow();
  void setupShortCuts();
  void createTopLayout();
  void createBottomLayout();
  void topbarlayoutclick(int buttonindex);
  void controlbuttonslayoutclick(int buttonindex);
  void setsliderrange(qint64 position);
  void playertimeline(qint64 position);
  void updateTimer();
  void mediaposition(int position);
  void mediaplayer(QString url="blackscreen");
  void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
  void slidermanagement(qreal position);
  void closeVideo();
  void toggleSubtitles();
  void toggleChaptersIndicators();
  void moveToNextChapter();
  void moveToPrevChapter();
  void SubFileParsing(std::string subpath);
  void srtSubFileParsing(std::string subpath);
  void assSubFileParsing(std::string subpath);

  QString fixhtml(QString test);
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

// savingNewSettings

  //turnning off the tab focusing
  bool focusNextPrevChild(bool next) override{
    if(next){}
    return false;
  }

private:
  bool paused=false;
  bool fullscreened = false;
  bool finished = false;
  size_t videoindex=0;
  RepeatMode  rep = PlaylistRepeat ;
  Qt::AspectRatioMode CurrentAspectMode = Qt::KeepAspectRatio;

  QLabel* image;
  QWidget *mainwidget;
  QStackedLayout *stackedlayout;
  QVBoxLayout *mainlayout;
  QHBoxLayout *topbarlayout=nullptr;
  QGridLayout *videolayout;
  QVBoxLayout *controlbuttonslayout=nullptr;
  QMediaPlayer *player;
  QGraphicsTextItem *sublabel;
  QAudioOutput *audio;
  QGraphicsVideoItem *video;
  QGraphicsView *view;
  QGraphicsScene *scene;
  CustomSlider *videoslider;
  CustomSlider *volumeslider;
  QLabel *currenttimer;
  QLabel *totaltimer;
  qreal oldvolume;
  float subdelay=0;
  bool repeatfromposition = false;
  int startingpoint;
  int finishpoint;
  // floating control layout elements
  QWidget* floatingControlPannelWidget;
  QVBoxLayout * floatingControlPannelContainerLayout;
  QGraphicsProxyWidget * floatingControlPannelProxy;

  QList<QString> mcbuttons = {"BPause","BBack","BStop","BNext","BFullscreen","BPlaylist","BRepeating","BContinueFLP","BVolumeControl"};
  std::vector <QPushButton*> ButtonsObjectList = {};
  std::vector <QAction*> TopBarButtonsObjectList= {};
  QList<QString> topbarlayoutbuttons = {"Media","Playback","Audio","Video","Subtitle","View","Help"};

  QList<QList <QString> > actionslist = {{"Open File","Open Folder","Open Media","Quit"},
                                {"Jump Backward","Jump Forward","Jump to Time","Move To Next Chapter","Move To Previous Chapter","Start Segment Loop"},
                                {"Full Volume","Mute"},
                                {"Stretch to Fit"},
                                {"Add Subtitle File","Load Subtitle","Hide Subtitles","Add Delay","Reduce Delay","Subtitle Settings"},
                                {"Video Title","Display Chapters Indicators","Change Theme"},
                                {"Shortcuts Instructions"}};

  bool ShowSubs = true;
  bool showChaptersIndicators = false;
  QString htmlstyle;
  QGraphicsTextItem *toshowtext = nullptr;
  int subpadding;
  
  int subBottomMargin;
  int currentSubBottomSpace;

  SubObject* subobject;
  std::vector <SubObject*> subslist;
  std::string current_video_title="";
  QString currenturl = "";
  std::string currentworkdirectory;
  int lastsavedposition=0;
  std::vector <ChapterObject> ChaptersVectors = {};
  QString currentLoadedSubPath = "";

  //default Values
  std::map<std::string,float> Settings;

public:
  std::vector<QUrl> playlist;
  std::vector <QString> subsInVideo = {};
  QString playertype;
  bool MouseIsInsideFloatingPanel = false;
};

class PATHS {
  public:
    QString homedir = QString::fromStdString(std::getenv(
      #ifdef _WIN32
        "USERPROFILE"
      #else
        "HOME"
      #endif
    ));

    std::string Projectdir(){
      std::string projectpath;

      // if the code is running from a .appimage file return the project path from the env
      const char* appimage_root = std::getenv("VFW_ROOT");
      if (appimage_root != nullptr) {
          projectpath = appimage_root;
          return projectpath;
      }

      #ifdef _WIN32
        wchar_t path[1024];
        GetModuleFileName(NULL,path,sizeof(path)/sizeof(wchar_t));
        std::wstring ws(path);
        std::string str(ws.begin(),ws.end());
        projectpath = str;
      #elif __APPLE__
        char path[102];
        uint32_t size = sizeof(path);
        NSGetModuleFileName(path,&size);
        projectpath = path;
      #else
        char path[102];
        ssize_t len = readlink("/proc/self/exe",path,sizeof(path)-1);
        if(len != -1) path[len]='\0';
        projectpath = path;
      #endif

      size_t position = projectpath.rfind("bin");
      projectpath = projectpath.substr(0,position);
      return projectpath ;
    }


    std::string GETTHEME(std::string configDirectory){
      std::ifstream themefile(configDirectory+"/theme");
      std::string theme ="light";
      if(themefile){
        getline(themefile,theme);
      }
      return theme;
    }
};


extern PATHS path;
extern QString homedir;
extern std::string projectdir;
extern std::string theme;
extern std::string CONFIGSDIRECTORY;
extern std::string STYLESDIRECTORY;
extern std::string FONTSDIRECTORY;
extern QString ICONSDIRECTORY;
extern std::vector<std::string> supportedMediaFormats;
extern std::vector<std::string> supportedSubtitlesFormats;

#endif
