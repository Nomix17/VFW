#ifndef CONTEXTMENU
#define CONTEXTMENU

#include <QMenu>
#include <QAction>
#include "ToolMenu.h"
#include "MenuButton.h"

class ContextMenu : public QMenu, public ToolMenu {
  Q_OBJECT
  signals:
    void handleButtonsClick(int actionNumber);
  public:
    ContextMenu(QWidget* parent = nullptr) : QMenu(parent) {
      buildButtons(
        [this](MenuButton* btn) {
          QMenu* sub = this->addMenu(btn->getButtonName());
          for (QAction* action : btn->getButtonActionsList())
            sub->addAction(action);
          QObject::connect(btn, &MenuButton::buttonClicked, [this](int n){
            emit handleButtonsClick(n);
          });
        }
      );
    }
};

#endif
