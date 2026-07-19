#include "../headers/main/mainwindow.h"

#include <QShortcut>

void MainWindow::setupShortcuts() {
  setFocusPolicy(Qt::StrongFocus);
  clearVector(qShortcutsObjs);

  // --- Playback controls ---
  QShortcut* pause = new QShortcut(currentShortcuts["TOGGLE_PAUSE"], this);
  pause->setContext(Qt::ApplicationShortcut);
  connect(pause, &QShortcut::activated, this, [this]{
      controlButtonsHandler(BottomControlPanel::PAUSE_BUTTON);
  });
  qShortcutsObjs.push_back(pause);

  QShortcut* seekFwd = new QShortcut(currentShortcuts["SEEK_FORWARD"], this);
  seekFwd->setContext(Qt::ApplicationShortcut);
  connect(seekFwd, &QShortcut::activated, this, [this]{
      changePlayBackPosition(player->position() + 5000);
  });
  qShortcutsObjs.push_back(seekFwd);

  QShortcut* seekBack = new QShortcut(currentShortcuts["SEEK_BACKWARD"], this);
  seekBack->setContext(Qt::ApplicationShortcut);
  connect(seekBack, &QShortcut::activated, this, [this]{
      changePlayBackPosition(std::max(0, static_cast<int>(player->position() - 5000)));
  });
  qShortcutsObjs.push_back(seekBack);

  // --- Volume controls ---
  QShortcut* volUp = new QShortcut(currentShortcuts["VOLUME_UP"], this);
  volUp->setContext(Qt::ApplicationShortcut);
  connect(volUp, &QShortcut::activated, this, [this]{
      int volumeSliderValue = controlbuttonslayout->getVolumeValue();
      setVolumeSliderPosition(volumeSliderValue + 100);
  });
  qShortcutsObjs.push_back(volUp);

  QShortcut* volDown = new QShortcut(currentShortcuts["VOLUME_DOWN"], this);
  volDown->setContext(Qt::ApplicationShortcut);
  connect(volDown, &QShortcut::activated, this, [this]{
      int volumeSliderValue = controlbuttonslayout->getVolumeValue();
      setVolumeSliderPosition(volumeSliderValue - 100);
  });
  qShortcutsObjs.push_back(volDown);

  // --- Fullscreen ---
  QShortcut* toggleFS = new QShortcut(currentShortcuts["TOGGLE_FULLSCREEN"], this);
  toggleFS->setContext(Qt::ApplicationShortcut);
  connect(toggleFS, &QShortcut::activated, this, &MainWindow::toggleFullScreen);
  qShortcutsObjs.push_back(toggleFS);

  QShortcut* escFS = new QShortcut(currentShortcuts["EXIT_FULLSCREEN"], this);
  escFS->setContext(Qt::ApplicationShortcut);
  connect(escFS, &QShortcut::activated, this, &MainWindow::exitFullScreen);
  qShortcutsObjs.push_back(escFS);

  // --- Subtitles controls ---
  QShortcut* toggleSubs = new QShortcut(currentShortcuts["TOGGLE_SUBTITLES"], this);
  toggleSubs->setContext(Qt::ApplicationShortcut);
  connect(toggleSubs, &QShortcut::activated, this, &MainWindow::toggleSubtitles);
  qShortcutsObjs.push_back(toggleSubs);

  // --- Chapters controls ---
  QShortcut* toggleChaptersIndHandler = new QShortcut(currentShortcuts["TOGGLE_CHAPTER_INDICATORS"], this);
  toggleChaptersIndHandler->setContext(Qt::ApplicationShortcut);
  connect(toggleChaptersIndHandler, &QShortcut::activated, this, &MainWindow::toggleChaptersIndicators);
  qShortcutsObjs.push_back(toggleChaptersIndHandler);

  QShortcut* goToNextChapter = new QShortcut(currentShortcuts["NEXT_CHAPTER"], this);
  goToNextChapter->setContext(Qt::ApplicationShortcut);
  connect(goToNextChapter, &QShortcut::activated, this, &MainWindow::moveToNextChapter);
  qShortcutsObjs.push_back(goToNextChapter);

  QShortcut* goToPrevChapter = new QShortcut(currentShortcuts["PREVIOUS_CHAPTER"], this);
  goToPrevChapter->setContext(Qt::ApplicationShortcut);
  connect(goToPrevChapter, &QShortcut::activated, this, &MainWindow::moveToPrevChapter);
  qShortcutsObjs.push_back(goToPrevChapter);

  // --- Other controls ---
  QShortcut* volPanel = new QShortcut(currentShortcuts["TOGGLE_MUTE"], this);
  volPanel->setContext(Qt::ApplicationShortcut);
  connect(volPanel, &QShortcut::activated, this, [this]{
      controlButtonsHandler(BottomControlPanel::TOGGLE_VOLUME_BUTTON);
  });
  qShortcutsObjs.push_back(volPanel);

  QShortcut* displayTitle = new QShortcut(currentShortcuts["DISPLAY_TITLE"], this);
  displayTitle->setContext(Qt::ApplicationShortcut);
  connect(displayTitle, &QShortcut::activated, this, [this]{
      onToolMenuAction(ToolMenu::TITLE);
  });
  qShortcutsObjs.push_back(displayTitle);

  QShortcut* reduceDelay = new QShortcut(currentShortcuts["REDUCE_SUBTITLES_DELAY"], this);
  reduceDelay->setContext(Qt::ApplicationShortcut);
  connect(reduceDelay, &QShortcut::activated, this, [this]{
      onToolMenuAction(ToolMenu::REDUCEDELAY);
  });
  qShortcutsObjs.push_back(reduceDelay);

  QShortcut* addDelay = new QShortcut(currentShortcuts["INCREASE_SUBTITLES_DELAY"], this);
  addDelay->setContext(Qt::ApplicationShortcut);
  connect(addDelay, &QShortcut::activated, this, [this]{
      onToolMenuAction(ToolMenu::ADDDELAY);
  });
  qShortcutsObjs.push_back(addDelay);
}
