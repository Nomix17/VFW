#ifndef JUMPTIME
#define JUMPTIME

#include <QDialog>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QFile>

class JumpTime : public QDialog{
  Q_OBJECT;

public:
  JumpTime(QWidget*parent,std::string projectpath):QDialog(parent){
    this->setFocus();
    QFile file(QString::fromStdString(projectpath)+"jumpwindow.css");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&file);
      QString styleSheet = in.readAll();
      this->setStyleSheet(styleSheet);
    }
    this->setFixedSize(250,100);
    QSpinBox *hour = new QSpinBox();
    QSpinBox *min = new QSpinBox();
    QSpinBox *sec = new QSpinBox();

    QLabel *separet1 = new QLabel(":");
    QLabel *separet2 = new QLabel(":");
    firstlayout->addWidget(hour);
    firstlayout->addWidget(separet1);
    firstlayout->addWidget(min);
    firstlayout->addWidget(separet2);
    firstlayout->addWidget(sec);
    
    QPushButton *done = new QPushButton("OK");
    QPushButton *cancel= new QPushButton("Cancel");

    connect(done,&QPushButton::clicked,[this,hour,min,sec](){
      targettime = hour->value()*60*60+min->value()*60+sec->value();
      QDialog::accept();
    });
    connect(cancel,&QPushButton::clicked,[this](){
      QDialog::accept();
    });
    secondlayout->addWidget(done);
    secondlayout->addWidget(cancel);

    mainlayout->addLayout(firstlayout);
    mainlayout->addLayout(secondlayout);
    setLayout(mainlayout);
  }

  int targettime=-1;
private:
  QVBoxLayout *mainlayout = new QVBoxLayout();
  QHBoxLayout *firstlayout = new QHBoxLayout();
  QHBoxLayout * secondlayout = new QHBoxLayout();
}; 



#endif