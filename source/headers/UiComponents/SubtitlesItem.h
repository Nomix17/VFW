#include <QGraphicsTextItem>
#include <QFont>
#include "../main/TextItem.h"

class SubtitlesItem: public TextItem {
  public:
    SubtitlesItem() {
      QFont font;
      this->setFont(font);
      this->setObjectName("sublabel");
      this->setZValue(12);
    }
    void repositionText(QSize viewSize) {
      TextItem::repositionText(viewSize, TextPosition::BOTTOM);
    }
};
