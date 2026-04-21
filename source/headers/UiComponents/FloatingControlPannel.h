#include "qgraphicsproxywidget.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

class FloatingControlPannel: public QWidget {
  QVBoxLayout * containerLayout;
  QGraphicsProxyWidget * pannelProxy;

  public:
    FloatingControlPannel(QGraphicsScene *scene) {
      containerLayout = new QVBoxLayout(this);
      pannelProxy = scene->addWidget(this);
      this->hide();
      pannelProxy->setZValue(10);
      this->update();
    }

    bool isHovered(QPoint mousePos) {
      bool mouseInsideX =
        (mousePos.rx() >= getXPos()) &&
        (mousePos.rx() <= getRightBoundry());
      bool mouseInsideY =
        (mousePos.ry() >= getYPos()) &&
        (mousePos.ry() <= getBottomBoundry());

      return (mouseInsideX && mouseInsideY);
    }

    int getWidth() { return pannelProxy->boundingRect().width(); }
    int getHeight() { return pannelProxy->boundingRect().height(); }
    int getXPos() { return pannelProxy->pos().rx(); }
    int getYPos() { return pannelProxy->pos().ry(); }

    int getBottomBoundry() {
      return getYPos() + getHeight();
    }
    int getRightBoundry() {
      return getXPos() + getWidth();
    }

    void addButtonsLayout(QLayout* buttonsLayout) {
      containerLayout->addLayout(buttonsLayout);
    }

    QGraphicsProxyWidget* getPannelProxyObj() { return pannelProxy; }
};
