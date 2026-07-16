#ifndef CUSTOMSLIDER 
#define CUSTOMSLIDER 

#include "qpainter.h"
#include <iostream>
#include <QSlider>
#include <QMouseEvent>
#include <QPainter>

struct ChapterObject{
  QString id;
  QString title;
  float startTime;
  float endTime;
};

class CustomSlider:public QSlider {
  Q_OBJECT
public:
  //constructor to redirect the object defined by this class to QSlider
  explicit CustomSlider(Qt::Orientation orientation,QWidget *parent=nullptr):QSlider(orientation,parent){}


  //function to change the sliderposition based on the mouse position and the slider size
  void movetoposition(int mousePosX){
      double target_position = sliderValueFromXPos(mousePosX);
      this->setSliderPosition(target_position);
      emit sliderMoved(mousePosX);
  }

  void mousePressEvent(QMouseEvent * event){
    buttonpressed=true;
    int mousexposition = event->pos().rx();
    if(buttonpressed)
      movetoposition(mousexposition);
  }

  void mouseMoveEvent(QMouseEvent *event){
    //moving the slider position if the mouse moved while pressing on the slider (holding it)
    int mousexposition = event->pos().rx();
    if(buttonpressed)
      movetoposition(mousexposition);
  }

  void mouseReleaseEvent(QMouseEvent *event){
    //if the cursor is not holding the slider we set the boolean variable to false
    buttonpressed = false;
    QSlider::mouseReleaseEvent(event);
  }
 
  int sliderValueFromXPos(int posX) {
    int slider_range = this->maximum();
    int slider_size = size().width();
    return static_cast<double>(posX)*slider_range/slider_size;
  }

  bool isHovered(QPoint mousePos) {
    bool mouseInsideX =
      (mousePos.rx() >= getSliderGlobalPos().rx()) &&
      (mousePos.rx() <= getRightBoundry());
    bool mouseInsideY =
      (mousePos.ry() >= getSliderGlobalPos().ry()) &&
      (mousePos.ry() <= getBottomBoundry());
    return (mouseInsideX && mouseInsideY);
  }
  QPoint getSliderGlobalPos() { return this->mapToGlobal(QPoint{0,0}); }
  int getBottomBoundry() { return getSliderGlobalPos().ry() + this->rect().height(); }
  int getRightBoundry() { return getSliderGlobalPos().rx() + this->rect().width(); }

  void setChaptersMarks(std::vector <ChapterObject> Chapters, bool displayChapters){
    this->Chapters = Chapters;
    this->displayChapters = displayChapters;
    this->update();
  }

private:
  void drawChapterIndicators() {
    if (Chapters.empty() || !displayChapters) return;

    QPainter painter(this);
    QColor lineColor = palette().color(QPalette::Window);
    QPen pen(lineColor);
    pen.setWidth(3);
    painter.setPen(pen);

    int slider_width = width();
    int slider_height = height();
    int slider_range = maximum() - minimum();

    for (size_t i = 0; i < Chapters.size(); i++) {
      float start = Chapters[i].startTime*1000;
      int position = static_cast<int>((start * slider_width) / slider_range);
      if (position <= 0) continue;
      if (position >= slider_width) continue;
      painter.drawLine(position, slider_height, position, height()/4);
    }
  }



protected:
  void paintEvent(QPaintEvent* event) override{
    QSlider::paintEvent(event);
    drawChapterIndicators();
  }


private:
  bool buttonpressed = true;//bool to save the state of cursor holding/pressing or not
  std::vector <ChapterObject> Chapters;
  bool displayChapters;
};



#endif
