#include "../headers/main/mainwindow.h"

#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

void MainWindow::updateButtonsIcon(){
  updatePlayPauseButtonIcon();
  updateVolumeButtonIcon();
  updateRepeatButtonIcon();
  updateFullscreenIcon();
}

void MainWindow::updatePlayPauseButtonIcon() {
  bool Paused = (!videoIsPaused || currentVideoUrl == "");
  controlbuttonslayout->updatePausePlayButtonIcon(Paused);
}

void MainWindow::updateVolumeButtonIcon() {
  controlbuttonslayout->updateVolumeButtonIcon();
}

void MainWindow::updateRepeatButtonIcon() {
  controlbuttonslayout->updateRepetitionButtonIcon(rep);
}

void MainWindow::updateFullscreenIcon() {
  controlbuttonslayout->updateFullscreenIcon(fullScreenEnabled);
}

void MainWindow::updateTimeLabels() {
  if(currentVideoUrl == "") return;
  controlbuttonslayout->updatePlayBackTimer(player->position());
  controlbuttonslayout->updateDurationTimer(player->duration());
}

void MainWindow::setTopbarLayoutVisible(bool visible) {
  for (int i = 0; i < topbarlayout->count(); i++) {
    QWidget *searchtoolbutton = topbarlayout->itemAt(i)->widget();
    if (searchtoolbutton) {
      searchtoolbutton->setVisible(visible);
    }
  }
}

void MainWindow::setUniformMargins(QLayout *layout, int margin) {
  layout->setContentsMargins(margin, margin, margin, margin);
}

bool MainWindow::handleFloatingPannelDisplaying(QObject *obj, QEvent *event) {
  if(!static_cast<QWidget* >(obj)->isWidgetType())
    MouseIsInsideFloatingPanel = mouseInsideFloatingPanel(event);

  static bool floatingPannelDisplayed = false;
  if (
    floatingPannelDisplayed ||
    contextMenuOpened ||
    !fullScreenEnabled
  ) return false;

  int viewheight = view->size().height();

  // calculate the targetPosition that we want to move the pannel into
  QPointF targetPos = QPointF(
    floatingControlPannel->getXPos(),
    viewheight - floatingControlPannel->getHeight()
  );
  moveSomethingToPos(floatingControlPannel->getPannelProxyObj(),targetPos,200);

  // keeping the same subtitles margin
  subtitlesItem->setSubOffset(floatingControlPannel->getHeight());
  subtitlesItem->repositionText();
  floatingPannelDisplayed = true;

  QTimer::singleShot(TIME_BEFORE_HIDING_FLOATING_PANNEL,[this]() {
    if(!MouseIsInsideFloatingPanel && floatingPannelDisplayed){
      repositionFloatingControllPannel(200);
      subtitlesItem->setSubOffset(0);
      subtitlesItem->repositionText();
    }
    floatingPannelDisplayed = false;
  });

  return true;
}

void MainWindow::repositionFloatingControllPannel(int animationTime) {
  int VIEWWIDTH = view->size().width();
  int VIEWHEIGHT = view->size().height();
  QPoint hiding_position = QPoint(
    (VIEWWIDTH - floatingControlPannel->getWidth()) / 2,
    (VIEWHEIGHT + floatingControlPannel->getHeight())
  );
  moveSomethingToPos(floatingControlPannel->getPannelProxyObj(),  hiding_position, animationTime);
}

bool MainWindow::mouseInsideFloatingPanel(QEvent* event) {
  QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
  QPoint mouseGlobalPos = mouseEvent->globalPosition().toPoint();
  return (floatingControlPannel->isHovered(mouseGlobalPos));
}

void MainWindow::moveSomethingToPos(QGraphicsWidget *widget, QPointF targetPos, int animationTime) {
  QPropertyAnimation *animation = new QPropertyAnimation(widget, "pos");//defining the animation
  animation->setDuration(animationTime);//duration of the animation
  animation->setStartValue(widget->pos());
  animation->setEndValue(targetPos);
  animation->start(QPropertyAnimation::DeleteWhenStopped);  // start the animation
}

void MainWindow::handleCursorHiding(QEvent* event) {
  QApplication::restoreOverrideCursor();
  if(MouseIsInsideFloatingPanel || contextMenuOpened) {
    closeHideMouseTimer();
    return;
  }
  if (hideMouseTimer == nullptr) {
    hideMouseTimer = new QTimer(this);
    hideMouseTimer->setSingleShot(true);
    connect(hideMouseTimer, &QTimer::timeout, this, [this]() {
      QApplication::setOverrideCursor(Qt::BlankCursor);
    });
  }
  hideMouseTimer->start(TIME_BEFORE_HIDING_CURSOR);
}

void MainWindow::closeHideMouseTimer() {
  if(hideMouseTimer != nullptr) {
    hideMouseTimer->stop();
    hideMouseTimer->disconnect();
    hideMouseTimer->deleteLater();
    hideMouseTimer = nullptr;
  }
}

bool MainWindow::handleTimestampIndicator(QEvent* event) {
  QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
  QPoint mouseGlobalPos = mouseEvent->globalPosition().toPoint();

  if(controlbuttonslayout->isVideoSliderHovered(mouseGlobalPos) && currentVideoUrl != "") {
    double bottom_margin = 7;
    double width = timestampIndicator->rect().width();
    QPoint windowRelativePos = this->mapFromGlobal(QPoint(
      mouseGlobalPos.rx(),
      controlbuttonslayout->getVideoSliderGlobalPos().ry()
        - timestampIndicator->rect().height()
        - (fullScreenEnabled ? floatingControlPannel->getBordersMargin() : 6) // 6px is a magic number to align the indicator
        - bottom_margin
    ));

    timestampIndicator->move(windowRelativePos.rx() - width / 2, windowRelativePos.ry());

    int timestamp = controlbuttonslayout->sliderValueFromXPos(mouseEvent->pos().rx());
    int chapterIndex = findChapterIndexByTime(timestamp);
    if(ChaptersVectors.size() != 0 && showChaptersIndicators && chapterIndex >= 0) {
      QString chapterName = ChaptersVectors[chapterIndex].title;
      timestampIndicator->setString(QString("%1 %2").arg(chapterName).arg(TextTimer::formatTime(timestamp)));
    } else {
      timestampIndicator->setValue(timestamp);
    }

    timestampIndicator->adjustSize();
    timestampIndicator->show();
    return true;

  } else {
    timestampIndicator->setToDefaultState();
    timestampIndicator->hide();
    return false;
  }
}

void MainWindow::renderOverlayText (
  std::string textToRender,
  TextPosition position,
  int animationduration
) {
  if(
    overlayTextItem != nullptr &&
    overlayTextItem->scene() == scene 
  ) {
    scene->removeItem(overlayTextItem);
    delete overlayTextItem;
  }
  if(animationduration == 0) return;

  overlayTextItem = new TextItem(position, view);
  overlayTextItem->setContent(QString::fromStdString(textToRender));
  overlayTextItem->repositionText();
  scene->addItem(overlayTextItem);

  // making an effect for the apearence and deapearence of the text
  QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(overlayTextItem);
  overlayTextItem->setGraphicsEffect(opacityEffect);

  // configuring the animation of the text
  QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity");
  animation->setDuration(animationduration);
  animation->setStartValue(1.0);
  animation->setEndValue(0.0);
  animation->start(QPropertyAnimation::DeleteWhenStopped);  // start the animation
}
