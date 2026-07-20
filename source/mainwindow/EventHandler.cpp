#include "../headers/main/mainwindow.h"
#include <QMouseEvent>
#include <QResizeEvent>

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  bool res = QMainWindow::eventFilter(obj, event);
  if(event->type() == QEvent::MouseMove) {
    if(!handleFloatingPannelDisplaying(obj, event)) res = false;
    if(!handleTimestampIndicator(event)) res = false;
    if(fullScreenEnabled) handleCursorHiding(event);
  }
  return res;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  resizeMainUiElement();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent * event) {
  if(event->button() == Qt::LeftButton)
    handleDoubleLeftClick(event);
}

void MainWindow::handleDoubleLeftClick(QMouseEvent *event) {
  int mousePosition_x = event->pos().rx();
  int mousePosition_y = event->pos().ry();
  int video_start_x = view->pos().rx();
  int video_start_y = view->pos().ry();
  int video_end_x = view->size().width()+video_start_x;
  int video_end_y = view->size().height()+video_start_y;

  bool ClickIsInsideVideoLayout = (
    mousePosition_x > video_start_x &&
    mousePosition_y > video_start_y &&
    mousePosition_x < video_end_x &&
    mousePosition_y < video_end_y
  );

  bool ClickNotOnfloatingPannel = !floatingControlPannel->isHovered(event->pos());
  if(ClickIsInsideVideoLayout && ClickNotOnfloatingPannel){
    toggleFullScreen();
  }
}
