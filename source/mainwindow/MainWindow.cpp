#include "../headers/main/mainwindow.h"

#include <QApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QVideoWidget>

#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QMenu>
#include <QAction>
#include <QShortcut>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QGridLayout>

#include <QFileDialog>
#include <QFileInfo>
#include <QTimer>
#include <QKeyEvent>
#include <QStyleHints>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsVideoItem>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>

#include <fstream>
#include <sstream>
#include <filesystem>
#include <limits.h>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupWindow();

  currentShortcuts = ShortcutsInst::getShortcutMap(SYSTEMPATHS->configPath);
  setupShortcuts();

  initMediaElements();
  createTopLayout();
  createBottomLayout();
  createTimestampIndicator();

  floatingControlPannel = new FloatingControlPannel(scene);
  subtitlesItem = new SubtitlesItem(view);

  connectPlayerSignals();
  setupSceneAndView();
  setupMainLayout();

  parseSettingsFile();
  setPlayerDefaultState();
  loadSubtitleStyle();
}

//distractor
MainWindow::~MainWindow(){
  savingNewSettings();
  savevideoposition();
  for (SubObject* ptr:currentLoadedSubList){
    delete ptr;
  }
}
