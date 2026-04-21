#ifndef TEXTTIMER
#define TEXTTIMER

#include "qlabel.h"
#include <iostream>
#include <string>
#include <sstream>

class TextTimer: public QLabel {
  public:

  TextTimer(QWidget *parent=nullptr):QLabel(parent) {
    setToDefaultState();
  }

  void setValue(int position) {
    this->setText(formatTime(position));
  }

  void setToDefaultState(){
    this->setText("--:--:--");
  }

  QString formatTime(int timeInMs){
    int hour = timeInMs / (1000 * 60 * 60);
    int min = (timeInMs / 1000 - hour * 60 * 60) / 60;
    int second = timeInMs / 1000 - min * 60 - hour * 60 * 60;
    std::ostringstream osshour, ossmin, osssecond;
    osshour << std::setfill('0') << std::setw(2) << hour;
    ossmin << std::setfill('0') << std::setw(2) << min;
    osssecond << std::setfill('0') << std::setw(2) << second;
    return QString::fromStdString(osshour.str() +":"+ ossmin.str() +":"+ osssecond.str());
  }
};


#endif
