#ifndef URLWINDOW_H
#define URLWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QString>
#include <QFile>

class UrlWindow : public QDialog {
  Q_OBJECT

public:

  //constructor for the url window input
  UrlWindow(QWidget *parent,std::string projectpath):QDialog(parent){
    this->setFocus();
    QFile file(QString::fromStdString(projectpath)+"mediaurl.css");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&file);
      QString styleSheet = in.readAll();
      this->setStyleSheet(styleSheet);
    }
    // this->setFixedSize(800,500);
    mainlayout = new QVBoxLayout;
    firstlayout = new QHBoxLayout;
    secondlayout = new QHBoxLayout;
    urllabel = new QLabel("URL: ");
    urlentry = new QTextEdit;
    donebutton = new QPushButton("OK");
    donebutton->setObjectName("okbutton");
    cancelbutton=new QPushButton("Cancel");
    connect(donebutton,&QPushButton::clicked,[this](){
      url = urlentry->toPlainText();
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
  QTextEdit *urlentry;
  QVBoxLayout *mainlayout;
  QHBoxLayout *firstlayout;
  QHBoxLayout *secondlayout;
  QLabel *urllabel;
  QPushButton *donebutton;
  QPushButton * cancelbutton;
};

#endif
