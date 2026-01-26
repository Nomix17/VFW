#ifndef TOPBAR
#define TOPBAR

#include <iostream>
#include <QToolButton>
#include <QMenu>
#include <QHBoxLayout>
#include "topBarButton.h"

class TopBar : public QHBoxLayout {

  Q_OBJECT
  signals:
    void handleButtonsClick(int actionNumber);

  public:
    static std::vector <QAction*> TopBarButtonsObjectList;

    TopBar(QWidget *parent = nullptr) : QHBoxLayout(parent){
      QList<QString> topbarlayoutbuttons = {
        "Media","Playback","Audio","Video","Subtitle","View","Help"
      };

      QList<QList <QString>> actionslist = {
        {"Open File","Open Folder","Open Media","Quit"},
        {"Jump Backward","Jump Forward","Jump to Time","Move To Next Chapter","Move To Previous Chapter","Start Segment Loop"},
        {"Full Volume","Mute"},
        {"Stretch to Fit"},
        {"Add Subtitle File","Load Subtitle","Hide Subtitles","Add Delay","Reduce Delay","Subtitle Settings"},
        {"Video Title","Display Chapters Indicators","Change Theme"},
        {"Shortcuts Instructions"}
      };

      this->setAlignment(Qt::AlignLeft);

      for (int i = 0; i < topbarlayoutbuttons.length(); i++) {
        QString topToolButtonName = topbarlayoutbuttons[i];
        QList<QString> topToolButtonActionList = actionslist[i];
        TopBarToolButton *newButton = new TopBarToolButton(topToolButtonName);
        newButton->createMenuElements(topToolButtonActionList);
        connect(newButton, &TopBarToolButton::buttonClicked, [this](int actionNumber){
          emit handleButtonsClick(actionNumber);
        });
        this->addWidget(newButton);
      }
    }

    static std::vector <QAction*> getTopBarActionsList(){
      return TopBarToolButton::getActionsList();
    }

};


#endif
