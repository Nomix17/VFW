#ifndef URLWINDOW_H
#define URLWINDOW_H

#include <fstream>
#include <sstream>
#include <filesystem>

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class UrlWindow : public QDialog {
  Q_OBJECT

public:

  //constructor for the url window input
  UrlWindow(std::string StyleDirectory, QWidget *parent=nullptr):QDialog(parent){

    //load style file
    std::filesystem::path styleFullPath(std::filesystem::path(StyleDirectory) / "mediaurl.css");
    std::ifstream stylefile(styleFullPath);
    if(stylefile){
      std::string script;
      std::ostringstream ss;
      ss<<stylefile.rdbuf();
      script = ss.str();
      this->setStyleSheet(QString::fromStdString(script));
      stylefile.close();
    }

    // this->setFixedSize(800,500);
    mainlayout = new QVBoxLayout;
    firstlayout = new QHBoxLayout;
    secondlayout = new QHBoxLayout;
    urllabel = new QLabel("URL: ");
    urlentry = new QLineEdit;

    donebutton = new QPushButton("OK");
    donebutton->setFocusPolicy(Qt::NoFocus);
    donebutton->setObjectName("okbutton");

    cancelbutton=new QPushButton("Cancel");
    cancelbutton->setFocusPolicy(Qt::NoFocus);

    connect(donebutton,&QPushButton::clicked,[this](){
      url = urlentry->text();
      QDialog::accept();
    });
    connect(cancelbutton,&QPushButton::clicked,[this](){
      QDialog::accept();
    });
    firstlayout->addWidget(urllabel);
    firstlayout->addWidget(urlentry);
    secondlayout->addWidget(donebutton);
    secondlayout->addWidget(cancelbutton);
    mainlayout->addLayout(firstlayout);
    mainlayout->addLayout(secondlayout);
    setLayout(mainlayout);
    setWindowTitle("URL Window");
  }

public:
  QString url="";
private:
  QLineEdit *urlentry;
  QVBoxLayout *mainlayout;
  QHBoxLayout *firstlayout;
  QHBoxLayout *secondlayout;
  QLabel *urllabel;
  QPushButton *donebutton;
  QPushButton * cancelbutton;
};

#endif
