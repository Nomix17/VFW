#ifndef CUSTOMPUSHBUTTON 
#define CUSTOMPUSHBUTTON 

#include <QMouseEvent>
#include <QPushButton>
#include <csignal>

class CustomPushButton : public QPushButton {
  Q_OBJECT

signals:
  void leftClick();
  void rightClick();

public:
  CustomPushButton(QWidget *parent=nullptr) : QPushButton(parent){}

  void mousePressEvent(QMouseEvent * event) override{
    if(event->button() == Qt::RightButton) {
      emit rightClick();
    } else if(event->button() == Qt::LeftButton) {
      emit leftClick();
    } else {
      QPushButton::mousePressEvent(event);
    }
  }
};



#endif
