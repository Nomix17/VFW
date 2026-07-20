#ifndef SUBTITLESITEM_H
#define SUBTITLESITEM_H

#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QFont>
#include "TextItem.h"

class SubtitlesItem: public TextItem {
  public:
    SubtitlesItem(QGraphicsView* parentView) 
    : TextItem(TextPosition::BOTTOM, parentView) {
      QFont font;
      this->setFont(font);
      this->setObjectName("sublabel");
      this->setZValue(12);
    }
};

#endif
