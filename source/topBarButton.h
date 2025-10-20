#ifndef TOPBAEBUTTON
#define TOPBAEBUTTON

#include <iostream>
#include <QToolButton>
#include <QMenu>


class topBarButton:public QToolButton{

  Q_OBJECT
  signals:
    void handleButtonsClick(int ButtonNumber);

  public:
  QToolButton *button;

  static std::vector <QAction*> TopBarButtonsObjectList;

  topBarButton(QString buttonName):QToolButton(nullptr){
    this->button = new QToolButton(this);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setText(buttonName);
    button->setObjectName(buttonName);
  }

  std::vector <QAction*> setupMenu(QList<QString> actionslist){
    QMenu *menu = new QMenu(this);

    for (qsizetype j = 0; j < actionslist.size(); j++) {

      QAction *action = new QAction(this);
      action->setObjectName(actionslist[j]);
      action->setText(actionslist[j]);

      TopBarButtonsObjectList.push_back(action);
      int numberOfActions = TopBarButtonsObjectList.size()-1;

      connect(action, &QAction::triggered, [this,numberOfActions]() {
        emit handleButtonsClick(numberOfActions); 
      });

      menu->addAction(action);
      numberOfActions++;
    }
    button->setMenu(menu);
    return TopBarButtonsObjectList;
  }

};


#endif
