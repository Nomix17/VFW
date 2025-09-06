#ifndef CHANGETHEME_W
#define CHANGETHEME_W

#include <fstream>
#include <sstream>
#include <filesystem>

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>

class ChangeThemeWindow : public QDialog {
  Q_OBJECT;

  QVBoxLayout *mainlayout = new QVBoxLayout(this);
  QScrollArea *scrollarea = new QScrollArea;
  QGridLayout *medialayout = new QGridLayout;
  QPushButton *doneButton = new QPushButton("Exit");
  QHBoxLayout *doneButtonHolder = new QHBoxLayout;
  QWidget *holderwidget = new QWidget;

public:
  std::string changetotheme = "";

  ChangeThemeWindow(QWidget *parent, std::string ConfigDirectory, std::string ProjectDirectory, std::string StyleDirectory)
      : QDialog(parent) {
    this->setFixedSize(400,500);
    scrollarea->setWidgetResizable(true);
    scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    holderwidget->setLayout(medialayout);
    holderwidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    holderwidget->setMinimumWidth(300);

    scrollarea->setWidget(holderwidget);
    // Add scroll area directly to main layout
    mainlayout->addWidget(scrollarea);

    // Done button setup
    doneButton->setObjectName("donebutton");
    doneButtonHolder->addStretch();
    doneButtonHolder->addWidget(doneButton);
    doneButtonHolder->addStretch();

    mainlayout->addLayout(doneButtonHolder);

    // Load themes and styles
    loadthemes(StyleDirectory, ProjectDirectory,ConfigDirectory);

    // Connect done button
    connect(doneButton, &QPushButton::clicked, [this, ConfigDirectory]() {
      QDialog::accept();
    });
  }

  void loadthemes(std::string StyleDirectory, std::string ProjectDirectory, std::string ConfigDirectory) {
    // Load stylesheet
    std::ifstream stylefile(StyleDirectory + "/ChangeThemeWindow.css");
    if (stylefile) {
      std::ostringstream sstr;
      sstr << stylefile.rdbuf();
      std::string script = sstr.str();
      this->setStyleSheet(QString::fromStdString(script));
      stylefile.close();
    }

    // Load theme buttons
    int counter = 0;
    for (auto &file : std::filesystem::directory_iterator(ProjectDirectory)) {
      std::string themename = file.path().filename().string();

      QPushButton *Theme_Button = new QPushButton(QString::fromStdString(themename));
      Theme_Button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
      medialayout->addWidget(Theme_Button, counter, 0);
      connect(Theme_Button,&QPushButton::clicked, [this, themename, ConfigDirectory](){
        changetotheme = themename;
        if (changetotheme != "") {
          std::ofstream themefile(ConfigDirectory + "/theme", std::ofstream::out | std::ofstream::trunc);
          themefile << changetotheme;
          themefile.close();
        }
        QDialog::accept();
      });
      counter++;
    }

    // Add stretch at the end to push buttons to top
    medialayout->setRowStretch(counter, 1);
  }
};

#endif
