//you change the media position manually alot in this code make a function that do that and call it in line 725, 641, 593, 566 
#include <QApplication>
#include <mainwindow.h>
#include <mediaurl.h>
#include <jumptotime.h>
#include <subconfig.h>
#include <SRepeatWindow.h>
#include <iomanip>
#include <iostream>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <fstream>
#include <QAction>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QLabel>
#include <QKeyEvent>
#include <QFileDialog>
#include <chrono>
#include <QGraphicsTextItem>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent){
    this->setFocus();
    this->resize(750,550);
    //elements definition
    player = new QMediaPlayer(this);
    audio = new QAudioOutput(this);

    video = new QGraphicsVideoItem();
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene,this);

    mainwidget = new QWidget(this);
    mainlayout = new QVBoxLayout();
    firstlayout = new QHBoxLayout();
    videolayout = new QGridLayout();
    thirdlayout = new QHBoxLayout();
    fourthlayout = new QHBoxLayout();
    videoslider = new QSlider(Qt::Horizontal);

    QFont font;
    sublabel = new QGraphicsTextItem();
    sublabel->setDefaultTextColor(Qt::white);
    font.setPointSize(24);
    sublabel->setFont(font);
    
    currenttimer = new QLabel("--:--:--");
    totaltimer = new QLabel("--:--:--");
    volumeslider = new QSlider(Qt::Horizontal);

    sublabel->setObjectName("sublabel");
    volumeslider->setObjectName("volumeslider");


    //adding margin for style
    mainlayout->setContentsMargins(10,10,10,10);

    //align the buttons for  style
    firstlayout->setAlignment(Qt::AlignLeft);
    fourthlayout->setAlignment(Qt::AlignLeft);

    //setting firstlayout toolbuttons with it's actions
    size_t counter=0;
    for(qsizetype i=0;i<firstlayoutbuttons.size();i++){
      //making toolbuttons basing on the elements of firstlayoutbuttons list
      QToolButton *button = new QToolButton(this);
      button->setPopupMode(QToolButton::InstantPopup);
      button->setText(firstlayoutbuttons[i]);
      button->setObjectName(firstlayoutbuttons[i]);
      connect(button,&QPushButton::clicked,[this,i](){
          firstlayoutclick(i);
      });

      //creat a menu for eachbutton
      QMenu *menu = new QMenu(this);
      if(actionslist.size()>i){
      //loop the elements in actionslist of the i button 
        for(qsizetype j=0;j<actionslist[i].size();j++){
            QAction *action = new QAction(this);
            action->setObjectName(actionslist[i][j]);
            action->setText(actionslist[i][j]);
            connect(action,&QAction::triggered,[this,i,j,counter](){
              firstlayoutclick(counter);
            });
            // connect these actions later
            menu->addAction(action);
            counter++;
        }
      }
      button->setMenu(menu);
      firstlayout->addWidget(button);
    }

    //setting fourthlayout pushbuttons
    for(int j=0;j<mcbuttons.size();j++){
        //adding space for the style 
        if(j==1 || j==4){
            fourthlayout->addSpacing(20);
        }else if(j==7){
            //adding space for the style between buttons and volume parameters 
            fourthlayout->addStretch(100);
        }
        QPushButton *button = new QPushButton(this);
        button->setObjectName(mcbuttons[j]);
        QPixmap pix("/home/pain/.config/VFW/cache/icons/"+mcbuttons[j]+".png");
        button->setIcon(pix);
        if (button->objectName() == "BVolumeControl"){
            button->setIconSize(QSize(20,20));
        }else{
            button->setIconSize(QSize(16,16));
        }
        connect(button,&QPushButton::clicked,[this,j](){
            fourthlayoutclick(j);
        });
        fourthlayout->addWidget(button);
    }

    //adding volumeslider to the fourthlayout
    fourthlayout->addWidget(volumeslider);

    //connecting volume slider and the audiooutput volume
    connect(volumeslider,&QSlider::sliderMoved,this,&MainWindow::slidertovolume);
    connect(audio,&QAudioOutput::volumeChanged,this,&MainWindow::volumetoslider);

    //connecting the slider and media with there logic
    connect(player,&QMediaPlayer::positionChanged,this,&MainWindow::setsliderposition);
    connect(player,&QMediaPlayer::durationChanged,this,&MainWindow::setsliderrange);
    connect(videoslider,&QSlider::sliderMoved,this,&MainWindow::mediaposition);

    //setting QGraphics parametres
    video->setSize(view->size());
    view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    view->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene->addItem(video);
    scene->addItem(sublabel);
    //adding widgets to there layouts and the layous to the central widget
    videolayout->addWidget(view);
    thirdlayout->addWidget(currenttimer);
    thirdlayout->addWidget(videoslider);
    thirdlayout->addWidget(totaltimer);
    mainlayout->addLayout(firstlayout);
    mainlayout->addLayout(videolayout);
    mainlayout->addLayout(thirdlayout);
    mainlayout->addLayout(fourthlayout);
    mainwidget->setLayout(mainlayout);
    setCentralWidget(mainwidget);

    //showing a blackscreen
    mediaplayer("blackscreen");
    
    //load the sub style
    SubConfig win;
    htmlstyle = win.makehtml();
    subpadding = win.padding;
    submarginbottom = win.marginbottom;
}

void MainWindow::mediaplayer(QString url){

    //if there is no video to play a black image will play (blackscreen)
    video->setSize(view->size());
    if(videoindex>playlist.size()||url=="blackscreen") {
        player->setSource(QUrl("blackscreen"));
        currenttimer->setText("--:--:--");
        totaltimer ->setText("--:--:--");
        videoslider->setStyleSheet("QSlider::handle{background-color: #1e1e1e;}");
        }
    
    //if pass "play a list" as an argunent a video from the playlist will play
    else if(url=="play a list"){
        player->setSource(QUrl(playlist[videoindex]));
        videoslider->setStyleSheet("QSlider::handle{background-color: #0f353a;}");
        }
    
    //if we pass a url, a video with the url will play and the playlist will be cleared
    else{
        player->setSource(QUrl(url));
        videoslider->setStyleSheet("QSlider::handle{background-color: #0f353a;}");
        }

    //mediaplayer setup (sound and video widget)

    player->setVideoOutput(video);
    player->setAudioOutput(audio);
    audio->setVolume(0.5);
    volumeslider->setRange(0,1000);
    volumeslider->setSliderPosition(500);
    video->show();
    player->play();

}

//firstlayout buttons logic
void MainWindow::firstlayoutclick(int buttonindex){
    QString url;
    QString suburl;
    switch(buttonindex){
        
        //if the user choose to open a file
        case Open_file:{
          url = QFileDialog::getOpenFileName(this,tr("Select Video File"),"/home/pain/Downloads",tr("Mp4 files (*.mp4)"));
          if(!url.isEmpty()){
              mediaplayer(url);
              playertype = "vid";
          }
          playlist.clear(); //clearing the playlist
          break;
        }

        //if the user choose to open a directory (playlist)
        case Open_folder:{
          url = QFileDialog::getExistingDirectory(this,tr("Setect Playlist Directory","/home/pain/Downloads", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
          if(!url.isEmpty()){
              
              playlist.clear(); //clearing the playlist
              //saving all the urls in a list
              for(auto i : std::filesystem::directory_iterator(url.toStdString())){
                  playlist.push_back(QUrl(QString::fromStdString(i.path())));
              }
              playertype = "playlist";
              mediaplayer("play a list");
            }
          break;
        }

        //if the user choose to open a media file
        case Open_media:{
          //launching the constructor for url window input
          UrlWindow x;
          x.exec();
          //getting the url inputed by the user
          url = x.url;
          //checking if the user set a url or not
          if(!url.isEmpty()){
              playlist.clear();
              playertype="vid";
              mediaplayer(url);
          }
          break;
        }
        //if the user choose to quit the app
        case Quit:{
          QApplication::quit();
          break;
        }
        
        //getting the title of the video
        case Title:{
          break;
        }
        
        //jump backward (player)
        case JUMP_BACKWARD:{
          player->setPosition(player->position()-5000);
          break;
        }
        
        //jump forward (player)
        case JUMP_FORWARD:{
          player->setPosition(player->position()+5000);
          break;
        }
        
        //jump to a specific time
        case JUMP_TO_TIME:{
            JumpTime x;
            x.exec();
            if (x.targettime>=0){
                player->stop();
                int volume = audio->volume();
                delete audio;
                audio = new QAudioOutput();
                player->setPosition(x.targettime*1000);
                player->setAudioOutput(audio);
                audio->setVolume(volume);
                player->pause();
                player->play();
            }
            break;
        }
        //loop on segment of the video 
        case LOOPSEGMENT:{
            SRepeatWindow win;
            win.exec();
            if(win.startingpoint>0 && win.finishingpoint>0){
                repeatfromposition = true;
                startingpoint=win.startingpoint;
                finishpoint=win.finishingpoint;
                player->stop();
                int volume = audio->volume();
                delete audio;
                audio = new QAudioOutput();
                player->setPosition(startingpoint*1000);
                player->setAudioOutput(audio);
                audio->setVolume(volume);
                player->pause();
                player->play();
            }
            break;
        }
        //break the segment loop
        case BREAKLOOP:{
            repeatfromposition = false;
            break;
        }
        //setting the audio to full volume
        case FULL_VOLUME:{
          volumetoslider(1);
          break;
        }
        
        //setting the audio to mute
        case MUTE:{
          volumetoslider(0);
          break;
        }
        
        //set a video radio
        case SET_RADIO:{
          //setting a whole new window
          break;
        }
        
        //if the user choose to open a sub file
        case ADDSUB:{
          suburl = QFileDialog::getOpenFileName(this,tr("Select Subtitle file"),"/home/pain",tr("Srt files (*.srt)"));
          if(!suburl.isEmpty()){
            subscraper(suburl.toStdString());
          }
          break;
        }
        
        //if the user choose to not show a sub 
        case STOPSUB:{
          subtimer.clear();
          sublines.clear();
          break;
        }
        case ADDDELAY:{
            if(subtimer.size()){
                for(size_t i=0;i<subtimer.size();i++){
                    subtimer[i]+=0.1;
                }
                subdelay+=0.1;
            }
            break;
        }
        case REDUCEDELAY:{
            if(subtimer.size()){
                for(size_t i=0;i<subtimer.size();i++){
                    if(subtimer[0]>0){
                        subtimer[i]-=0.1;
                    }
                }
                subdelay-=0.1;
            }
            break;
        }
        //if the user choose to custumize the sub
        case SUBSETTINGS:{
          SubConfig win;
          win.gui();
          win.exec();
          htmlstyle = win.makehtml();
          subpadding = win.padding;
          submarginbottom = win.marginbottom;
          break;
        }
    }
    //add animation so the user can see that the delay has been changed
    if(buttonindex==ADDDELAY|| buttonindex==REDUCEDELAY){
        if(subtimer.size()){
            //creating the text
            QGraphicsTextItem * delaytext = new QGraphicsTextItem;
            //if the delay is less then 0.1 and more then -0.1 I want it to be 0
            if(-0.1<subdelay && subdelay<0.1){
                subdelay=0;
            }
            //show the state of the delay using the same font config of the subtitles
            delaytext->setHtml(htmlstyle + "Subtitles Delay:"+QString::number(subdelay*1000) + "ms </div>");
            
            //calculating the position that the text should go to
            int textwidth = delaytext->boundingRect().width();
            int textheight = delaytext->boundingRect().height();
            delaytext->setPos((view->size().width()-textwidth)/2,(view->size().height()-textheight)/2);
            scene->addItem(delaytext);
            
            //making an effect for the apearence and deapearence of the text
            QGraphicsOpacityEffect * opacityEffect = new QGraphicsOpacityEffect(delaytext);
            delaytext->setGraphicsEffect(opacityEffect);
            //configuring the animation of the text
            QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect,"opacity");
            animation->setDuration(1000);
            animation->setStartValue(1.0);
            animation->setEndValue(0.0);
            animation->start(QPropertyAnimation::DeleteWhenStopped);//start the animation 
            //after 1s delete the text
            QTimer::singleShot(1000, [delaytext]() {
                delete delaytext;
            });
        }
    }
    
    this->setFocus();
}

//fourthlayout buttons logic
void MainWindow::fourthlayoutclick(int buttonindex){
    switch(buttonindex){

        //if the pause button is clicked
        case PAUSE_BUTTON:
            {QPushButton *searchbutton = this->findChild<QPushButton *>("BPause");
            if(paused){
                searchbutton->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BPause.png"));
                player->play();
            }else{
                searchbutton->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BPlay.png"));
                player->pause();
            }
            paused=!paused;
            break;}
        
        //if the prevous video button is clicked
        case BACK_BUTTON:
            {if(playertype=="playlist" && videoindex>0){
            videoindex--;
            mediaplayer("play a list");}
            break;}
            

        //if the stop button is clicked
        case STOP_BUTTON:
            {player->setSource(QUrl("blackscreen"));
            currenttimer->setText("--:--:--");
            totaltimer ->setText("--:--:--");
            playlist.clear();
            playertype="video";
            break;}
        
        //if the next video button is clicked
        case NEXT_BUTTON:
            {if(playertype=="playlist"){
            //we set the player to the end of the video so it trigger the logic in the lines (326,...354)
            player->setPosition(player->duration());}
            break;}
        //if fullscreen button is clicked
        case FULLSCREEN_BUTTON:
            {if(fullscreened){
                this->showMaximized();
                volumeslider->show();
                currenttimer->show();
                totaltimer->show();
                videoslider->show();
                mainlayout->setContentsMargins(10,10,10,10);

            }else{
                this->showFullScreen();
                volumeslider->hide();
                currenttimer->hide();
                totaltimer->hide();
                videoslider->hide();
                mainlayout->setContentsMargins(0,0,0,0);
                video->setSize(this->size());
                view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
            }

            for(int i=0;i<firstlayoutbuttons.size();i++){
                QToolButton *searchtoolbutton = this->findChild<QToolButton *>(firstlayoutbuttons[i]);
                if(searchtoolbutton){
                    if(fullscreened){
                        searchtoolbutton->show();
                    }else{
                        searchtoolbutton->hide();

                    }
                }
            }
            for(int i=0;i<mcbuttons.size();i++){
                QPushButton *seachpushbutton = this->findChild<QPushButton *>(mcbuttons[i]);
                if(seachpushbutton){
                    if(fullscreened){
                        seachpushbutton->show();
                    }else{
                    seachpushbutton->hide();
                    }
                }
            }
            fullscreened=!fullscreened;
            break;
        }
        
        case PLAYLIST_BUTTON:{
            //showing the full playlist that is playing
        }
        
        //if reloading behavior is clicked 
        case REPETITION_BUTTON:
            {QPushButton * sb = this->findChild<QPushButton*>("BRepeating");
            if(rep==PlaylistRepeat){
                //repeat playlist
                sb->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BRepeatingone.png"));
                rep=VideoRepeat;
            }else if (rep==VideoRepeat){
                //repeating one video
                sb->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BSuffle.png"));
                rep=Shuffle;
            }else if (rep==Shuffle){
                //shuffle
                sb->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BRepeating.png"));
                rep=PlaylistRepeat;
            }
            break;
        }
        
        //mute and unmute
        case BVolumeControl:
            {if (audio->volume()){
                oldvolume=audio->volume();
                volumetoslider(0);
            }else{
                volumetoslider(oldvolume);
            }
            break;
        }
    }
    this->setFocus();
}


//keyboard event catching function
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_Tab){
        event->ignore();
    }else if(event->key()  == Qt::Key_Escape){
       fullscreened = true;
       fourthlayoutclick(4);
    }else if(event->key()  == Qt::Key_F){
       fourthlayoutclick(4);
    }else if(event->key()  == Qt::Key_M){
       fourthlayoutclick(BVolumeControl);
    }else if(event->key()==Qt::Key_Space){
       fourthlayoutclick(0);
    }else if(event->key()==Qt::Key_Right){

        //calculating when was the last click (left or right)
        std::chrono::duration<double> time = std::chrono::system_clock::now()-now;
        //if the time is less then 500 ms  it will only increasing the position
        if(time.count()<0.5){
            player->setPosition(player->position()+5000);
            player->pause();
            player->play();
        //if the time is more then 500 ms 
        }else{
            //saving the position of the player
            int myposition = player->position();
            player->stop();
            //resetting the audio carrier so the audio will not break 
            delete audio;
            audio = new QAudioOutput(this);
            //increasing the position of the player
            player->setPosition(myposition+5000);
            player->setAudioOutput(audio);
            //pause and play the player to reset the player
            player->pause();
            player->play();

        }
        now = std::chrono::system_clock::now();

    }else if(event->key()==Qt::Key_Left){

        //calculating when was the last click (left or right)
        std::chrono::duration<double> time = std::chrono::system_clock::now()-now;
        //if the time is less then 500 ms  it will only decrease the position
        if(time.count()<0.5){
            player->setPosition(player->position()-5000);
            player->pause();
            player->play();
        //if the time is more then 500 ms 
        }else{
            //saving the position of the player
            int myposition = player->position();
            player->stop();
            //resetting the audio carrier so the audio will not break 
            int volume = audio->volume(); //saving the volume state1
            delete audio;
            audio = new QAudioOutput(this);
            //decreasing the position of the player
            player->setPosition(myposition-5000);
            //addting the audio carrier to the player
            player->setAudioOutput(audio);
            audio->setVolume(volume);
            //pause and play the player to reset the player
            player->pause();
            player->play();

        }
        now = std::chrono::system_clock::now();

    }else if(event->key()==Qt::Key_Up){
        volumetoslider(audio->volume()+0.1);
        volumeslider->setSliderPosition(volumeslider->sliderPosition()+0.1);
    }else if(event->key()==Qt::Key_Down){
        volumetoslider(audio->volume()-0.1);
        volumeslider->setSliderPosition(volumeslider->sliderPosition()-0.1);
    }else if(event->key()==Qt::Key_G){
        firstlayoutclick(REDUCEDELAY);
    }else if(event->key()==Qt::Key_H){
        firstlayoutclick(ADDDELAY);
    }

}


// slider and player relationship

//setting the player position basing on the slider position
void MainWindow::mediaposition(int position){

    //calculate the time of the between the last change of position and now
    std::chrono::duration<double> time = std::chrono::system_clock::now()-now;
    //if the time is less then 500 ms  it will only change the position
    if(time.count()<0.5){
        player->setPosition(position);
        player->pause();
        player->play();
    //if the time is more then 500 ms 
    }else{
        //saving the position of the player
        player->stop();
        //resetting the audio carrier so the audio will not break 
        delete audio;
        audio = new QAudioOutput(this);
        //change the position of the player
        player->setPosition(position);
        player->setAudioOutput(audio);
        //pause and play the player to reset the player
        player->pause();
        player->play();

    }
    now = std::chrono::system_clock::now();
    this->setFocus();

}

//setting the range of the slider basing on the player
void MainWindow::setsliderrange(qint64 position){
    videoslider->setRange(0,position);
}

//setting the app elements in there relation with the player
void MainWindow::setsliderposition(qint64 position){
    videoslider->setValue(position);

    //if the video is still playing (position != video duration) 
    if(position!=player->duration()){

        //setting the current timer basing on the player position
        int hour = player->position()/(1000*60*60);
        int min = (player->position()/1000-hour*60*60)/60;
        int second = player->position()/1000-min*60-hour*60*60;
        std::ostringstream osshour, ossmin, osssecond;
        osshour<<std::setfill('0')<<std::setw(2)<<hour;
        ossmin<<std::setfill('0')<<std::setw(2)<<min;
        osssecond<<std::setfill('0')<<std::setw(2)<<second;
        currenttimer->setText(QString::fromStdString(osshour.str())+":"+
            QString::fromStdString(ossmin.str())+":"+
            QString::fromStdString(osssecond.str()));
        
        //setting the total timer basing on the player duration
        int thour = player->duration()/(1000*60*60);
        int tmin = (player->duration()/1000-thour*60*60)/60;
        int tsecond = player->duration()/1000-tmin*60-thour*60*60;
        std::ostringstream tosshour, tossmin, tosssecond;
        tosshour<<std::setfill('0')<<std::setw(2)<<thour;
        tossmin<<std::setfill('0')<<std::setw(2)<<tmin;
        tosssecond<<std::setfill('0')<<std::setw(2)<<tsecond;
        totaltimer->setText(QString::fromStdString(tosshour.str())+":"+
            QString::fromStdString(tossmin.str())+":"+
            QString::fromStdString(tosssecond.str()));

    }
    //if the video is finished
    else if (position==player->duration()){
        //if the reloading button is in the "reload full playlist" mode
        if(rep==PlaylistRepeat){
            if(videoindex==playlist.size()-1){
                videoindex = 0;
            }else{
                videoindex++;
            }
            mediaplayer("play a list");
        }

        //if the reloading button is in the "reload one video" mode
        else if (rep==VideoRepeat){
            player->setPosition(0);
            player->stop();
            player->play();
        }

        //if the reloading button is in the "random video" mode
        else if (rep==Shuffle){
            videoindex = rand()%playlist.size();
            mediaplayer("play a list");
        }
        videoslider->setValue(0);
    }


    if(repeatfromposition){
        if(position>=finishpoint*1000){
                player->stop();
                int volume = audio->volume();
                delete audio;
                audio = new QAudioOutput();
                player->setPosition(startingpoint*1000);
                player->setAudioOutput(audio);
                audio->setVolume(volume);
                player->pause();
                player->play();
        }
    }
  //syncing subtitles to the player position 
  //looping all the times that exist in the sub file
  for(size_t i=0;i<subtimer.size();i+=2){
    //checking if the player position is between the 2 times
    if(subtimer[i]*1000<=position && subtimer[i+1]*1000>=position){
        //getting the size of the view and the sub
        sublabel->setOpacity(1);
        //adding 100ms delay to the sub apearence until the sub is fully randered
        if(subtimer[i]*1000<=position && subtimer[i]*1000+100>position){
            sublabel->setOpacity(0);
        }
        int VIEWWIDTH = view->size().width();
        int VIEWHEIGHT = view->size().height();
        int SUBWIDTH = sublabel->boundingRect().width();
        int SUBHEIGHT = sublabel->boundingRect().height();
        //posetioning the sub on the correct spot
        sublabel->setPos((VIEWWIDTH-SUBWIDTH)/2,(VIEWHEIGHT-SUBHEIGHT/2)-submarginbottom);
      
      //if the media is in the targeted position we merge the html style with the subtitle and pass it as html script
      sublabel->setHtml(htmlstyle + QString::fromStdString(sublines[i/2]) + "</div>");
      break;
    }else if(i==subtimer.size()-2){
      //if the media is not in a target position we pass an empty string
      sublabel->setHtml("");
    }
  }
}


//volume logic 
void MainWindow::slidertovolume(int position){
    audio->setVolume(position/1000.0);
}
void MainWindow::volumetoslider(qreal position){
    QPushButton *searchbutton = this->findChild<QPushButton*>("BVolumeControl");
    audio->setVolume(position);
    //changing the volume button icon basing on the volume state
    if(position*1000 == 0){
        searchbutton->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BMute.png"));
        volumeslider->setStyleSheet("QSlider#volumeslider::handle{background:#1e1e1e;}");
    }else if (position*1000<=333 && position*1000>0){
        searchbutton->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BVolumeLow.png"));
        volumeslider->setStyleSheet("QSlider#volumeslider::handle{background:#484949;}");

    }else if(position*1000>=333 && position*1000<=666){
        searchbutton->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BVolumeMid.png"));

    }else if(position*1000>=666){
        searchbutton->setIcon(QPixmap("/home/pain/.config/VFW/cache/icons/BVolumeControl.png"));

    }
    volumeslider->setSliderPosition(static_cast<int>(position*1000));
}

//scraping the subtitles from the sub file
void MainWindow::subscraper(std::string subpath){
  std::ifstream file(subpath);
  std::string line;

  //clearing the sub lists (so i can load a sub even when another is loaded)
  subtimer.clear();
  sublines.clear();
  
  //looping the lines in the file
  while(getline(file,line)){
    int lettercounter=0;
    bool timefound=false;
    //looping the characters in the line
    while(line[lettercounter]!='\0'){
      //if the line content the set of characters "-->"
      if(line[lettercounter]=='-'&&line[lettercounter+1]=='-'&&line[lettercounter+2]=='>'){
        //calculating the starting time from the string line (hour, minutes, seconds)
        double fhour = ((line[0]-'0')*10 + (line[1]-'0'))*60*60;
        double fmin = ((line[3]-'0')*10 + (line[4]-'0'))*60;
        double fsec = (line[6]-'0')*10 + (line[7]-'0') +(line[9]-'0')*0.1 + (line[10]-'0')*0.01 + (line[11]-'0')*0.001;
        double firsttime = fhour+fmin+fsec;

        //calculating the ending time from the string line (hour, minutes, seconds)
        double shour = ((line[17]-'0')*10 + (line[18]-'0'))*60*60;
        double smin = ((line[20]-'0')*10 + (line[21]-'0'))*60;
        double ssec = (line[23]-'0')*10 + (line[24]-'0') +(line[26]-'0')*0.1 + (line[27]-'0')*0.01 + (line[28]-'0')*0.001;
        double secondtime = shour+smin+ssec;
      
        //adding the starting time and the end time in a list
        subtimer.push_back(firsttime);
        subtimer.push_back(secondtime);

        //bool variable to know what does the line content (times in this case)
        timefound=true;
        break;
      }
      lettercounter+=1;
    }

  //if the line is after a time line
    if(timefound){
      //adding the sub lines to a list
      std::string nextline;
      std::string fulltext="";
      //checking if there is a next line
      while(getline(file,nextline)){
        //checking if the line is empty
        if(nextline.size()>2){
          //if the line is not empty is will add it to a bandal
          fulltext+=nextline+"<br>";
        }else{
          //if the line is empty it will break (so only the subs are added to the variable(fulltext))
          break;
        }
      }
      //adding the subs to the list
      sublines.push_back(fulltext);
      timefound=false;
    }
  }
}


void MainWindow:: resizeEvent(QResizeEvent * event){
    QMainWindow::resizeEvent(event);
    int VIEWWIDTH = view->size().width();
    int VIEWHEIGHT = view->size().height();
    
    video->setSize(QSize(VIEWWIDTH+2,VIEWHEIGHT+2));
    scene->setSceneRect(0,0,VIEWWIDTH-1,VIEWHEIGHT-1);
    view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    
    int SUBWIDTH = sublabel->boundingRect().width();
    int SUBHEIGHT = sublabel->boundingRect().height();
    sublabel->setPos((VIEWWIDTH-SUBWIDTH)/2,(VIEWHEIGHT-SUBHEIGHT/2)-submarginbottom);

}
