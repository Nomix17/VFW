#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qevent.h"
#include "CustomObjects.h"
#include <QMainWindow>
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



// main window class
class MainWindow: public QMainWindow{
    Q_OBJECT
public:
    enum RepeatMode{
        PlaylistRepeat=0,
        VideoRepeat=1,
        Shuffle=2,
    };
    enum FIRSTLAYOUT_LABEL_ELEMENTS{
        PAUSE_BUTTON = 0,
        BACK_BUTTON = 1,
        STOP_BUTTON = 2,
        NEXT_BUTTON = 3,
        FULLSCREEN_BUTTON = 4,
        PLAYLIST_BUTTON = 5,
        REPETITION_BUTTON = 6,
        BVolumeControl = 7,
    };
    enum FOURTHLAYOUT_LABEL_ELEMENTS{
        Open_file = 0,
        Open_folder = 1,
        Open_media = 2,
        Quit = 3,
        Title = 4,
        JUMP_BACKWARD = 5,
        JUMP_FORWARD = 6,
        JUMP_TO_TIME = 7,
        LOOPSEGMENT = 8,
        BREAKLOOP = 9,
        FULL_VOLUME = 10,
        MUTE = 11,
        SET_RADIO = 12,
        ADDSUB = 13,
        STOPSUB = 14,
        ADDDELAY = 15,
        REDUCEDELAY = 16,
        SUBSETTINGS = 17,
    };
    MainWindow(QWidget *parent=nullptr);
    void firstlayoutclick(int buttonindex);
    void fourthlayoutclick(int buttonindex);
    void setsliderrange(qint64 position);
    void setsliderposition(qint64 position);
    void mediaposition(int position);
    void keyPressEvent(QKeyEvent *event)override;
    void mediaplayer(QString url="blackscreen");
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void slidertovolume(int position);
    void volumetoslider(qreal position);
    void subscraper(std::string subpath);
    QString fixhtml(QString test);
    void changefarposition(int newpos);
    void resizeEvent(QResizeEvent * event) override;
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
    QHBoxLayout *firstlayout;
    QGridLayout *videolayout;
    QHBoxLayout *thirdlayout;
    QHBoxLayout *fourthlayout;
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
    std::chrono::time_point<std::chrono::system_clock> now;
    float subdelay=0;
    bool repeatfromposition = false;
    int startingpoint;
    int finishpoint;

    QList<QString> mcbuttons = {"BPause","BBack","BStop","BNext","BFullscreen","BPlaylist","BRepeating","BVolumeControl"};
    QList<QString> firstlayoutbuttons = {"Media","Playback","Audio","Video","Subtitle","Tools","View","Help"};
    QList<QList <QString> > actionslist = {{"Open File","Open Folder","Open Media","Quit"},{"Title","Jump Backward","Jump Forward","Jump to Time","Loop Segment","Break Loop"},{"Full Volume","Mute"},{"Set Radio"},{"Add Subtitles","Stop Subtitles","Add Delay","Reduce Delay","Subtitle Settings",}};

public:
    std::vector<QUrl> playlist;
    QString htmlstyle;
    int subpadding;
    int submarginbottom;
    std::vector <float> subtimer;
    std::vector <std::string> sublines;
};






#endif
//https://vo-live.cdb.cdn.orange.com/Content/Channel/NationalGeographicHDChannel/HLS/index.m3u8
