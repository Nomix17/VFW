#ifndef LOOPSEGMENT_WINDOW
#define LOOPSEGMENT_WINDOW

#include<QDialog>
#include <QVBoxLayout>
#include<QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QFile>

class SRepeatWindow :public QDialog{
  Q_OBJECT;
  
public:
  SRepeatWindow(QWidget *parent=nullptr):QDialog(parent){
    this->setFocus();
    QFile file("/home/pain/.config/VFW/cache/styles/jumpwindow.css");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString styleSheet = in.readAll();
        this->setStyleSheet(styleSheet);
    }
    QLabel * FROM = new QLabel("FROM:");
    QSpinBox *startinghour = new QSpinBox();
    QSpinBox *startingmin = new QSpinBox();
    QSpinBox *startingsec = new QSpinBox();
    
    QLabel *startingseparet1 = new QLabel(":");
    QLabel *startingseparet2 = new QLabel(":");

    QLabel * TO = new QLabel("TO:");
    QSpinBox *finishinghour = new QSpinBox();
    QSpinBox *finishingmin = new QSpinBox();
    QSpinBox *finishingsec = new QSpinBox();

    QLabel *finishingseparet1 = new QLabel(":");
    QLabel *finishingseparet2 = new QLabel(":");

    firstlayout->addWidget(FROM);
    firstlayout->addWidget(startinghour);
    firstlayout->addWidget(startingseparet1);
    firstlayout->addWidget(startingmin);
    firstlayout->addWidget(startingseparet2);
    firstlayout->addWidget(startingsec);

    firstlayout->addStretch();

    firstlayout->addWidget(TO);
    firstlayout->addWidget(finishinghour);
    firstlayout->addWidget(finishingseparet1);
    firstlayout->addWidget(finishingmin);
    firstlayout->addWidget(finishingseparet2);
    firstlayout->addWidget(finishingsec);

    QPushButton *done = new QPushButton("OK");
    QPushButton *cancel= new QPushButton("Cancel");

    connect(done,&QPushButton::clicked,[this,startinghour,startingmin,startingsec,finishinghour,finishingmin,finishingsec](){
      startingpoint = startinghour->value()*60*60+startingmin->value()*60+startingsec->value();
      finishingpoint = finishinghour->value()*60*60+finishingmin->value()*60+finishingsec->value();
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

public:
  int startingpoint = -1;
  int finishingpoint = -1;
private:
  QVBoxLayout *mainlayout = new QVBoxLayout();
  QHBoxLayout *firstlayout = new QHBoxLayout();
  QHBoxLayout * secondlayout = new QHBoxLayout();
};








#endif