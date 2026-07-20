#include "../headers/main/mainwindow.h"
#include <QSize>

void MainWindow::resizeMainUiElement() {
  resizeVideoContainers();
  repositionFloatingControllPannel();
  subtitlesItem->repositionText();
  if(overlayTextItem != nullptr) {
    overlayTextItem->repositionText();
  }
}

void MainWindow::resizeVideoContainers() {
  int VIEWWIDTH = view->size().width();
  int VIEWHEIGHT = view->size().height();
  video->setSize(QSize(VIEWWIDTH + 2, VIEWHEIGHT + 2));
  scene->setSceneRect(0, 0, VIEWWIDTH - 1, VIEWHEIGHT - 1);
  view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
}
