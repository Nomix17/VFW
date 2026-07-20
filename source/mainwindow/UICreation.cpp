#include "../headers/main/mainwindow.h"
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <iostream>

void MainWindow::setupWindow() {
  this->setWindowTitle("VFW");
  this->resize(750, 550);
  setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::initMediaElements() {
  player = new QMediaPlayer(this);
  audio = new QAudioOutput(this);
  video = new VideoItemContainer();
  scene = new QGraphicsScene(this);
  view = new QGraphicsView(scene, this);
  mainwidget = new QWidget(this);
  mainlayout = new QVBoxLayout();
  videolayout = new QGridLayout();
}

void MainWindow::setupMainLayout() {
  setUniformMargins(mainlayout, MAINWINDOW_BORDERS_MARGIN);
  videolayout->addWidget(view);
  mainlayout->addLayout(topbarlayout);
  mainlayout->addLayout(videolayout);
  mainlayout->addLayout(controlbuttonslayout);
  mainwidget->setLayout(mainlayout);
  setCentralWidget(mainwidget);
}

void MainWindow::createTopLayout() {
  if(topbarlayout != nullptr) delete topbarlayout; // gets triggered when toggling fullscreen
  topbarlayout = new TopBar();
  toolMenuActionsObjectsList = topbarlayout->getActionsObjects();
  connect(topbarlayout, &TopBar::handleButtonsClick,[this](int actionNumber) {
     onToolMenuAction(actionNumber);
  });
}

void MainWindow::setupSceneAndView() {
  video->setSize(view->size());
  view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scene->addItem(video);
  scene->addItem(subtitlesItem);
}

void MainWindow::setupVideoDropConnector() {
  connect(video, &VideoItemContainer::videoDropped, video, [this](const QList<QUrl> &urls) {
    bool videoAdded = false;
    int firstNewVideoIndex = playlist.size();
    for(QUrl url : urls) {
      std::filesystem::path filePath(url.toLocalFile().toStdString());
      if(!isVideoSupported(filePath)) continue;
      playlist.push_back(url);
      videoAdded = true;
      std::cout<<"Loading Video: "<<url.toString().toStdString()<<"\n";
    }
    if(videoAdded) {
      currentPlayerMode = PlayerMode::Playlist;
      playVideoInPlaylist(firstNewVideoIndex);
    }
  });
}

void MainWindow::setupContextMenuConnector() {
  connect(video, &VideoItemContainer::openContextMenu, this, [this](QPointF pos) {
    ContextMenu* menu = new ContextMenu(this);
    connect(menu, &ContextMenu::handleButtonsClick, this, &MainWindow::onToolMenuAction);
    connect(menu, &QMenu::aboutToHide, [this]() { contextMenuOpened = false; });
    contextMenuOpened = true;
    menu->exec(pos.toPoint());
    delete menu;
  });
}

void MainWindow::createBottomLayout() {
  if (controlbuttonslayout != nullptr) {
    delete controlbuttonslayout;
  }
  controlbuttonslayout=nullptr;
  controlbuttonslayout = new BottomControlPanel(SYSTEMPATHS->currentIconsDir);//creating new layout
  connect(controlbuttonslayout, &BottomControlPanel::controlButtonsHandler, this, &MainWindow::controlButtonsHandler);
  connect(controlbuttonslayout, &BottomControlPanel::videoSliderMoved, this, &MainWindow::changePlayBackPosition);
  connect(controlbuttonslayout, &BottomControlPanel::volumeSliderMoved,[this](int value) {
    audio->setVolume((float)value / 1000);
  });
  connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::playbackPositionUpdated);
  connect(player, &QMediaPlayer::durationChanged, [this](qint64 duration){
    controlbuttonslayout->setVideoSliderRange(0,duration);
  });
  setVideoSliderRange(player->duration());
  controlbuttonslayout->setVideoSliderValue(player->position());
  controlbuttonslayout->setChaptersMarks(ChaptersVectors, showChaptersIndicators);
  updateButtonsIcon();
}

void MainWindow::connectPlayerSignals() {
  connect(player, &QMediaPlayer::metaDataChanged, this, [this]() {
    std::cout << "\n";
    getSubtitleTracksFromMetaData();
    getAudioTracksFromMetaData();
    std::cout << "\n";
  });
}

void MainWindow::createTimestampIndicator() {
  timestampIndicator = new TextTimer(this);
  timestampIndicator->hide();
  timestampIndicator->setAlignment(Qt::AlignCenter);
  timestampIndicator->setObjectName("TimestampIndicator");
}
