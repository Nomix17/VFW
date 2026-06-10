#ifndef TOPBARBUTTON
#define TOPBARBUTTON

#include <iostream>
#include <QToolButton>
#include <QMenu>
#include <QWidget>
#include <QHBoxLayout>


class MenuButton : public QWidget {
  Q_OBJECT
  signals:
    void buttonClicked(int actionNumber);

  private:
    QToolButton* Button;
    QString buttonName;
    std::vector <QAction*> buttonActionsList;

  public:
    MenuButton(QString buttonName, QWidget* parent = nullptr) 
        : QWidget(parent) {
      Button = new QToolButton(this);
      this->buttonName = buttonName;
      Button->setPopupMode(QToolButton::InstantPopup);
      Button->setText(buttonName);
      Button->setObjectName(buttonName);
      Button->setFocusPolicy(Qt::NoFocus);

      // Add layout so Button fills the widget
      QHBoxLayout* layout = new QHBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->addWidget(Button);
      this->setLayout(layout);
    }

    void buildMenuActions(QList<QString> actionslist, int actionIndexOffset) {
      QMenu *menu = new QMenu(this);
      for (qsizetype i = 0; i < actionslist.size(); i++) {
        QAction *action = new QAction(this);
        action->setObjectName(actionslist[i]);
        action->setText(actionslist[i]);
        int actionNumber = buttonActionsList.size() + actionIndexOffset;
        connect(action, &QAction::triggered, [this,actionNumber]() {
          emit buttonClicked(actionNumber); 
        });
        buttonActionsList.push_back(action);
        menu->addAction(action);
      }

      Button->setMenu(menu);
    }

    QString getButtonName() {
      return buttonName;
    }
    std::vector <QAction*> getButtonActionsList() {
      return buttonActionsList;
    }
    void setText(QString text){
      Button->setText(text);
    }
};


#endif
