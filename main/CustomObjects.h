#ifndef CUSTOMOBJ
#define CUSTOMOBJ

#include <iostream>
#include <QSlider>
#include <QMouseEvent>

class CustomSlider:public QSlider{
  Q_OBJECT
public:
  //constructor to redirect the object defined by this class to QSlider
  explicit CustomSlider(Qt::Orientation orientation,QWidget *parent=nullptr):QSlider(orientation,parent){}


  //function to change the sliderposition based on the mouse position and the slider size
  void movetoposition(int mouse_position){
      //move only if the cursor is pressing on the slider
      if(buttonpressed){
        int slider_range = this->maximum();//the media to slider range 
        int slider_size = size().width();//the size of the slider
        double target_position = static_cast<double>(mouse_position)*slider_range/slider_size;//calculating where should the slider move relativly to it's size and range
        this->setSliderPosition(target_position);
        emit sliderMoved(mouse_position);
      }
  }
  
  void mousePressEvent(QMouseEvent * event){
      buttonpressed=true;
      int mousexposition = event->pos().rx();
      movetoposition(mousexposition);//moving to the position where the cusor press
  }
  
  void mouseMoveEvent(QMouseEvent *event){
    //moving the slider position if the mouse moved while pressing on the slider (holding it)
    int mousexposition = event->pos().rx();
    movetoposition(mousexposition);
  }
  
  void mouseReleaseEvent(QMouseEvent *event){
    //if the cursor is not holding the slider we set the boolean variable to false
    buttonpressed = false;
    QSlider::mouseReleaseEvent(event);
  }

private:
  bool buttonpressed = true;//bool to save the state of cursor holding/pressing or not
};



#endif
