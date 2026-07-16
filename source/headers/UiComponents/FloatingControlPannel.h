#include "qgraphicsproxywidget.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

class FloatingControlPannel: public QWidget {
  QVBoxLayout * containerLayout;
  QGraphicsProxyWidget * pannelProxy;
  int borders_margin = 10;
  public:
    FloatingControlPannel(QGraphicsScene *scene) {
      containerLayout = new QVBoxLayout(this);
      containerLayout->setContentsMargins(borders_margin, borders_margin, borders_margin, borders_margin);
      pannelProxy = scene->addWidget(this);
      this->hide();
      pannelProxy->setZValue(10);
      this->update();
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

    int getWidth() { return pannelProxy->boundingRect().width(); }
    int getHeight() { return pannelProxy->boundingRect().height(); }
    int getXPos() { return pannelProxy->pos().rx(); }
    int getYPos() { return pannelProxy->pos().ry(); }

    void addButtonsLayout(QLayout* buttonsLayout) {
      containerLayout->addLayout(buttonsLayout);
    }

    int getBordersMargin() {
      return borders_margin;
    }

    QGraphicsProxyWidget* getPannelProxyObj() { return pannelProxy; }
};
