#ifndef LOOPSEGMENT_WINDOW
#define LOOPSEGMENT_WINDOW

#include <fstream>
#include <sstream>
#include <filesystem>

#include<QDialog>
#include <QVBoxLayout>
#include<QHBoxLayout>

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>

class SRepeatWindow :public QDialog{
  Q_OBJECT;
  
public:
  SRepeatWindow(std::string StyleDirectory, QWidget *parent=nullptr):QDialog(parent){
    this->setWindowTitle("Set Range");
    this->setFixedSize(590,200);
   
    //load style file
    std::filesystem::path styleFullPath(std::filesystem::path(StyleDirectory) / "srepeat.css");
    std::ifstream stylefile(styleFullPath.string());
    if(stylefile){
      std::ostringstream sstr;
      std::string script;
      sstr<<stylefile.rdbuf();
      script = sstr.str();
      this->setStyleSheet(QString::fromStdString(script));
      stylefile.close();
    }

    QLabel * FROM = new QLabel("From: ");
    QSpinBox *startinghour = new QSpinBox();
    QSpinBox *startingmin = new QSpinBox();
    QSpinBox *startingsec = new QSpinBox();
    
    QLabel *startingseparet1 = new QLabel(":");
    startingseparet1->setObjectName("separetor");
    QLabel *startingseparet2 = new QLabel(":");
    startingseparet2->setObjectName("separetor");

    QLabel * TO = new QLabel("To: ");
    QSpinBox *finishinghour = new QSpinBox();
    QSpinBox *finishingmin = new QSpinBox();
    QSpinBox *finishingsec = new QSpinBox();

    QLabel *finishingseparet1 = new QLabel(":");
    finishingseparet1->setObjectName("separetor");
    QLabel *finishingseparet2 = new QLabel(":");
    finishingseparet2->setObjectName("separetor");

    firstlayout->addWidget(FROM);
    firstlayout->addWidget(startinghour);
    firstlayout->addWidget(startingseparet1);
    firstlayout->addWidget(startingmin);
    firstlayout->addWidget(startingseparet2);
    firstlayout->addWidget(startingsec);

    firstlayout->addSpacing(20);

    firstlayout->addWidget(TO);
    firstlayout->addWidget(finishinghour);
    firstlayout->addWidget(finishingseparet1);
    firstlayout->addWidget(finishingmin);
    firstlayout->addWidget(finishingseparet2);
    firstlayout->addWidget(finishingsec);

    QPushButton *done = new QPushButton("OK");
    QPushButton *cancel= new QPushButton("Cancel");
    done->setFocusPolicy(Qt::NoFocus);
    cancel->setFocusPolicy(Qt::NoFocus);

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

    mainlayout->setContentsMargins(20,40,20,20);
    mainlayout->addLayout(firstlayout);
    mainlayout->addStretch();
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
