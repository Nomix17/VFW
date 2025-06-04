#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QStyleHints>
#include "qboxlayout.h"
#include "qevent.h"
#include "CustomObjects.h"
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


struct SubObject{
  int starttime;
  std::string textcontaint;
  int endtime;
};

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
    LOOPSEGMENT,
    BREAKLOOP,
    FULL_VOLUME,
    MUTE,
    SET_RADIO,
    ADDSUB,
    STOPSUB,
    ADDDELAY,
    REDUCEDELAY,
    SUBSETTINGS,
    TITLE,
    THEME,
    SHORTCUTS,
  };

  MainWindow(QWidget *parent=nullptr);
  ~MainWindow();
  void createTopLayout();
  void createBottomLayout();
  void topbarlayoutclick(int buttonindex);
  void controlbuttonslayoutclick(int buttonindex);
  void setsliderrange(qint64 position);
  void playertimeline(qint64 position);
  void mediaposition(int position);
  void keyPressEvent(QKeyEvent *event)override;
  void mediaplayer(QString url="blackscreen");
  void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
  void slidermanagement(qreal position);
  void subfileparsing(std::string subpath);
  QString fixhtml(QString test);
  void changingposition(int newpos);
  void resizelements(std::string elementtorezise="all",int animationTime = 0);

  void resizeEvent(QResizeEvent * event) override;
  void mouseDoubleClickEvent(QMouseEvent * event)override;
  bool eventFilter(QObject *obj, QEvent *event) override;

  void FullScreen();
  void showingthings(std::string texttoshow, int xposition, int yposition,int animationduration);
  void topbarlayoutvisibility(std::string status);
  void savevideoposition();
  void getlastsavedposition();


  //turnning off the tab focusing
  bool focusNextPrevChild(bool next) override{
    if(next){}
    return false;
  }
private:
  bool paused = false;
  bool fullscreened = false;
  bool finished = false;
  QString playertype;
  size_t videoindex=0;
  RepeatMode  rep = PlaylistRepeat ;

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
  QList<QString> topbarlayoutbuttons = {"Media","Playback","Audio","Video","Subtitle","View","Help"};
  QList<QList <QString> > actionslist = {{"Open File","Open Folder","Open Media","Quit"},{"Jump Backward","Jump Forward","Jump to Time","Loop Segment","Break Loop"},{"Full Volume","Mute"},{"Set Radio"},{"Add Subtitles","Stop Subtitles","Add Delay","Reduce Delay","Subtitle Settings"},{"Video Title","change theme"},{"Shortcuts Instructions"}};

public:
  std::vector<QUrl> playlist;
  QString htmlstyle;
  int subpadding;
  int submarginbottom;
  SubObject* subobject;
  std::vector <SubObject*> subslist;
  std::string current_video_title="";
  QString currenturl = "";
  std::string currentworkdirectory;
  int lastsavedposition=0;

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
      auto defaultcolorscheme = qApp->styleHints()->colorScheme();
      std::string theme = (defaultcolorscheme == Qt::ColorScheme::Dark)? "dark":"light";
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
extern QString ICONSDIRECTORY;


#endif
