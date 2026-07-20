#ifndef CHANGETHEME_W
#define CHANGETHEME_W

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QString>
#include <QSizePolicy>

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

  ChangeThemeWindow(std::filesystem::path configPath, std::string ProjectDirectory, std::string StyleDirectory, QWidget *parent=nullptr)
      : QDialog(parent) {
    
    this->setWindowTitle("Pick your theme");
    this->setFixedSize(400,500);
    scrollarea->setWidgetResizable(true);
    scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollarea->setFocusPolicy(Qt::NoFocus);

    holderwidget->setLayout(medialayout);
    holderwidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    holderwidget->setMinimumWidth(300);
    holderwidget->setFocusPolicy(Qt::NoFocus);

    scrollarea->setWidget(holderwidget);
    // Add scroll area directly to main layout
    mainlayout->addWidget(scrollarea);

    // Done button setup
    doneButton->setObjectName("donebutton");
    doneButton->setFocusPolicy(Qt::NoFocus);

    doneButtonHolder->addStretch();
    doneButtonHolder->addWidget(doneButton);
    doneButtonHolder->addStretch();

    mainlayout->addLayout(doneButtonHolder);

    // Load themes and styles
    loadthemes(StyleDirectory, ProjectDirectory, configPath);

    // Connect done button
    connect(doneButton, &QPushButton::clicked, [this]() {
      QDialog::accept();
    });
  }

  void loadthemes(std::string StyleDirectory, std::string ProjectDirectory, std::filesystem::path configPath) {
    // Load stylesheet
    std::filesystem::path styleFullPath(std::filesystem::path(StyleDirectory) / "ChangeThemeWindow.css");
    std::ifstream stylefile(styleFullPath);
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
      Theme_Button->setFocusPolicy(Qt::NoFocus);
      medialayout->addWidget(Theme_Button, counter, 0);

      connect(Theme_Button,&QPushButton::clicked, [this, themename, configPath](){
        changetotheme = themename;
        if (changetotheme != "") {
          std::string themePath = (configPath / "theme").string();
          std::ofstream themefile(themePath, std::ofstream::out | std::ofstream::trunc);
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
