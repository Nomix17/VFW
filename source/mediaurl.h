#ifndef URLWINDOW_H
#define URLWINDOW_H

#include <fstream>
#include <sstream>

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

class UrlWindow : public QDialog {
  Q_OBJECT

public:

  //constructor for the url window input
  UrlWindow(QWidget *parent,std::string StyleDirectory):QDialog(parent){

    //load style file
    std::ifstream stylefile(StyleDirectory+"/mediaurl.css");
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
