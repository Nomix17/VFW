#include "qaction.h"
#include "qboxlayout.h"
#include "qevent.h"
#include "qpoint.h"
#include "qpushbutton.h"
#include "qtoolbutton.h"
#include "mainwindow.h"
#include "SRepeatWindow.h"
#include "jumptotime.h"
#include "playlistmanager.h"
#include "shortcutsinstructions.h"
#include "ChangeThemeWindow.h"
#include "mediaurl.h"
#include "subWindow.h"
#include "subconfig.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>

#include <QApplication>

#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QAction>

#include <QAudioOutput>
#include <QMediaPlayer>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QFileDialog>
#include <QTimer>
#include <QProcess>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsVideoItem>
#include <QGraphicsTextItem>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsProxyWidget>


void moveSomethingToPos(QGraphicsWidget *widget, QPointF targetPos, int animationTime);
void deletelayout(QLayout* layout);


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setFocusPolicy(Qt::StrongFocus);
  setupShortCuts();
  this->resize(750, 550);
  // elements definition
  player = new QMediaPlayer(this);
  audio = new QAudioOutput(this);

  video = new QGraphicsVideoItem();
  scene = new QGraphicsScene(this);
  view = new QGraphicsView(scene, this);

  mainwidget = new QWidget(this);
  mainlayout = new QVBoxLayout();
  videolayout = new QGridLayout();

  createTopLayout();
  createBottomLayout();

  //creating a floating layout to use it when fullscreening
  floatingControlPannelWidget = new QWidget;
  floatingControlPannelContainerLayout = new QVBoxLayout(floatingControlPannelWidget);
  floatingControlPannelProxy = scene->addWidget(floatingControlPannelWidget);
  floatingControlPannelProxy->setZValue(10);
  floatingControlPannelWidget->update();

  //setting up the subtites
  QFont font;
  sublabel = new QGraphicsTextItem();
  sublabel->setFont(font);
  sublabel->setObjectName("sublabel");
  sublabel->setZValue(12);

  video->setSize(view->size());
  view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scene->addItem(video);
  scene->addItem(sublabel);

  // adding margin for style
  mainlayout->setContentsMargins(10, 10, 10, 10);

  // adding widgets to there layouts and the layous to the central widget
  videolayout->addWidget(view);
  mainlayout->addLayout(topbarlayout);
  mainlayout->addLayout(videolayout);
  mainlayout->addLayout(controlbuttonslayout);
  mainwidget->setLayout(mainlayout);
  setCentralWidget(mainwidget);

  // showing a blackscreen
  mediaplayer();

  // load the sub style
  SubConfig win;
  win.loadFonts(FONTSDIRECTORY);
  htmlstyle = win.makehtml(CONFIGSDIRECTORY);
  subpadding = win.padding;
  submarginbottom = win.marginbottom;

}
//function to create the top layout elements
void MainWindow::createTopLayout(){
  if(topbarlayout != nullptr) delete topbarlayout;

  topbarlayout = new QHBoxLayout();

  size_t counter = 0;
  for (qsizetype i = 0; i < topbarlayoutbuttons.size(); i++) {
    // making toolbuttons basing on the elements of topbarlayoutbuttons list
    QToolButton *button = new QToolButton(this);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setText(topbarlayoutbuttons[i]);
    button->setObjectName(topbarlayoutbuttons[i]);
    connect(button, &QPushButton::clicked, [this, i]() { topbarlayoutclick(i); });

    // creat a menu for eachbutton
    QMenu *menu = new QMenu(this);
    if (actionslist.size() > i) {
      // loop the elements in actionslist of the i button
      for (qsizetype j = 0; j < actionslist[i].size(); j++) {

        QAction *action = new QAction(this);
        action->setObjectName(actionslist[i][j]);
        action->setText(actionslist[i][j]);

        TopBarButtonsObjectList.push_back(action);
        connect(action, &QAction::triggered, [this, i, j, counter]() { topbarlayoutclick(counter); });
        // connect these actions later
        menu->addAction(action);
        counter++;
      }
    }
    button->setMenu(menu);
    topbarlayout->addWidget(button);
  }

  topbarlayout->setAlignment(Qt::AlignLeft);

}


//function to delete a layout passed as argument
void deletelayout(QLayout* layout){
  if(!layout) return;

  while(QLayoutItem* item = layout->takeAt(0)){
    if(QLayout* childlayout = item->layout()){
      deletelayout(childlayout);
    }

    if(QWidget* childwidget = item->widget()){
     childwidget->deleteLater();
    }

    delete item;
  }

}

//function to create the bottom layout
void MainWindow::createBottomLayout(){
  // if the layout isn't null (this means that eather we entered or exited fullscreen mode)
  if(controlbuttonslayout != nullptr){
    deletelayout(controlbuttonslayout);// deleting elements in the layout
    delete controlbuttonslayout; // deleting the layout
    delete volumeslider;
    delete currenttimer;
    delete totaltimer;
  }
  controlbuttonslayout=nullptr;
  ButtonsObjectList.clear(); // clearing the vector that hold the buttons of the bottom layout

  controlbuttonslayout = new QVBoxLayout();//creating new layout
  QHBoxLayout *firsthalflayout = new QHBoxLayout();
  QHBoxLayout *secondhalflayout = new QHBoxLayout();


  videoslider = new CustomSlider(Qt::Horizontal);// creating video slider
  currenttimer = new QLabel("--:--:--");
  totaltimer = new QLabel("--:--:--");
  volumeslider = new CustomSlider(Qt::Horizontal);// create volume slider
  volumeslider->setObjectName("volumeslider");

  connect(videoslider, &QSlider::sliderMoved, [this]() {changingposition(videoslider->sliderPosition());});
  firsthalflayout->addWidget(currenttimer);
  firsthalflayout->addWidget(videoslider);
  firsthalflayout->addWidget(totaltimer);

  connect(volumeslider, &QSlider::valueChanged,[this]() {audio->setVolume((float)volumeslider->value() / 1000);});
  controlbuttonslayout->addLayout(firsthalflayout);
  controlbuttonslayout->addLayout(secondhalflayout);

  // setting controlbuttonslayout pushbuttons
  for (int j = 0; j < mcbuttons.size(); j++) {
    // adding space for the style
    if (j == 1 || j == 4 || j == 7) {
      secondhalflayout->addSpacing(20);
    } else if (j == 8) {
      // adding space for the style between buttons and volume parameters
      secondhalflayout->addStretch(1);
    }
    QPushButton *button = new QPushButton(this);
    button->setObjectName(mcbuttons[j]);

    QPixmap pix(ICONSDIRECTORY + mcbuttons[j] + ".png");
    button->setIcon(pix);
    if (mcbuttons[j] == "BVolumeControl") {
      button->setIconSize(QSize(24, 24));
    } else {
      button->setIconSize(QSize(16, 16));
    }
    if(mcbuttons[j] == "BContinueFLP"){
      button->hide();
    }
    connect(button, &QPushButton::clicked, [this, j]() { controlbuttonslayoutclick(j);});

    ButtonsObjectList.push_back(button);

    secondhalflayout->addWidget(button);
  }
  secondhalflayout->addWidget(volumeslider);
  controlbuttonslayout->setAlignment(Qt::AlignLeft);

  // adding volumeslider to the controlbuttonslayout
  connect(audio, &QAudioOutput::volumeChanged, this, &MainWindow::slidermanagement);
  // connecting the slider and media with there logic
  connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::playertimeline);
  connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::setsliderrange);

  setsliderrange(player->duration());
  videoslider->setValue(player->position());
  updateButtonsIcon();

}
void MainWindow::LoadingInDirectorySubtitles(QString currenturl){
  std::filesystem::path currentVideoPath(currenturl.toStdString()); 
  std::string directoryPath = currentVideoPath.parent_path().string();
  
  for(const auto & fileEntry : std::filesystem::directory_iterator(directoryPath)){
    std::string fileExtention = fileEntry.path().extension();
    if(std::find(supportedSubtitlesFormats.begin(), supportedSubtitlesFormats.end(), fileExtention) != supportedSubtitlesFormats.end()){
      std::string filePathWithoutExtention = fileEntry.path().stem().string();
      std::string currentVideoPathWithoutExtension = currentVideoPath.stem().string();
      if(filePathWithoutExtention == currentVideoPathWithoutExtension){
        subsInVideo.push_back(QString::fromStdString(fileEntry.path().string()));
        if(currentLoadedSubPath == ""){
          currentLoadedSubPath = QString::fromStdString(fileEntry.path().string());
          SubFileParsing(currentLoadedSubPath.toStdString());
          QAction * ToggleSubs = TopBarButtonsObjectList[TOGGLE_SUB];
          ToggleSubs->setText("Remove Subtitles");
        }
        break;
      }
    }
  }
}

void MainWindow::ExtranctingChapterData(QString currenturl){
  ChaptersVectors.clear();
  QProcess ChaptersProcess;
  ChaptersProcess.start("/usr/bin/ffprobe",{
    "-i",currenturl,
    "-show_chapters"
  });
  ChaptersProcess.waitForFinished(-1);
  QStringList Chapters = QString(ChaptersProcess.readAllStandardOutput()) 
    .split("[CHAPTER]",Qt::SkipEmptyParts);

  std::cout<<"\n\n<-------------------------| Number Of Chapters Detected: "<<Chapters.size()<<" |------------------------->\n\n";

  for(int i=0;i<Chapters.size();i++){
    QStringList chapterStreams = QString(Chapters[i]).split("\n");
    ChapterObject newChapter; 
    newChapter.title = "Unknown";
    for(int j=0;j<chapterStreams.size();j++){
      if(chapterStreams[j] != "[/CHAPTER]"){
        QStringList SplitedLine = QString(chapterStreams[j]).split("=");
        if(SplitedLine[0] == "id") newChapter.id = SplitedLine[1];
        else if(SplitedLine[0] == "TAG:title") newChapter.title = SplitedLine[1];
        else if(SplitedLine[0] == "start_time") newChapter.startTime = SplitedLine[1].toFloat();
        else if(SplitedLine[0] == "end_time") newChapter.endTime = SplitedLine[1].toFloat();
      }
    }
    ChaptersVectors.push_back(newChapter);
  }
  videoslider->setChaptersMarks(ChaptersVectors, showChaptersIndicators);
}

void MainWindow::ExtractingBuiltInSubs(QString currenturl) {
  QProcess lookForSubs;
  lookForSubs.start("/usr/bin/ffprobe", {
    "-v", "error",
    "-select_streams", "s",
    "-show_entries", "stream=index",
    "-of", "csv=p=0",
    currenturl
  });
  lookForSubs.waitForFinished(-1);

  QStringList subStreams = QString(lookForSubs.readAllStandardOutput())
    .split("\n", Qt::SkipEmptyParts);

  std::cout<<"\n\n<-------------------------| Number Of Subs Detected: "<<subStreams.size()<<" |------------------------->\n\n";

  for(int i=0; i < subStreams.size(); i++) {
    std::filesystem::path currentVideoPath(currenturl.toStdString());
    std::string currentVideoPathWithoutExtension = currentVideoPath.parent_path().string() +"/"+currentVideoPath.stem().string();

    QString fileSubPath = QString("%1_Sub%2.srt").arg(QString::fromStdString(currentVideoPathWithoutExtension)).arg(i);
    QString subId = QString("0:s:%1").arg(i);

    QProcess* ffmpegProcess = new QProcess(this);
    connect(ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), ffmpegProcess, &QProcess::deleteLater); // delete the process when finished
    ffmpegProcess->start("/usr/bin/ffmpeg", {"-y","-i", currenturl, "-map", subId, fileSubPath});

    subsInVideo.push_back(fileSubPath);

    if(i == 0){
      currentLoadedSubPath = fileSubPath;
      // when done extracting the first subtitle in the file parse the subs
      connect(ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), ffmpegProcess,[this](){
        SubFileParsing(currentLoadedSubPath.toStdString());
        // change the text of the QAction 
        QAction * ToggleSubs = TopBarButtonsObjectList[TOGGLE_SUB];
        ToggleSubs->setText("Remove Subtitles");
      });
    }

    std::cout <<"\n\n<-------------------------| Extract subtitle to: " << fileSubPath.toStdString() <<" |------------------------->\n\n";
  }
}

void MainWindow::mediaplayer(QString url) {
  video->setSize(view->size());
  QAction * ToggleSubs = TopBarButtonsObjectList[TOGGLE_SUB];
  ToggleSubs->setText("Add Subtitles");
  subsInVideo.clear();
  currentLoadedSubPath = "";

  // if there is no video to play a black image will play (blackscreen)
  if (videoindex > playlist.size() || url == "blackscreen") {
    player->setSource({});
    currenttimer->setText("--:--:--");
    totaltimer->setText("--:--:--");
    currenturl="";
    volumeslider->setRange(0, 1000);
    volumeslider->setSliderPosition(1000);
    paused = true;
    updateButtonsIcon();
    QPushButton *SkipButton = ButtonsObjectList[CONTINUEFROMLASTPOS_BUTTON];
    SkipButton->hide();

    return ;

  } else if (url == "play a list") {  // if pass "play a list" as an argunent a video from the playlist will play
    currenturl = playlist[videoindex].toString();

  } else {  // if we pass a url, a video with the url will play and the playlist will be cleared
    currenturl = url;

  }

  //getting the path of the video playing as std::string
  std::filesystem::path currentPath (currenturl.toStdString());

  // getting the title of the video that is currently playing for later uses
  current_video_title = currentPath.stem().string();

  //get the current path of directory that the video is playing in
  currentworkdirectory = currentPath.parent_path().string();

  ExtranctingChapterData(currenturl);
  ExtractingBuiltInSubs(currenturl);
  LoadingInDirectorySubtitles(currenturl);

  // mediaplayer setup (sound and video widget)
  player->setSource(QUrl(currenturl));
  player->setVideoOutput(video);
  player->setAudioOutput(audio);
  audio->setVolume(1);
  volumeslider->setRange(0, 1000);
  volumeslider->setSliderPosition(1000);
  video->show();
  player->play();
  paused = false;
  //resize some ui elements based on the media opened
  resizelements();

  // displaying the title for a brief of time
  int xposition = view->size().width() / 2;
  int yposition = view->size().height() - submarginbottom;

  showingthings(current_video_title, xposition, yposition, 2000);
  //load the last saved position if it's availble
  getlastsavedposition();

  //save the position of the video what was playing before
  savevideoposition();

}

// topbarlayout buttons logic
void MainWindow::topbarlayoutclick(int buttonindex) {
  QString url;
  switch (buttonindex) {
    // if the user choose to open a file
    case Open_file: {
      QString displaydir;
      if (currentworkdirectory.size())
        displaydir = QString::fromStdString(currentworkdirectory);
      else{
        displaydir = homedir;
      }
      std::stringstream SupportecFormatsString; 
      SupportecFormatsString << "Video/Audio files ("; 
      for(size_t i=0;i<supportedMediaFormats.size();i++) SupportecFormatsString << "*" << supportedMediaFormats[i] <<" ";
      SupportecFormatsString << ")";

      url = QFileDialog::getOpenFileName(this, tr("Select Video File"), displaydir, tr(SupportecFormatsString.str().c_str()));

      if (!url.isEmpty()) {
        mediaplayer(url);
        playertype = "vid";
      }
      playlist.clear();  // clearing the playlist
      break;
    }

    // if the user choose to open a directory (playlist)
    case Open_folder: {
      QString displaydir;
      if (currentworkdirectory.size())
        displaydir = QString::fromStdString(currentworkdirectory);
      else{
        displaydir = homedir;
      }

      url = QFileDialog::getExistingDirectory(this, tr("Setect Playlist Directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks), displaydir);
      if (!url.isEmpty()) {
        playlist.clear();  // clearing the playlist
        // saving all the urls in a list
        for (auto i : std::filesystem::directory_iterator(url.toStdString())) {
          if(std::find(supportedMediaFormats.begin(), supportedMediaFormats.end(),i.path().extension()) != supportedMediaFormats.end()){
            playlist.push_back(QUrl(QString::fromStdString(i.path().string())));
          }
        }
        if (playlist.size()) {
          playertype = "playlist";
          mediaplayer("play a list");
        }
      }
      break;
    }

    // if the user choose to open a media file
    case Open_media: {
      // launching the constructor for url window input
      UrlWindow x(nullptr, STYLESDIRECTORY);
      x.exec();
      // getting the url inputed by the user
      url = x.url;
      // checking if the user set a url or not
      if (!url.isEmpty()) {
        playlist.clear();
        playertype = "vid";
        mediaplayer(url);
      }
      break;
    }

    // if the user choose to quit the app
    case Quit: {
      QApplication::quit();
      break;
    }

    // jump backward (player)
    case JUMP_BACKWARD: {
      changingposition(player->position() - 5000);
      break;
    }

    // jump forward (player)
    case JUMP_FORWARD: {
      changingposition(player->position() + 5000);
      break;
    }

    // jump to a specific time
    case JUMP_TO_TIME: {
      JumpTime x(nullptr, STYLESDIRECTORY);
      x.exec();
      if (x.targettime >= 0) {
        changingposition(x.targettime * 1000);
      }
      break;
    }
    // jump to next chapter
    case JUMP_TO_NEXT_CHAP:{
      moveToNextChapter();
      break;
    }
    // jump to previous chapter
    case JUMP_TO_PREV_CHAP:{
      moveToPrevChapter();
      break;
    }

    // loop on segment of the video
    case TOGGLE_LOOPSEGMENT: {
      QAction * toggleLoopAction = TopBarButtonsObjectList[TOGGLE_LOOPSEGMENT];
      if(!currenturl.isEmpty()){
        if(!repeatfromposition){
          SRepeatWindow win(nullptr, STYLESDIRECTORY);
          win.exec();
          if (win.startingpoint >= 0 && win.finishingpoint >= 0 && win.finishingpoint != win.startingpoint) {
            repeatfromposition = true;
            startingpoint = win.startingpoint;
            finishpoint = win.finishingpoint;
            changingposition(startingpoint * 1000);
            toggleLoopAction->setText("Exit The Loop");

          }
        }else{
          repeatfromposition = false;
          toggleLoopAction->setText("Start Segment Loop");
        }
      }
      break;
    }

    // setting the audio to full volume
    case FULL_VOLUME: {
      slidermanagement(1);
      break;
    }

    // setting the audio to mute
    case MUTE: {
      slidermanagement(0);
      break;
    }

    // make the videofill the view
    case TOGGLE_ASPRadio: {
      QAction * toggleFillAction = TopBarButtonsObjectList[TOGGLE_ASPRadio];
      if(CurrentAspectMode == Qt::KeepAspectRatio){
        CurrentAspectMode = Qt::IgnoreAspectRatio; 
        toggleFillAction->setText("Lock Aspect Ratio");
      }else{
        CurrentAspectMode = Qt::KeepAspectRatio;
        toggleFillAction->setText("Stretch to Fill");
      }
      video->setAspectRatioMode(CurrentAspectMode);
      break;
    }

    // if the user choose to open a sub file
    case TOGGLE_SUB: {
      QAction * ToggleSubs = TopBarButtonsObjectList[TOGGLE_SUB];
      QString displaydir;
      if(subslist.size() == 0){
        if (currentworkdirectory.size())
          displaydir = QString::fromStdString(currentworkdirectory);
        else{
          displaydir = homedir;
        }

        std::stringstream SupportedSubtitlesFormatstring; 
        SupportedSubtitlesFormatstring << "Srt files ("; 

        for(size_t i=0;i<supportedSubtitlesFormats.size();i++) SupportedSubtitlesFormatstring << "*" << supportedSubtitlesFormats[i] <<" ";
        SupportedSubtitlesFormatstring << ")";

        currentLoadedSubPath = QFileDialog::getOpenFileName(this, tr("Select Subtitle file"), displaydir, tr(SupportedSubtitlesFormatstring.str().c_str()));
        if (!currentLoadedSubPath.isEmpty()) {
          SubFileParsing(currentLoadedSubPath.toStdString());
          std::cout<<"Subtitles were Loaded: "<<currentLoadedSubPath.toStdString()<<"\n";
          ToggleSubs->setText("Remove Subtitles");
        }
      }else{
        for (SubObject* ptr:subslist){
          delete ptr;
        }
        sublabel->setOpacity(0);
        subslist.clear();
        currentLoadedSubPath = "";
        ToggleSubs->setText("Add Subtitles");
      }
      break;
    } 
    case LOADSUBTITLES:{
      subWindow subWin(nullptr,STYLESDIRECTORY,ICONSDIRECTORY.toStdString(),subsInVideo,currentLoadedSubPath);
      subWin.exec();
      if(!subWin.clickedSubPath.isEmpty()){
        currentLoadedSubPath = subWin.clickedSubPath;
        sublabel->setOpacity(0);
        subslist.clear();
        SubFileParsing(currentLoadedSubPath.toStdString());
        std::cout<<"Subtitles were Loaded: "<<currentLoadedSubPath.toStdString()<<"\n";
        QAction * ToggleSubs = TopBarButtonsObjectList[TOGGLE_SUB];
        ToggleSubs->setText("Remove Subtitles");
      }
      break;
    }

    case TOGGLE_SUBDISPLAY: {
      QAction * ToggleSubsDisplay = TopBarButtonsObjectList[TOGGLE_SUBDISPLAY];
      toggleSubtitles();
      if(ShowSubs){
        ToggleSubsDisplay->setText("Hide Subtitles");
      }else{
        ToggleSubsDisplay->setText("Display Subtitles");
      }
      break;
    }
    case TOGGLE_CHAPTERSINDICATORS: {
      QAction * toggleChapters = TopBarButtonsObjectList[TOGGLE_CHAPTERSINDICATORS];
      toggleChaptersIndicators();
      if(showChaptersIndicators){
        toggleChapters->setText("Hide Chapters Indicators");
      }else{
        toggleChapters->setText("Display Chapters Indicators");
      }
      break;
    }
    // add delay to the subtitles time
    case ADDDELAY: {
      if (subslist.size()) {
        for (size_t i = 0; i < subslist.size(); i++) {
          subslist[i]->starttime += 0.1;
          subslist[i]->endtime += 0.1;
        }
        subdelay += 0.1;
        if (-0.1 < subdelay && subdelay < 0.1) {
          subdelay = 0;
        }
        // add animation so the user can see that the delay has been changed
        std::string text = "Subtitles Delay:" + std::to_string((int)(subdelay * 1000)) + " ms";
        int xposition = view->size().width();
        int yposition = view->size().height();
        showingthings(text, xposition / 2, yposition / 2, 1000);
      }
      break;
    }

    // reducing delay to the subtitles time
    case REDUCEDELAY: {
      if (subslist.size()) {
        for (size_t i = 0; i < subslist.size(); i++) {
          if (subslist[0]->starttime > 0) {
            subslist[i]->starttime -= 0.1;
            subslist[i]->endtime -= 0.1;

          }
        }
        subdelay -= 0.1;

        if (-0.1 < subdelay && subdelay < 0.1) {
          subdelay = 0;
        }
        // add animation so the user can see that the delay has been changed
        std::string text = "Subtitles Delay:" + std::to_string((int)(subdelay * 1000)) + " ms";
        int xposition = view->size().width();
        int yposition = view->size().height();
        showingthings(text, xposition / 2, yposition / 2, 1000);
      }
      break;
    }

    // if the user choose to custumize the sub
    case SUBSETTINGS: {
      SubConfig win;
      win.gui(CONFIGSDIRECTORY, FONTSDIRECTORY, STYLESDIRECTORY);
      win.exec();
      htmlstyle = win.makehtml(CONFIGSDIRECTORY);
      subpadding = win.padding;
      submarginbottom = win.marginbottom;
      break;
    }

    // showing the title of the video
    case TITLE: {
      if (current_video_title.size()) {
        int xposition = view->size().width() / 2;
        int yposition = view->size().height() - submarginbottom;
        showingthings(current_video_title, xposition, yposition, 3000);
      }
      break;
    }

    //changing the theme of the app
    case THEME:{
      ChangeThemeWindow win(nullptr,CONFIGSDIRECTORY,projectdir+"cache/styles",STYLESDIRECTORY);
      win.exec();
      int xposition = view->size().width() / 2;
      int yposition = view->size().height()/2 ;
      if(win.changetotheme!=""){
        showingthings("You Need to reset the Application to apply the new Theme",xposition,yposition,4000);
      }
      break;
    }

    // showing the shortcuts instructions
    case SHORTCUTS: {
      ShortcutsInst win(nullptr,STYLESDIRECTORY,CONFIGSDIRECTORY);
      win.exec();
      break;
    }
  }

  ;
}

// controlbuttonslayout buttons logic
void MainWindow::controlbuttonslayoutclick(int buttonindex) {
  switch (buttonindex) {
    // if the pause button is clicked
    case PAUSE_BUTTON: {
      if (!paused) player->pause();
      else player->play();
      paused = !paused ;
      updateButtonsIcon();
      break;
    }

    // if the prevous video button is clicked
    case BACK_BUTTON: {
      if (playertype == "playlist" && videoindex > 0) {
        videoindex--;
        mediaplayer("play a list");
      }
      break;
    }

    // if the stop button is clicked
    case STOP_BUTTON: {
      savevideoposition();
      mediaplayer("blackscreen");
      playlist.clear();
      playertype = "video";
      break;
    }

    // if the next video button is clicked
    case NEXT_BUTTON: {
      if (playertype == "playlist") {
        // we set the player to the end of the video so it trigger the logic in the lines (326,...354)
        player->setPosition(player->duration());
      }
      break;
    }
    // if fullscreen button is clicked
    case FULLSCREEN_BUTTON: {
      FullScreen();
      break;
    }

    case PLAYLIST_BUTTON: {
      PlaylistManager win(nullptr, STYLESDIRECTORY, ICONSDIRECTORY.toStdString(), playlist, currenturl);
      win.exec();
      if (win.new_video_index != (int)videoindex && win.new_video_index != -1) {
        videoindex = win.new_video_index;
        mediaplayer("play a list");
      }
      break;
    }

    // if reloading behavior is clicked
    case REPETITION_BUTTON: {
      if (rep == PlaylistRepeat) rep = VideoRepeat;
      else if (rep == VideoRepeat) rep = Shuffle;
      else if (rep == Shuffle) rep = PlaylistRepeat;
      updateButtonsIcon("repetition");

      break;
    }

    // Continue from last position you stoped
    case CONTINUEFROMLASTPOS_BUTTON:{
      changingposition(lastsavedposition);
      QPushButton *skipbutton = ButtonsObjectList[CONTINUEFROMLASTPOS_BUTTON];
      skipbutton->hide();
      break;
    }
    // mute and unmute
    case BVolumeControl: {
      if (audio->volume()) {
        oldvolume = audio->volume();
        slidermanagement(0);
      } else {
        slidermanagement(oldvolume);
      }
      break;
    }
  }
  ;
}

// keyboard event catching function
void MainWindow::setupShortCuts(){
    setFocusPolicy(Qt::StrongFocus);

    // --- Playback controls ---
    auto pause = new QShortcut(QKeySequence(Qt::Key_Space), this);
    pause->setContext(Qt::ApplicationShortcut);
    connect(pause, &QShortcut::activated, this, [this]{
        controlbuttonslayoutclick(PAUSE_BUTTON);
    });

    auto seekFwd = new QShortcut(QKeySequence(Qt::Key_Right), this);
    seekFwd->setContext(Qt::ApplicationShortcut);
    connect(seekFwd, &QShortcut::activated, this, [this]{
        changingposition(player->position() + 5000);
    });

    auto seekBack = new QShortcut(QKeySequence(Qt::Key_Left), this);
    seekBack->setContext(Qt::ApplicationShortcut);
    connect(seekBack, &QShortcut::activated, this, [this]{
        changingposition(player->position() - 5000);
    });

    // --- Volume controls ---
    auto volUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    volUp->setContext(Qt::ApplicationShortcut);
    connect(volUp, &QShortcut::activated, this, [this]{
        slidermanagement(audio->volume() + 0.1f);
        volumeslider->setValue(volumeslider->value() + 1);
    });

    auto volDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    volDown->setContext(Qt::ApplicationShortcut);
    connect(volDown, &QShortcut::activated, this, [this]{
        slidermanagement(audio->volume() - 0.1f);
        volumeslider->setValue(volumeslider->value() - 1);
    });

    // --- Fullscreen ---
    auto toggleFS = new QShortcut(QKeySequence(Qt::Key_F), this);
    toggleFS->setContext(Qt::ApplicationShortcut);
    connect(toggleFS, &QShortcut::activated, this, [this]{ FullScreen(); });

    auto escFS = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    escFS->setContext(Qt::ApplicationShortcut);
    connect(escFS, &QShortcut::activated, this, [this]{
        fullscreened = true;
        FullScreen();
    });

    // --- Subtitles controls ---
    auto toggleSubs = new QShortcut(QKeySequence(Qt::Key_V),this);
    toggleSubs->setContext(Qt::ApplicationShortcut);
    connect(toggleSubs, &QShortcut::activated, this, [this]{
      toggleSubtitles();
    });
    
    // --- Chapters controls ---
    auto toggleChaptersIndHandler = new QShortcut(QKeySequence(Qt::Key_C),this);
    toggleChaptersIndHandler->setContext(Qt::ApplicationShortcut);
    connect(toggleChaptersIndHandler,&QShortcut::activated,this, [this]{
      toggleChaptersIndicators();
    });

    auto goToNextChapter = new QShortcut(QKeySequence(Qt::Key_N),this);
    goToNextChapter->setContext(Qt::ApplicationShortcut);
    connect(goToNextChapter, &QShortcut::activated, this, [this]{
      moveToNextChapter();
    });

    auto goToPrevChapter = new QShortcut(QKeySequence(Qt::Key_B),this);
    goToPrevChapter->setContext(Qt::ApplicationShortcut);
    connect(goToPrevChapter, &QShortcut::activated, this, [this]{
      moveToPrevChapter();
    });

    // --- Other controls ---
    auto volPanel = new QShortcut(QKeySequence(Qt::Key_M), this);
    volPanel->setContext(Qt::ApplicationShortcut);
    connect(volPanel, &QShortcut::activated, this, [this]{
      controlbuttonslayoutclick(BVolumeControl);
    });

    auto reduceDelay = new QShortcut(QKeySequence(Qt::Key_G), this);
    reduceDelay->setContext(Qt::ApplicationShortcut);
    connect(reduceDelay, &QShortcut::activated, this, [this]{
      topbarlayoutclick(REDUCEDELAY);
    });

    auto addDelay = new QShortcut(QKeySequence(Qt::Key_H), this);
    addDelay->setContext(Qt::ApplicationShortcut);
    connect(addDelay, &QShortcut::activated, this, [this]{
      topbarlayoutclick(ADDDELAY);
    });
}

void MainWindow::toggleSubtitles(){
  ShowSubs =! ShowSubs;
  if(!ShowSubs) sublabel->setHtml("");
  int xposition = view->size().width() / 2;
  int yposition = view->size().height() /2;
  std::string displayMessage = ShowSubs ? "Subtitles On" : "Subtitles Off";
  showingthings(displayMessage, xposition, yposition, 2000);
}

void MainWindow::toggleChaptersIndicators(){
  showChaptersIndicators =!showChaptersIndicators;
  int xposition = view->size().width() / 2;
  int yposition = view->size().height() /2;
  std::string displayMessage = showChaptersIndicators ? "Chapters Indicators On" : "Chapters Indicators Off";
  showingthings(displayMessage, xposition, yposition, 2000);
  videoslider->setChaptersMarks(ChaptersVectors, showChaptersIndicators);
}

std::vector<int> calculatingTextDimentions(QString text){
  QGraphicsTextItem *dummyText = new QGraphicsTextItem;
  dummyText->setHtml(text);

  int textwidth = dummyText->boundingRect().width();
  int textheight = dummyText->boundingRect().height();
  return {textwidth, textheight};
}

void MainWindow::moveToNextChapter(){
  for(size_t i=0;i<ChaptersVectors.size();i++){
    if(player->position() >= ChaptersVectors[i].startTime*1000 && player->position() <= ChaptersVectors[i].endTime*1000){
      ChapterObject newChapter;
      if(i == ChaptersVectors.size()-1) newChapter = ChaptersVectors[i];
      else newChapter = ChaptersVectors[i+1];
      changingposition(newChapter.startTime*1000+1);
      QString titleToShow = htmlstyle + newChapter.title + "</div>";
      int video_start_x = view->pos().rx();
      int video_start_y = view->pos().ry();
      showingthings(titleToShow.toStdString(),video_start_x+calculatingTextDimentions(titleToShow)[0]/2+10,video_start_y+calculatingTextDimentions(newChapter.title)[1]+15, 2000); 
      break;
    }
  }
}

void MainWindow::moveToPrevChapter(){
  for(size_t i=1;i<ChaptersVectors.size();i++){
    if(player->position() >= ChaptersVectors[i].startTime*1000 && player->position() <= ChaptersVectors[i].endTime*1000){
      ChapterObject newChapter = ChaptersVectors[i-1];
      changingposition(newChapter.startTime*1000+1);
      QString titleToShow = htmlstyle + newChapter.title + "</div>";
      int video_start_x = view->pos().rx();
      int video_start_y = view->pos().ry();
      showingthings(titleToShow.toStdString(),video_start_x+calculatingTextDimentions(titleToShow)[0]/2+10,video_start_y+calculatingTextDimentions(newChapter.title)[1]+15, 2000); 
 
      break;
    }
  }
} 


// slider and player relationship
// setting the range of the slider basing on the player
void MainWindow::setsliderrange(qint64 position) { videoslider->setRange(0, position); }

// setting the app elements in there relation with the player
void MainWindow::playertimeline(qint64 position) {

  videoslider->setValue(position);//syncing the slider with the player position

  // setting the current timer basing on the player position
  if (position != player->duration()) updateTimer();

  // if the video is finished
  else if (position == player->duration()) {
    // if the reloading button is in the "reload full playlist" mode
    if (rep == PlaylistRepeat) {
      if (playlist.size()){
        if (videoindex == playlist.size() - 1) {
          videoindex = 0;
        } else {
          videoindex++;
        }
        mediaplayer("play a list");
      }else mediaplayer("blackscreen");
    }

    // if the reloading button is in the "reload one video" mode
    else if (rep == VideoRepeat) changingposition(0);

    // if the reloading button is in the "random video" mode
    else if (rep == Shuffle) {
      videoindex = rand() % playlist.size();
      mediaplayer("play a list");
    }
    videoslider->setValue(0);
  }

  //if the user created a loop
  if (repeatfromposition) {
    if (position >= finishpoint * 1000) {
      changingposition(startingpoint * 1000);
    }
  }

  // syncing subtitles to the player position
  for(size_t i=0;i<subslist.size();i++){
    // checking if the player position is between the 2 times
    if (ShowSubs && (subslist[i]->starttime * 1000) - 200 <= position && position <= subslist[i]->endtime * 1000) {
      // getting the size of the view and the sub
      sublabel->setOpacity(1);
      // adding 10ms delay to the sub apearence until the sub is fully randered to hide the animation
      if (position <= subslist[i]->starttime * 1000 + 200 ) {
        sublabel->setOpacity(0);
      }

      resizelements("sub");

      // if the media is in the targeted position we merge the html style with the subtitle and pass it as html script
      sublabel->setHtml(htmlstyle + QString::fromStdString(subslist[i]->textContent) + "</td></tr></table>");
      break;
    }else if (i == subslist.size() - 2) {
      // if the media is not in a target position we pass an empty string
      sublabel->setHtml("");
    }
  }
  updateButtonsIcon();
}

void MainWindow::updateTimer(){

  if(currenturl == "") return;

  int position = player->position();

  // setting the current timer basing on the player position
  int hour = position / (1000 * 60 * 60);
  int min = (position / 1000 - hour * 60 * 60) / 60;
  int second = position  / 1000 - min * 60 - hour * 60 * 60;
  std::ostringstream osshour, ossmin, osssecond;
  osshour << std::setfill('0') << std::setw(2) << hour;
  ossmin << std::setfill('0') << std::setw(2) << min;
  osssecond << std::setfill('0') << std::setw(2) << second;
  currenttimer->setText(QString::fromStdString(osshour.str()) + ":"+
    QString::fromStdString(ossmin.str()) +":" +
    QString::fromStdString(osssecond.str()));

  // setting the total timer basing on the player duration
  int thour = player->duration() / (1000 * 60 * 60);
  int tmin = (player->duration() / 1000 - thour * 60 * 60) / 60;
  int tsecond = player->duration() / 1000 - tmin * 60 - thour * 60 * 60;
  std::ostringstream tosshour, tossmin, tosssecond;
  tosshour << std::setfill('0') << std::setw(2) << thour;
  tossmin << std::setfill('0') << std::setw(2) << tmin;
  tosssecond << std::setfill('0') << std::setw(2) << tsecond;
  totaltimer->setText(QString::fromStdString(tosshour.str()) + ":" +
    QString::fromStdString(tossmin.str()) +
    ":" + QString::fromStdString(tosssecond.str()));

}


/*
  this function is to work around a bug that break the audio when changing position,
  by deleting the audio output widget and create a new one
  every time we change position.
*/

void MainWindow::changingposition(int newpos) {
  float oldvol = audio->volume();
  player->pause();
  player->setAudioOutput(nullptr);
  delete audio;
  audio = new QAudioOutput();
  player->setPosition(newpos);
  player->setAudioOutput(audio);
  audio->setVolume(oldvol);
  if (!paused){
    player->play();
    paused = false;
  }
}


// managing the interactions with the volume slider
void MainWindow::slidermanagement(qreal position) {
  updateButtonsIcon("volume");
  volumeslider->setSliderPosition(static_cast<int>(position * 1000));
}

bool lineIsEmpty(const std::string& str){
  return str.empty() || std::all_of(str.begin(), str.end(), [](unsigned char c){
      return std::isspace(c);
  });
}

bool stringIsInteger(std::string text){
  if(text.empty()) return false;
  for(size_t i=0;i<text.size();i++){
    if(((int)text[i] < '0' || (int)text[i] > '9') && (int)text[text.size()] != ' ' ) return false;
  }
  return true;
}

bool lineContainsTime(std::string text){
  return (text.find("-->") != std::string::npos);
}

void MainWindow::assSubFileParsing(std::string subpath){
  std::ifstream file(subpath);
  std::string line;
  subslist.clear();
  int startTimeIndex=1; 
  int endTimeIndex=2;
  int textIndex=9;

  while(getline(file,line)){
    if(line.find("Format:") != std::string::npos){
      line.erase(std::remove(line.begin(),line.end(),' '), line.end());
      line.erase(std::remove(line.begin(),line.end(),'\r'), line.end());
      QStringList splitedLine = QString::fromStdString(line).split(",");
      for(int i=0;i<splitedLine.size();i++){
        if(splitedLine[i] == "Start") startTimeIndex = i;
        if(splitedLine[i] == "End") endTimeIndex = i;
        if(splitedLine[i] == "Text") textIndex = i;
      }
    }
    if(line.find("Dialogue:") != std::string::npos){
      QStringList splitedDialogLine = QString::fromStdString(line).split(",");

      subobject = new SubObject;

      char step;     
      std::istringstream ss1(splitedDialogLine[startTimeIndex].toStdString());
      int h1, min1, sec1, milsec1;
      ss1>>h1>>step>>min1>>step>>sec1>>step>>milsec1;
      double firsttime = h1*60*60 + min1*60 + sec1 + milsec1*0.001;

      std::istringstream ss2(splitedDialogLine[endTimeIndex].toStdString());
      int h2, min2, sec2, milsec2;
      ss2>>h2>>step>>min2>>step>>sec2>>step>>milsec2;
      double secondtime = h2*60*60 + min2*60 + sec2 + milsec2*0.001;
      
      std::string subText = splitedDialogLine[textIndex].toStdString();

      size_t pos = 0;
      while ((pos = subText.find("\\N", pos)) != std::string::npos) {
        subText.replace(pos, 2, "<br/>");
        pos += 1;
      }
      while ((pos = subText.find("\\n", pos)) != std::string::npos) {
        subText.replace(pos, 2, "<br/>");
        pos += 1;
      }


      subobject->starttime = firsttime;
      subobject->endtime = secondtime;
      subobject->textContent = subText;
      subslist.push_back(subobject);
    }
  }
}

void MainWindow::strSubFileParsing(std::string subpath){
  std::ifstream file(subpath);
  std::string line;
  std::string nextline;
  std::string fulltext = "";

  // looping the lines in the file
  while (getline(file, line)) {
    if(lineContainsTime(line)){
      std::istringstream ss(line);
      int h1, min1, sec1, milsec1, h2, min2, sec2, milsec2;
      std::string arrow;
      char step;
      ss>>h1>>step>>min1>>step>>sec1>>step>>milsec1;
      ss>>std::ws>>arrow>>std::ws;
      ss>>h2>>step>>min2>>step>>sec2>>step>>milsec2;

      // calculating the starting time from the string line (hour, minutes, seconds)
      double firsttime = h1*60*60 + min1*60 + sec1 + milsec1*0.001;

      // calculating the ending time from the string line (hour, minutes, seconds)
      double secondtime = h2*60*60 + min2*60 + sec2 + milsec2*0.001;

      subobject = new SubObject;
      subobject->starttime = firsttime;
      subobject->endtime = secondtime;

      while(getline(file,nextline)){
        nextline.erase(std::remove(nextline.begin(), nextline.end(), '\r'), nextline.end());
        if(stringIsInteger(nextline)){
          if(!fulltext.empty()){
            int brPosition = fulltext.rfind("<br/>");
            subobject->textContent = fulltext.substr(0,brPosition);
            subslist.push_back(subobject);
            fulltext = "";
          }
          break;
        }else{
          fulltext += nextline + "<br/>";
        }
      }
    }
  }
}

// scraping the subtitles from the sub file
void MainWindow::SubFileParsing(std::string subpath) {
  for (SubObject* ptr:subslist) delete ptr;
  subslist.clear();
  if(std::filesystem::path(subpath).extension().string() == ".ass") assSubFileParsing(subpath);
  else strSubFileParsing(subpath);
}

//function to resize ui elements
void MainWindow::resizelements(std::string elementtorezise, int animationTime ){
  int VIEWWIDTH = view->size().width();
  int VIEWHEIGHT = view->size().height();
  int SUBWIDTH = sublabel->boundingRect().width();
  int SUBHEIGHT = sublabel->boundingRect().height();

  //if the element being resized is the video layout
  if(elementtorezise=="video" || elementtorezise=="all"){
    video->setSize(QSize(VIEWWIDTH + 2, VIEWHEIGHT + 2));
    scene->setSceneRect(0, 0, VIEWWIDTH - 1, VIEWHEIGHT - 1);
    view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  }
  //if the element being resized is the floating pannel
  if(elementtorezise=="floatingPannel" || elementtorezise=="all"){
    int floatingPannel_width = floatingControlPannelProxy->boundingRect().width();
    int floatingPannel_height = floatingControlPannelProxy->boundingRect().height();
    QPoint hiding_position = QPoint((VIEWWIDTH - floatingPannel_width) / 2, (VIEWHEIGHT + floatingPannel_height));
    moveSomethingToPos(floatingControlPannelProxy,  hiding_position, animationTime);
  }
  //if the element being resized is the sub Label
  if(elementtorezise=="sub" || elementtorezise=="all"){
    sublabel->setPos((VIEWWIDTH - SUBWIDTH) / 2, (VIEWHEIGHT - SUBHEIGHT / 2) - submarginbottom);
  }
  showingthings("",0,0,0);
}

//resizing window logic
void MainWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  resizelements();
}

//function to toggle the fullscreen
void MainWindow::FullScreen(){
  if (fullscreened){
    this->showMaximized();
    topbarlayoutvisibility("show");// calling function to show topbar
    mainlayout->setContentsMargins(10, 10, 10, 10);
    createBottomLayout();//recreating bottom layout
    mainlayout->addLayout(controlbuttonslayout);// adding the bottom layout into the mainlayout
  } else {
    this->showFullScreen();
    topbarlayoutvisibility("hide");// calling function to hide topbar
    mainlayout->setContentsMargins(0, 0, 0, 0);
    video->setSize(this->size());
    view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    createBottomLayout();//recreating bottom layout
    floatingControlPannelContainerLayout->addLayout(controlbuttonslayout);// adding the bottom layout into the floating pannel
  }
  fullscreened = !fullscreened;
  // resetting the volume slider
  float currentVolume = audio->volume();
  volumeslider->setRange(0, 1000);
  volumeslider->setValue(currentVolume*1000);
  updateButtonsIcon();
  updateTimer();
}

//function that display text on the top of the video with fading animation
void MainWindow::showingthings(std::string texttoshow, int xposition, int yposition, int animationduration) {
  if(toshowtext != nullptr && toshowtext->scene() == scene ){
    scene->removeItem(toshowtext);
  }
  if(animationduration == 0) return;

  toshowtext = new QGraphicsTextItem;
  // show the state of the delay using the same font config of the subtitles
  toshowtext->setHtml(htmlstyle + QString::fromStdString(texttoshow) + "</div>");

  // calculating the position that the text should go to
  int textwidth = toshowtext->boundingRect().width();
  int textheight = toshowtext->boundingRect().height();
  toshowtext->setPos(xposition - textwidth / 2, yposition - textheight / 2);
  scene->addItem(toshowtext);

  // making an effect for the apearence and deapearence of the text
  QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(toshowtext);
  toshowtext->setGraphicsEffect(opacityEffect);
  // configuring the animation of the text
  QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity");
  animation->setDuration(animationduration);
  animation->setStartValue(1.0);
  animation->setEndValue(0.0);
  animation->start(QPropertyAnimation::DeleteWhenStopped);  // start the animation
}


// toplayout visibility control function
void MainWindow::topbarlayoutvisibility(std::string status){
  for (int i = 0; i < topbarlayout->count(); i++) {
    QWidget *searchtoolbutton = topbarlayout->itemAt(i)->widget();
      if (searchtoolbutton) {
        if (status=="show") {
          searchtoolbutton->show();
        }else if(status=="hide") {
          searchtoolbutton->hide();

        }
      }
  }
}

//this function update buttons icons
void MainWindow::updateButtonsIcon(std::string button_name){

  //update play/pause button icon
  if(button_name == "play/pause" || button_name == "all"){
    QPushButton *Pause_button = ButtonsObjectList[PAUSE_BUTTON];
    if(!paused || currenturl == "") Pause_button->setIcon(QPixmap(ICONSDIRECTORY + "BPause.png"));
    else Pause_button->setIcon(QPixmap(ICONSDIRECTORY + "BPlay.png"));
  }

  //update volume button icons
  if(button_name == "volume" || button_name == "all"){
    QPushButton *VolumeControlButton = ButtonsObjectList[BVolumeControl];
    float currentvolume = audio->volume() * 1000;
    if (currentvolume == 0) {
      VolumeControlButton->setIcon(QPixmap(ICONSDIRECTORY + "BMute.png"));
      volumeslider->setStyleSheet("QSlider#volumeslider::handle{background:#1e1e1e;}");

    } else if (currentvolume <= 333 && currentvolume > 0) {
      VolumeControlButton->setIcon(QPixmap(ICONSDIRECTORY + "BVolumeLow.png"));
      volumeslider->setStyleSheet("QSlider#volumeslider::handle{background:#484949;}");

    } else if (currentvolume >= 333 && currentvolume <= 666) {
      VolumeControlButton->setIcon(QPixmap(ICONSDIRECTORY + "BVolumeMid.png"));

    } else if (currentvolume >= 666) {
      VolumeControlButton->setIcon(QPixmap(ICONSDIRECTORY + "BVolumeControl.png"));
    }
  }

  //update repetition button icon
  if(button_name == "repetition" || button_name == "all"){
    QPushButton *Repeatition_button = ButtonsObjectList[REPETITION_BUTTON];
    if (rep == PlaylistRepeat) Repeatition_button->setIcon(QPixmap(ICONSDIRECTORY + "BRepeating.png"));
    else if (rep == VideoRepeat) Repeatition_button->setIcon(QPixmap(ICONSDIRECTORY + "BRepeatingone.png"));
    else if (rep == Shuffle) Repeatition_button->setIcon(QPixmap(ICONSDIRECTORY + "BSuffle.png"));
  }

}

//function to save the position of a video after closing it
void MainWindow::savevideoposition(){
  if(current_video_title.size() && player->position() > 5000){
    std::ofstream possavefile(CONFIGSDIRECTORY+"/.positionsave.csv",std::ios::app);
    std::string line = current_video_title+" ; "+QString::number(player->position()).toStdString()+'\n';
    possavefile<<line;
    possavefile.close();
  }
}

//load old position of a video if available
void MainWindow::getlastsavedposition(){
  if(current_video_title == "") return;
  lastsavedposition = 0;
  std::ifstream possavefile(CONFIGSDIRECTORY+"/.positionsave.csv");
  if (possavefile){
    std::string line;
    while(getline(possavefile,line)){
      if(line.find(current_video_title)!=std::string::npos){
        lastsavedposition = std::stoi(line.substr(line.rfind(";")+1,line.size()-1));
      }
    }

    if(lastsavedposition){
      QPushButton *skipbutton = ButtonsObjectList[CONTINUEFROMLASTPOS_BUTTON];
      skipbutton->show();
    }
  }
}

//double click detection
void MainWindow::mouseDoubleClickEvent(QMouseEvent * event){

  //getting the mouse position
  int mousePosition_x = event->pos().rx();
  int mousePosition_y = event->pos().ry();

  //getting the video layout position
  int video_start_x = view->pos().rx();
  int video_start_y = view->pos().ry();
  int video_end_x = view->size().width()+video_start_x;
  int video_end_y = view->size().height()+video_start_y;

  //making sure the position of the mouse when double clicking is inside the video layout
  bool ClickIsInsideVideoLayout = (mousePosition_x > video_start_x &&
                                mousePosition_y > video_start_y  &&
                                mousePosition_x < video_end_x &&
                                mousePosition_y < video_end_y);

  // getting the floating pannel position
  int floatingPannel_start_x = floatingControlPannelWidget->pos().rx();
  int floatingPannel_start_y = floatingControlPannelWidget->pos().ry();
  int floatingPannel_end_x = floatingControlPannelWidget->size().width() + floatingPannel_start_x;
  int floatingPannel_end_y = floatingControlPannelWidget->size().height() + floatingPannel_start_y;

  //making sure the position of the mouse when double clicking is ouside the floating pannel
  bool ClickNotOnfloatingPannel = ((mousePosition_x < floatingPannel_start_x ||
                                  mousePosition_y < floatingPannel_start_y ||
                                  mousePosition_x > floatingPannel_end_x ||
                                  mousePosition_y > floatingPannel_end_y));

  if(ClickIsInsideVideoLayout && ClickNotOnfloatingPannel){
    //if the user double clicked on the video layout a fullscreen function is gonna be called
    FullScreen();
  }
}

//function to move any graphic widget to any position smoothly
void moveSomethingToPos(QGraphicsWidget *widget, QPointF targetPos, int animationTime){
  QPropertyAnimation *animation = new QPropertyAnimation(widget, "pos");//defining the animation
  animation->setDuration(animationTime);//duration of the animation
  animation->setStartValue(widget->pos());
  animation->setEndValue(targetPos);
  animation->start(QPropertyAnimation::DeleteWhenStopped);  // start the animation
}

//function to detect mouse movement
bool MainWindow::eventFilter(QObject *obj, QEvent *event){
  static bool floatingPannelDisplayed = false; //this variable stores if the floating pannel is displayed already or not
  if (event->type() == QEvent::MouseMove && !floatingPannelDisplayed) {
    if(fullscreened){
      //getting the position of the floating pannel
      int viewheight = view->size().height();
      int floatingPannel_Xpos = floatingControlPannelProxy->pos().rx();
      int floatingPannel_height = floatingControlPannelProxy->boundingRect().height();
      //calculate the targetPosition that we want to move the pannel into, and then calling moveSomethingToPos function
      QPointF targetPos = QPointF(floatingPannel_Xpos,viewheight-floatingPannel_height);
      moveSomethingToPos(floatingControlPannelProxy,targetPos,200);
      int Oldsubmarginbottom = submarginbottom;
      submarginbottom += floatingPannel_height;
      resizelements("sub");
      floatingPannelDisplayed = true; //the floating pannel is displayed

        QTimer::singleShot(2000,[this,Oldsubmarginbottom](){
          resizelements("floatingPannel",200);// restoring the floating pannel to default (means it will be hidden)
          floatingPannelDisplayed = false;// the floating pannel is not displayed
          submarginbottom = Oldsubmarginbottom;
          resizelements("sub");
        });
    }
    return false;
  }
  return QMainWindow::eventFilter(obj, event);//return the event if it's not a mouse event
}

//distractor
MainWindow::~MainWindow(){
  savevideoposition();
  for (SubObject* ptr:subslist){
    delete ptr;
  }
}
