#include <QGraphicsTextItem>

enum TextPosition {
  TOP_LEFT,
  BOTTOM,
  CENTER
};

class TextItem: public QGraphicsTextItem {
  inline static QString htmlTemplate = "";
  inline static int bottomMargin = 0;
  inline static int subOffset = 0;

  public:
    int getBottomMargin() { return bottomMargin; }
    int getSubOffset() { return subOffset; }
    QString getSubtitlesStyle() { return htmlTemplate; }

    static void setHtmlTemplate(QString value) {
      htmlTemplate = value;
    }
    static void setBottomMargin(int value) {
      bottomMargin = value;
    }
    static void setSubOffset(int value) {
      subOffset = value;
    }

    void clearContent() {
      this->setHtml("");
    }
    void setContent(QString text) {
      this->setHtml(TextItem::htmlTemplate + text + "</td></tr></table>");
    }

    void repositionText(QSize viewSize, TextPosition pos) {
      auto rect = this->boundingRect();
      int textWidth = rect.width();
      int textHeight = rect.height();
      int VIEWWIDTH = viewSize.width();
      int VIEWHEIGHT = viewSize.height();

      if(pos == TextPosition::TOP_LEFT) {
        int paddingFromLeftBorder = VIEWWIDTH * 0.01;
        int paddingFromTopBorder = VIEWHEIGHT * 0.01;
        setPos(paddingFromLeftBorder, paddingFromTopBorder);

      } else if(pos == TextPosition::CENTER) {
        setPos(
          (VIEWWIDTH - textWidth) / 2,
          (VIEWHEIGHT - textHeight) / 2
        );

      } else if(pos == TextPosition::BOTTOM) {
        setPos(
          (VIEWWIDTH - textWidth) / 2,
          (VIEWHEIGHT - textHeight / 2) - bottomMargin - subOffset
        );
      }
    }
};
