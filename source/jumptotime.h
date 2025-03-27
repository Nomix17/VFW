#ifndef JUMPTIME
#define JUMPTIME

#include <fstream>
#include <sstream>

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>

class JumpTime : public QDialog{
  Q_OBJECT;

public:
  JumpTime(QWidget*parent,std::string StyleDirectory):QDialog(parent){

    //load style file
    std::ifstream stylefile(StyleDirectory+"/jumpwindow.css");
    if(stylefile){
      std::string script;
      std::ostringstream ss;
      ss<<stylefile.rdbuf();
      script = ss.str();
      this->setStyleSheet(QString::fromStdString(script));
      stylefile.close();
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