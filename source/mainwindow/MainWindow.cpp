#include "../headers/main/mainwindow.h"

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
  setupVideoDropConnector();
  setupContextMenuConnector();

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
