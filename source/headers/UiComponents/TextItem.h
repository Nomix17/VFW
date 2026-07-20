#ifndef TEXTITEM_H
#define TEXTITEM_H

#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QString>

enum TextPosition {
  TOP_LEFT,
  BOTTOM,
  CENTER
};

class TextItem: public QGraphicsTextItem {
  QGraphicsView* viewParent;
  TextPosition textPosition;
  inline static QString htmlTemplate = "";
  inline static int bottomMargin = 0;
  inline static int subOffset = 0;

  public:
    TextItem(TextPosition position,QGraphicsView* viewParent) {
      this->textPosition = position;
      this->viewParent = viewParent;
    }

    int getBottomMargin() { return bottomMargin; }
    int getSubOffset() { return subOffset; }
    QString getSubtitlesStyle() { return htmlTemplate; }

    static void setHtmlTemplate(QString value) { htmlTemplate = value; }
    static void setBottomMargin(int value) { bottomMargin = value; }
    static void setSubOffset(int value) { subOffset = value; }
    void setTextPosition(TextPosition value) { this->textPosition = value; }

    void clearContent() {
      this->setHtml("");
    }
    void setContent(QString text) {
      this->setHtml(TextItem::htmlTemplate + text + "</td></tr></table>");
    }

    void repositionText() {
      auto rect = this->boundingRect();
      int textWidth = rect.width();
      int textHeight = rect.height();
      int VIEWWIDTH = viewParent->size().width();
      int VIEWHEIGHT = viewParent->size().height();

      if(textPosition == TextPosition::TOP_LEFT) {
        int paddingFromLeftBorder = VIEWWIDTH * 0.01;
        int paddingFromTopBorder = VIEWHEIGHT * 0.01;
        setPos(paddingFromLeftBorder, paddingFromTopBorder);

      } else if(textPosition == TextPosition::CENTER) {
        setPos(
          (VIEWWIDTH - textWidth) / 2,
          (VIEWHEIGHT - textHeight) / 2
        );

      } else if(textPosition == TextPosition::BOTTOM) {
        setPos(
          (VIEWWIDTH - textWidth) / 2,
          (VIEWHEIGHT - textHeight / 2) - bottomMargin - subOffset
        );
      }
    }
};

#endif
