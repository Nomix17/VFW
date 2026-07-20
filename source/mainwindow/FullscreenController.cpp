#include "../headers/main/mainwindow.h"
#include <QApplication>

void MainWindow::toggleFullScreen() {
  fullScreenEnabled ? exitFullScreen() : enterFullScreen();
}

void MainWindow::enterFullScreen() {
  if(fullScreenEnabled) return;
  QApplication::setOverrideCursor(Qt::BlankCursor);
  setTopbarLayoutVisible(false);
  int currentVolumeSliderPos = controlbuttonslayout->getVolumeValue();

  createBottomLayout();//recreating bottom layout
  floatingControlPannel->addButtonsLayout(controlbuttonslayout); 

  this->showFullScreen();
  setUniformMargins(mainlayout, 0);
  video->setSize(this->size());

  if (floatingControlPannel->isHidden())
    floatingControlPannel->show();

  setVolumeSliderPosition(currentVolumeSliderPos);
  fullScreenEnabled = true;
  updateButtonsIcon();
  updateTimeLabels();
}


void MainWindow::exitFullScreen() {
  if(!fullScreenEnabled) return;
  QApplication::restoreOverrideCursor();
  closeHideMouseTimer();
  setTopbarLayoutVisible(true);
  int currentVolumeSliderPos = controlbuttonslayout->getVolumeValue();

  createBottomLayout();//recreating bottom layout
  mainlayout->addLayout(controlbuttonslayout);

  this->showMaximized();
  setUniformMargins(mainlayout, MAINWINDOW_BORDERS_MARGIN);

  setVolumeSliderPosition(currentVolumeSliderPos);
  fullScreenEnabled = false;
  updateButtonsIcon();
  updateTimeLabels();
}

