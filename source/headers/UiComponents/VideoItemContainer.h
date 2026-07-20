#ifndef VIDEOITEMCONTAINER
#define VIDEOITEMCONTAINER

#include <QGraphicsVideoItem>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

class VideoItemContainer : public QGraphicsVideoItem {
  Q_OBJECT
  public:
    VideoItemContainer() : QGraphicsVideoItem() {
      setAcceptDrops(true);
    }


  signals:
    void videoDropped(const QList<QUrl> &urls);
    void openContextMenu(QPointF pos);


  protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
      if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
      } else {
        event->ignore();
      }
    }

    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
      event->acceptProposedAction();
    }

    void dropEvent(QGraphicsSceneDragDropEvent *event) {
      const QMimeData* mimeData = event->mimeData();
      if (mimeData->hasUrls()) {
        emit videoDropped(mimeData->urls());
        event->acceptProposedAction();
        return;
      }
      event->ignore();
    }

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override {
      event->accept();
      emit openContextMenu(event->screenPos());
    }

};

#endif
