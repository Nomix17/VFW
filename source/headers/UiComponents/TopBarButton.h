#ifndef TOPBARBUTTON
#define TOPBARBUTTON

#include <iostream>
#include <QToolButton>
#include <QMenu>
#include <QWidget>
#include <QHBoxLayout>


class TopBarToolButton : public QWidget {
  Q_OBJECT
  signals:
    void buttonClicked(int actionNumber);

  public:
    QToolButton* Button;
    QString buttonName;
    inline static std::vector <QAction*> buttonsActionsList;

    TopBarToolButton(QString buttonName, QWidget* parent = nullptr) 
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

    void createMenuElements(QList<QString> actionslist) {
      QMenu *menu = new QMenu(this);
      for (qsizetype i = 0; i < actionslist.size(); i++) {
        QAction *action = new QAction(this);
        action->setObjectName(actionslist[i]);
        action->setText(actionslist[i]);
        int actionNumber = buttonsActionsList.size();
        connect(action, &QAction::triggered, [this,actionNumber]() {
          emit buttonClicked(actionNumber); 
        });
        buttonsActionsList.push_back(action);
        menu->addAction(action);
      }

      Button->setMenu(menu);
    }

    static std::vector <QAction*> getActionsList(){
      return buttonsActionsList;
    }
    
    void setText(QString text){
      Button->setText(text);
    }
};


#endif
