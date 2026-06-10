#ifndef TOPBAR
#define TOPBAR

#include <QHBoxLayout>
#include "ToolMenu.h"
#include "MenuButton.h"

class TopBar : public QHBoxLayout, public ToolMenu {
  Q_OBJECT
  signals:
    void handleButtonsClick(int actionNumber);

  public:
    TopBar(QWidget* parent = nullptr) : QHBoxLayout(parent) {
      this->setAlignment(Qt::AlignLeft);
      buildButtons(
        [this](MenuButton* btn) {
          QObject::connect(btn, &MenuButton::buttonClicked, [this](int n){
            emit handleButtonsClick(n);
          });
          this->addWidget(btn);
        }
      );
    }
};

#endif
