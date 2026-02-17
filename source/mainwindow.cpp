#include "mainwindow.h"
#include "qmediametadata.h"

#include <QApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QVideoWidget>

#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QMenu>
#include <QAction>
#include <QShortcut>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QGridLayout>

#include <QFileDialog>
#include <QTimer>
#include <QProcess>
#include <QKeyEvent>
#include <QStyleHints>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsVideoItem>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>

#include <fstream>
#include <sstream>
#include <filesystem>
#include <limits.h>
#include <algorithm>

void moveSomethingToPos(QGraphicsWidget *widget, QPointF targetPos, int animationTime);
template<typename T>
void clearVector(std::vector<T*>&);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->setWindowTitle("VFW");
  this->resize(750, 550);
  setFocusPolicy(Qt::StrongFocus);
  setupShortcuts();

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
  floatingControlPannelWidget->hide();
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

  //loading settings
  parseSettingsFile();

  // showing a blackscreen
  setPlayerDefaultState();

  // load the sub style
  SubConfig win(SYSTEMPATHS->configPath,SYSTEMPATHS->fontsDir,SYSTEMPATHS->currentThemeDir);
  win.loadFonts();
  htmlstyle = win.makehtml();
  subpadding = win.padding;
  subBottomMargin = win.marginbottom;
  currentSubBottomSpace = win.marginbottom;
}

//function to create the top layout
void MainWindow::createTopLayout(){
  if(topbarlayout != nullptr) delete topbarlayout; // gets triggered when toggling fullscreen

  topbarlayout = new TopBar();
  TopBarButtonsObjectList = TopBar::getTopBarActionsList();
  connect(topbarlayout, &TopBar::handleButtonsClick,[this](int actionNumber) {
     topBarButtonsHandler(actionNumber);
  });
}

//function to create the bottom layout
void MainWindow::createBottomLayout() {
  if (controlbuttonslayout != nullptr) {
    delete controlbuttonslayout;
  }

  controlbuttonslayout=nullptr;

  controlbuttonslayout = new BottomControlPanel(SYSTEMPATHS->currentIconsDir);//creating new layout

  connect(controlbuttonslayout, &BottomControlPanel::controlButtonsHandler, this, &MainWindow::controlButtonsHandler);
  connect(controlbuttonslayout, &BottomControlPanel::videoSliderMoved, this, &MainWindow::changePlayBackPosition);
  connect(controlbuttonslayout, &BottomControlPanel::volumeSliderMoved,[this](int value) {
    audio->setVolume((float)value / 1000);
  });

  connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::playbackPositionUpdated);
  connect(player, &QMediaPlayer::durationChanged, [this](qint64 duration){
    controlbuttonslayout->setVideoSliderRange(0,duration);
  });

  setVideoSliderRange(player->duration());
  controlbuttonslayout->setVideoSliderValue(player->position());
  updateButtonsIcon();
}

std::string lowerCase(std::string text){
  std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c){
    return std::tolower(c); 
  });
  return text;
}

void MainWindow::LoadingInDirectorySubtitles(QString currenturl){
  std::filesystem::path currentVideoPath(currenturl.toStdString()); 
  std::string directoryPath = currentVideoPath.parent_path().generic_string();

  if (!std::filesystem::exists(directoryPath)) {
      std::cout << "[ WARN ] Directory does not exist: " << directoryPath << "\n";
      return;
  }

  if (!std::filesystem::is_directory(directoryPath)) {
      std::cout << "[ WARN ] Path is not a directory: " << directoryPath << "\n";
      return;
  }

  if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
    for(const auto & fileEntry : std::filesystem::directory_iterator(directoryPath)){
      std::string fileExtention = fileEntry.path().extension().string();
      if(std::find(supportedSubtitlesFormats.begin(), supportedSubtitlesFormats.end(), fileExtention) != supportedSubtitlesFormats.end()){

        std::string filePathWithoutExtention = lowerCase(fileEntry.path().stem().generic_string());
        std::string currentVideoPathWithoutExtension = lowerCase(currentVideoPath.stem().generic_string());

        if(filePathWithoutExtention == currentVideoPathWithoutExtension){
          currentVideoSubtitlePaths.push_back(QString::fromStdString(fileEntry.path().generic_string()));
          std::cout<<"[ INFO ] Found subtitle: "<<fileEntry.path().generic_string()<<"\n";
          if(currentLoadedSubPath == ""){
            std::cout<<"[ INFO ] Subtitles were Loaded: "<<fileEntry.path().generic_string()<<"\n";
            currentLoadedSubPath = QString::fromStdString(fileEntry.path().generic_string());
            SubFileParsing(currentLoadedSubPath.toStdString());
            QAction * ToggleSubs = TopBarButtonsObjectList[TopBar::TOGGLE_SUB];
            ToggleSubs->setText("Remove Subtitles");
          }
        }
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

  std::cout<<"[ INFO ] Number Of Chapters Detected: "<<Chapters.size()<<"\n";

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
  controlbuttonslayout->setChaptersMarks(ChaptersVectors, showChaptersIndicators);
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

  std::cout<<"[ INFO ] Number Of Subs Detected: "<<subStreams.size()<<"\n";

  for(int i=0; i < subStreams.size(); i++) {
    std::filesystem::path currentVideoPath(currenturl.toStdString());
    std::string currentVideoPathWithoutExtension = currentVideoPath.parent_path().generic_string() +"/"+currentVideoPath.stem().generic_string();

    QString fileSubPath = QString("%1_Sub%2.srt").arg(QString::fromStdString(currentVideoPathWithoutExtension)).arg(i);
    QString subId = QString("0:s:%1").arg(i);

    QProcess* ffmpegProcess = new QProcess(this);
    connect(ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), ffmpegProcess, &QProcess::deleteLater); // delete the process when finished
    ffmpegProcess->start("/usr/bin/ffmpeg", {"-y","-i", currenturl, "-map", subId, fileSubPath});

    currentVideoSubtitlePaths.push_back(fileSubPath);

    if(i == 0){
      currentLoadedSubPath = fileSubPath;
      // when done extracting the first subtitle in the file parse the subs
      connect(ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), ffmpegProcess,[this](){
        SubFileParsing(currentLoadedSubPath.toStdString());
        // change the text of the QAction 
        QAction * ToggleSubs = TopBarButtonsObjectList[TopBar::TOGGLE_SUB];
        ToggleSubs->setText("Remove Subtitles");
      });
    }

    std::cout <<"[ INFO ] Extract subtitle to: " << fileSubPath.toStdString() <<"\n";
  }
}

void MainWindow::setPlayerDefaultState() {
  currentVideoUrl = "";
  videoIsPaused = true;
  player->setSource({});
  clearVector(audioTracksMetaDataVector);
  clearVector(subTracksMetaDataVector);
  controlbuttonslayout->hideSkipButton();
  controlbuttonslayout->setDefaultState();
  setVolumeSliderPosition(Settings["defaultVolume"] * 1000);
  updateButtonsIcon();
}

void MainWindow::playNextVideoInPlaylist() {
  if (currentVideoIndex > playlist.size()) {
    setPlayerDefaultState();
    return;
  }

  QString nextToPlayPath = playlist[currentVideoIndex].toLocalFile();
  startVideoPlayer(nextToPlayPath);
}

template<typename T>
void clearVector(std::vector<T*>& toClearVector) {
  for(auto& element : toClearVector) {
    delete element;
  }
  toClearVector.clear();
}

void MainWindow::getSubtitleTracksFromMetaData() {
  clearVector(subTracksMetaDataVector);
  std::cout << "SUBTITLES TRACKS:\n";
  for (int i=0 ; i < player->subtitleTracks().size() ; i++) {

    QMediaMetaData subTrack = player->subtitleTracks()[i];
    std::string title = subTrack.stringValue(QMediaMetaData::Title).toStdString();
    std::string language = subTrack.stringValue(QMediaMetaData::Language).toStdString();

    if (!title.empty()) std::cout << "  Title: " <<title<< "\n";
    if (!language.empty()) std::cout << "  Language: " <<language<< "\n";

    if (!title.empty() || !language.empty()) {
      MetaDataTrack *subTrackMetaData = new MetaDataTrack;
      subTrackMetaData->title = title;
      subTrackMetaData->language = language;
      subTrackMetaData->index = i;
      subTracksMetaDataVector.push_back(subTrackMetaData);

    } else {
      std::cout << "  (No metadata available)\n";
    }

  }
}

void MainWindow::getAudioTracksFromMetaData() {
  clearVector(audioTracksMetaDataVector);
  std::cout << "AUDIO TRACKS:\n";
  for (int i=0 ; i < player->audioTracks().size() ; i++) {
    QMediaMetaData audioTrack = player->audioTracks()[i];
    std::string title = audioTrack.stringValue(QMediaMetaData::Title).toStdString();
    std::string language = audioTrack.stringValue(QMediaMetaData::Language).toStdString();
   
    if (!title.empty()) std::cout << "  Title: " << title<< "\n";
    if (!language.empty()) std::cout << "  Language: " <<language<< "\n";

    if (!title.empty() || !language.empty()) {
      MetaDataTrack *audioTrackMetaData = new MetaDataTrack;
      audioTrackMetaData->title = title;
      audioTrackMetaData->language = language;
      audioTrackMetaData->index = i;
      audioTracksMetaDataVector.push_back(audioTrackMetaData);

    } else {
      std::cout << "  (No metadata available)\n";
    }
  }
}

void MainWindow::startVideoPlayer(QString path) {
  currentVideoUrl = path;
  video->setSize(view->size());
  QAction * ToggleSubs = TopBarButtonsObjectList[TopBar::TOGGLE_SUB];
  ToggleSubs->setText("Add Subtitles");
  currentVideoSubtitlePaths.clear();
  currentLoadedSubPath = "";

  //getting the path of the video playing as std::string
  std::filesystem::path currentPath(currentVideoUrl.toStdString());

  // getting the title of the video that is currently playing for later uses
  currentVideoTitle = currentPath.stem().generic_string();

  //get the current path of directory that the video is playing in
  currentVideoParentDirectory = currentPath.parent_path().generic_string();

  std::cout<<"\n";
  ExtranctingChapterData(currentVideoUrl);
  ExtractingBuiltInSubs(currentVideoUrl);
  std::cout<<"\n";
  LoadingInDirectorySubtitles(currentVideoUrl);
  std::cout<<"\n";

  connect(player, &QMediaPlayer::metaDataChanged, this, [this]() {
    std::cout<<"\n";
    getSubtitleTracksFromMetaData();
    getAudioTracksFromMetaData();
    std::cout<<"\n";
  });


  // startVideoPlayer setup (sound and video widget)
  player->setSource(QUrl::fromLocalFile(currentVideoUrl));
  player->setVideoOutput(video);
  player->setAudioOutput(audio);

  std::cout<<"Playing: "<<currentVideoUrl.toStdString()<<"\n";

  if(Settings.find("defaultVolume") != Settings.end()) {
    setVolumeSliderPosition(Settings["defaultVolume"] * 1000);
  }

  videoIsPaused = false;
  video->show();
  player->play();

  //resize ui elements based on the media opened
  resizeMainUiElement();
  updateButtonsIcon();

  // displaying the title for a brief of time
  int xposition = view->size().width() / 2;
  int yposition = view->size().height() - currentSubBottomSpace;
  showingthings(currentVideoTitle, xposition, yposition, 2000);

  //load the last saved position if it's availble
  getlastsavedposition();

  //save the position of the video what was playing before
  savevideoposition();
}

// topbarlayout buttons logic
void MainWindow::topBarButtonsHandler(int actionNumber) {

  QString url;
  switch (actionNumber) {
    case TopBar::Open_file: {
      QString displaydir;
      if (currentVideoParentDirectory.size())
        displaydir = QString::fromStdString(currentVideoParentDirectory);
      else{
        displaydir = QString::fromStdString(SYSTEMPATHS->HOME);
      }
      std::stringstream SupportecFormatsString; 
      SupportecFormatsString << "Video/Audio files ("; 
      for(size_t i=0;i<supportedMediaFormats.size();i++) SupportecFormatsString << "*" << supportedMediaFormats[i] <<" ";
      SupportecFormatsString << ")";

      url = QFileDialog::getOpenFileName(this, tr("Select Video File"), displaydir, tr(SupportecFormatsString.str().c_str()));

      if (!url.isEmpty()) {
        std::cout<<"Loading Video: "<<url.toStdString()<<"\n";
        startVideoPlayer(url);
        currentPlayerMode = PlayerMode::Single;
      }
      playlist.clear();
      break;
    }

    case TopBar::Open_folder: {
      QString displaydir;
      if (currentVideoParentDirectory.size())
        displaydir = QString::fromStdString(currentVideoParentDirectory);
      else{
        displaydir = QString::fromStdString(SYSTEMPATHS->HOME);
      }

      url = QFileDialog::getExistingDirectory(this, tr("Setect Playlist Directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks), displaydir);
      if (!url.isEmpty()) {
        playlist.clear();
        // saving all the urls in a list
        std::cout<<"\n"; 
        for (auto i : std::filesystem::directory_iterator(url.toStdString())) {
          if(std::find(supportedMediaFormats.begin(), supportedMediaFormats.end(),i.path().extension().string()) != supportedMediaFormats.end()){
            playlist.push_back(QUrl::fromLocalFile(QString::fromStdString(i.path().generic_string())));
            std::cout<<"Loading Video: "<<i.path().generic_string()<<"\n";
          }
        }

        if (playlist.size()) {
          currentPlayerMode = PlayerMode::Playlist;
          playNextVideoInPlaylist();
        }
      }
      break;
    }

    case TopBar::Open_media: {
      UrlWindow x(SYSTEMPATHS->currentThemeDir);
      x.exec();
      url = x.url;
      if (!url.isEmpty()) {
        playlist.clear();
        currentPlayerMode = PlayerMode::Single;
        startVideoPlayer(url);
      }
      break;
    }

    case TopBar::Quit: {
      QApplication::quit();
      break;
    }

    case TopBar::JUMP_BACKWARD: {
      changePlayBackPosition(player->position() - 5000);
      break;
    }

    case TopBar::JUMP_FORWARD: {
      changePlayBackPosition(player->position() + 5000);
      break;
    }

    case TopBar::JUMP_TO_TIME: {
      JumpTime x(SYSTEMPATHS->currentThemeDir);
      x.exec();
      if (x.targettime >= 0) {
        changePlayBackPosition(x.targettime * 1000);
      }
      break;
    }

    case TopBar::JUMP_TO_NEXT_CHAP:{
      moveToNextChapter();
      break;
    }

    case TopBar::JUMP_TO_PREV_CHAP:{
      moveToPrevChapter();
      break;
    }

    case TopBar::TOGGLE_LOOPSEGMENT: {
      QAction * toggleLoopAction = TopBarButtonsObjectList[TopBar::TOGGLE_LOOPSEGMENT];
      if(!currentVideoUrl.isEmpty()){
        if(!segmentLoopEnabled){
          SRepeatWindow win(SYSTEMPATHS->currentThemeDir);
          win.exec();
          if (win.startingpoint >= 0 && win.finishingpoint >= 0 && win.finishingpoint != win.startingpoint) {
            segmentLoopEnabled = true;
            segmentLoopStart = win.startingpoint;
            segmentLoopEnd = win.finishingpoint;
            changePlayBackPosition(segmentLoopStart * 1000);
            toggleLoopAction->setText("Exit The Loop");

          }
        }else{
          segmentLoopEnabled = false;
          toggleLoopAction->setText("Start Segment Loop");
        }
      }
      break;
    }

    case TopBar::AUDIO_TRACKS: {
      static int audioTrackIndex = 0;
      AudioTracksManager audioTracksManagerWindow(SYSTEMPATHS->currentThemeDir, SYSTEMPATHS->currentIconsDir, audioTracksMetaDataVector, audioTrackIndex);
      audioTracksManagerWindow.exec();
      if(audioTracksManagerWindow.newAudioTrackIndex >= 0) {
        audioTrackIndex = audioTracksManagerWindow.newAudioTrackIndex;
        player->setActiveAudioTrack(audioTrackIndex);
      }
      break;
    }

    case TopBar::FULL_VOLUME: {
      setVolumeSliderPosition(1000);
      break;
    }

    case TopBar::MUTE: {
      setVolumeSliderPosition(0);
      break;
    }

    case TopBar::TOGGLE_ASPRadio: {
      QAction * toggleFillAction = TopBarButtonsObjectList[TopBar::TOGGLE_ASPRadio];
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

    case TopBar::TOGGLE_SUB: {
      QAction * ToggleSubs = TopBarButtonsObjectList[TopBar::TOGGLE_SUB];
      QString displaydir;
      if(currentLoadedSubList.size() == 0){
        if (currentVideoParentDirectory.size())
          displaydir = QString::fromStdString(currentVideoParentDirectory);
        else{
          displaydir = QString::fromStdString(SYSTEMPATHS->HOME);
        }

        std::stringstream SupportedSubtitlesFormatstring; 
        SupportedSubtitlesFormatstring << "Srt files ("; 

        for(size_t i=0;i<supportedSubtitlesFormats.size();i++) SupportedSubtitlesFormatstring << "*" << supportedSubtitlesFormats[i] <<" ";
        SupportedSubtitlesFormatstring << ")";

        currentLoadedSubPath = QFileDialog::getOpenFileName(this, tr("Select Subtitle file"), displaydir, tr(SupportedSubtitlesFormatstring.str().c_str()));
        if (!currentLoadedSubPath.isEmpty()) {
          SubFileParsing(currentLoadedSubPath.toStdString());
          std::cout<<"[ INFO ] Subtitles were Loaded: "<<currentLoadedSubPath.toStdString()<<"\n";
          ToggleSubs->setText("Remove Subtitles");
        }
      }else{
        for (SubObject* ptr:currentLoadedSubList){
          delete ptr;
        }
        sublabel->setOpacity(0);
        currentLoadedSubList.clear();
        currentLoadedSubPath = "";
        ToggleSubs->setText("Add Subtitles");
      }
      break;
    } 

    case TopBar::LOADSUBTITLES:{
      subWindow subWin(SYSTEMPATHS->currentThemeDir,SYSTEMPATHS->currentIconsDir,currentVideoSubtitlePaths,currentLoadedSubPath);
      subWin.exec();
      if(!subWin.clickedSubPath.isEmpty()){
        currentLoadedSubPath = subWin.clickedSubPath;
        sublabel->setOpacity(0);
        currentLoadedSubList.clear();
        SubFileParsing(currentLoadedSubPath.toStdString());
        std::cout<<"[ INFO ] Subtitles were Loaded: "<<currentLoadedSubPath.toStdString()<<"\n";
        QAction * ToggleSubs = TopBarButtonsObjectList[TopBar::TOGGLE_SUB];
        ToggleSubs->setText("Remove Subtitles");
      }
      break;
    }

    case TopBar::TOGGLE_SUBDISPLAY: {
      QAction * ToggleSubsDisplay = TopBarButtonsObjectList[TopBar::TOGGLE_SUBDISPLAY];
      toggleSubtitles();
      if(ShowSubs){
        ToggleSubsDisplay->setText("Hide Subtitles");
      }else{
        ToggleSubsDisplay->setText("Display Subtitles");
      }
      break;
    }

    case TopBar::TOGGLE_CHAPTERSINDICATORS: {
      QAction * toggleChapters = TopBarButtonsObjectList[TopBar::TOGGLE_CHAPTERSINDICATORS];
      toggleChaptersIndicators();
      if(showChaptersIndicators){
        toggleChapters->setText("Hide Chapters Indicators");
      }else{
        toggleChapters->setText("Display Chapters Indicators");
      }
      break;
    }

    case TopBar::ADDDELAY: {
      increaseSubtitlesDelay();
      break;
    }

    case TopBar::REDUCEDELAY: {
      decreaseSubtitlesDelay();
      break;
    }

    case TopBar::SUBSETTINGS: {
      SubConfig win(SYSTEMPATHS->configPath,SYSTEMPATHS->fontsDir,SYSTEMPATHS->currentThemeDir);
      win.gui();
      win.exec();
      htmlstyle = win.makehtml();
      subpadding = win.padding;
      subBottomMargin = win.marginbottom;
      currentSubBottomSpace = win.marginbottom;
      break;
    }

    case TopBar::TITLE: {
      if (currentVideoTitle.size()) {
        int xposition = view->size().width() / 2;
        int yposition = view->size().height() - subBottomMargin;
        showingthings(currentVideoTitle, xposition, yposition, 3000);
      }
      break;
    }

    case TopBar::THEME:{
      ChangeThemeWindow win(SYSTEMPATHS->configPath,SYSTEMPATHS->themesDir,SYSTEMPATHS->currentThemeDir);
      win.exec();
      int xposition = view->size().width() / 2;
      int yposition = view->size().height()/2 ;
      if(win.changetotheme!=""){
        showingthings("You Need to reset the Application to apply the new Theme",xposition,yposition,4000);
      }
      break;
    }

    case TopBar::SHORTCUTS: {
      ShortcutsInst win(nullptr,SYSTEMPATHS->currentThemeDir,SYSTEMPATHS->configPath);
      win.exec();
      break;
    }

  }
}

// controlbuttonslayout buttons logic
void MainWindow::controlButtonsHandler(int buttonindex) {
  switch (buttonindex) {

    case BottomControlPanel::PAUSE_BUTTON: {
      if (!videoIsPaused) player->pause();
      else player->play();
      videoIsPaused = !videoIsPaused ;
      updateButtonsIcon();
      break;
    }

    case BottomControlPanel::BACK_BUTTON: {
      if (currentPlayerMode == PlayerMode::Playlist && currentVideoIndex > 0) {
        currentVideoIndex--;
        playNextVideoInPlaylist();
      }
      break;
    }

    case BottomControlPanel::STOP_BUTTON: {
      savevideoposition();
      closeVideo();
      break;
    }

    case BottomControlPanel::NEXT_BUTTON: {
      if (currentPlayerMode == PlayerMode::Playlist) {
        player->setPosition(player->duration());
      }
      break;
    }

    case BottomControlPanel::FULLSCREEN_BUTTON: {
      toggleFullScreen();
      break;
    }

    case BottomControlPanel::PLAYLIST_BUTTON: {
      PlaylistManager win(SYSTEMPATHS->currentThemeDir, SYSTEMPATHS->currentIconsDir, playlist, currentVideoUrl);
      win.exec();
      if (win.new_video_index != (int)currentVideoIndex && win.new_video_index != -1) {
        currentVideoIndex = win.new_video_index;
        playNextVideoInPlaylist();
      }
      break;
    }

    case BottomControlPanel::REPETITION_BUTTON: {
      if (rep == BottomControlPanel::PlaylistRepeat) rep = BottomControlPanel::VideoRepeat;
      else if (rep == BottomControlPanel::VideoRepeat) rep = BottomControlPanel::Shuffle;
      else if (rep == BottomControlPanel::Shuffle) rep = BottomControlPanel::PlaylistRepeat;
      updateRepeatButtonIcon();

      break;
    }

    case BottomControlPanel::CONTINUE_FROM_LAST_POS_BUTTON:{
      changePlayBackPosition(lastPlaybackPosition);
      controlbuttonslayout->hideSkipButton();
      break;
    }

    case BottomControlPanel::TOGGLE_VOLUME_BUTTON: {
      toggleVolume();
      break;
    }

  }
}

// keyboard event catching function
void MainWindow::setupShortcuts(){
    setFocusPolicy(Qt::StrongFocus);

    // --- Playback controls ---
    auto pause = new QShortcut(QKeySequence(Qt::Key_Space), this);
    pause->setContext(Qt::ApplicationShortcut);
    connect(pause, &QShortcut::activated, this, [this]{
        controlButtonsHandler(BottomControlPanel::PAUSE_BUTTON);
    });

    auto seekFwd = new QShortcut(QKeySequence(Qt::Key_Right), this);
    seekFwd->setContext(Qt::ApplicationShortcut);
    connect(seekFwd, &QShortcut::activated, this, [this]{
        changePlayBackPosition(player->position() + 5000);
    });

    auto seekBack = new QShortcut(QKeySequence(Qt::Key_Left), this);
    seekBack->setContext(Qt::ApplicationShortcut);
    connect(seekBack, &QShortcut::activated, this, [this]{
        changePlayBackPosition(std::max(0,static_cast<int>(player->position() - 5000)));
    });

    // --- Volume controls ---
    auto volUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    volUp->setContext(Qt::ApplicationShortcut);
    connect(volUp, &QShortcut::activated, this, [this]{
        int volumeSliderValue = controlbuttonslayout->getVolumeValue();
        setVolumeSliderPosition(volumeSliderValue + 100);
    });

    auto volDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    volDown->setContext(Qt::ApplicationShortcut);
    connect(volDown, &QShortcut::activated, this, [this]{
        int volumeSliderValue = controlbuttonslayout->getVolumeValue();
        setVolumeSliderPosition(volumeSliderValue - 100);
    });

    // --- Fullscreen ---
    auto toggleFS = new QShortcut(QKeySequence(Qt::Key_F), this);
    toggleFS->setContext(Qt::ApplicationShortcut);
    connect(toggleFS, &QShortcut::activated, this, &MainWindow::toggleFullScreen);

    auto escFS = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    escFS->setContext(Qt::ApplicationShortcut);
    connect(escFS, &QShortcut::activated, this, &MainWindow::exitFullScreen);

    // --- Subtitles controls ---
    auto toggleSubs = new QShortcut(QKeySequence(Qt::Key_V),this);
    toggleSubs->setContext(Qt::ApplicationShortcut);
    connect(toggleSubs, &QShortcut::activated, this, &MainWindow::toggleSubtitles);
    
    // --- Chapters controls ---
    auto toggleChaptersIndHandler = new QShortcut(QKeySequence(Qt::Key_C),this);
    toggleChaptersIndHandler->setContext(Qt::ApplicationShortcut);
    connect(toggleChaptersIndHandler,&QShortcut::activated,this, &MainWindow::toggleChaptersIndicators);

    auto goToNextChapter = new QShortcut(QKeySequence(Qt::Key_N),this);
    goToNextChapter->setContext(Qt::ApplicationShortcut);
    connect(goToNextChapter, &QShortcut::activated, this, &MainWindow::moveToNextChapter);

    auto goToPrevChapter = new QShortcut(QKeySequence(Qt::Key_B),this);
    goToPrevChapter->setContext(Qt::ApplicationShortcut);
    connect(goToPrevChapter, &QShortcut::activated, this, &MainWindow::moveToPrevChapter);

    // --- Other controls ---
    auto volPanel = new QShortcut(QKeySequence(Qt::Key_M), this);
    volPanel->setContext(Qt::ApplicationShortcut);
    connect(volPanel, &QShortcut::activated, this, [this]{
      controlButtonsHandler(BottomControlPanel::TOGGLE_VOLUME_BUTTON);
    });

    auto reduceDelay = new QShortcut(QKeySequence(Qt::Key_G), this);
    reduceDelay->setContext(Qt::ApplicationShortcut);
    connect(reduceDelay, &QShortcut::activated, this, [this]{
      topBarButtonsHandler(TopBar::REDUCEDELAY);
    });

    auto addDelay = new QShortcut(QKeySequence(Qt::Key_H), this);
    addDelay->setContext(Qt::ApplicationShortcut);
    connect(addDelay, &QShortcut::activated, this, [this]{
      topBarButtonsHandler(TopBar::ADDDELAY);
    });
}

void MainWindow::increaseSubtitlesDelay() {
  if (currentLoadedSubList.size()) {
    currentSubDelay += 100;
    if (-100 < currentSubDelay && currentSubDelay < 100)
      currentSubDelay = 0;

    // add animation so the user can see that the delay has been changed
    std::string text = "Subtitles Delay: " + std::to_string((int)(currentSubDelay)) + " ms";
    int xposition = view->size().width();
    int yposition = view->size().height();
    showingthings(text, xposition / 2, yposition / 2, 1000);
  }
}
void MainWindow::decreaseSubtitlesDelay(){
  if (currentLoadedSubList.size()) {
    currentSubDelay -= 100;
    if (-100 < currentSubDelay && currentSubDelay < 100)
      currentSubDelay = 0;

    // add animation so the user can see that the delay has been changed
    std::string text = "Subtitles Delay: " + std::to_string((int)(currentSubDelay)) + " ms";
    int xposition = view->size().width();
    int yposition = view->size().height();
    showingthings(text, xposition / 2, yposition / 2, 1000);
  }
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
  controlbuttonslayout->setChaptersMarks(ChaptersVectors, showChaptersIndicators);
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
      changePlayBackPosition(newChapter.startTime*1000+1);
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
      changePlayBackPosition(newChapter.startTime*1000+1);
      QString titleToShow = htmlstyle + newChapter.title + "</div>";
      int video_start_x = view->pos().rx();
      int video_start_y = view->pos().ry();
      showingthings(titleToShow.toStdString(),video_start_x+calculatingTextDimentions(titleToShow)[0]/2+10,video_start_y+calculatingTextDimentions(newChapter.title)[1]+15, 2000); 
 
      break;
    }
  }
} 


// setting the range of the slider basing on the player
void MainWindow::setVideoSliderRange(qint64 playbackPosition) {
  controlbuttonslayout->setVideoSliderRange(0, playbackPosition); 
}

void MainWindow::syncSubtitles(qint64 playbackPosition) {
  for(const SubObject* subObj : currentLoadedSubList){
    bool hasPlaybackPassedSubStart = (playbackPosition >= (subObj->startTime + currentSubDelay));
    bool isPlaybackBeforeSubEnd = (playbackPosition <= (subObj->endTime + currentSubDelay));

    if (ShowSubs && hasPlaybackPassedSubStart && isPlaybackBeforeSubEnd) {
      // merge the html style with the subtitle and pass it as html script
      sublabel->setHtml(htmlstyle + QString::fromStdString(subObj->textContent) + "</td></tr></table>");
      sublabel->setOpacity(1);
      repositionSubtitles();
      return; 
    }
  }

  sublabel->setHtml("");
}

// setting the app elements in there relation with the player
void MainWindow::playbackPositionUpdated(qint64 playbackPosition) {

  controlbuttonslayout->setVideoSliderValue(playbackPosition);//syncing the slider with the player position

  if (playbackPosition != player->duration()) {
    updateTimeLabels();

  } else if (playbackPosition == player->duration()) {
    determineNextVideo();

  }

  if (segmentLoopEnabled && playbackPosition >= segmentLoopEnd * 1000) {
    changePlayBackPosition(segmentLoopStart * 1000);
  }

  // syncing subtitles to the player position
  syncSubtitles(playbackPosition);
}

void MainWindow::determineNextVideo() {
  if (rep == BottomControlPanel::PlaylistRepeat) {
    if (playlist.size()) {
      if (currentVideoIndex == playlist.size() - 1) currentVideoIndex = 0;
      else currentVideoIndex++;
      playNextVideoInPlaylist();

    } else {
      closeVideo();
    }

  } else if(rep == BottomControlPanel::VideoRepeat) {
     changePlayBackPosition(0);

  } else if (rep == BottomControlPanel::Shuffle) {
    currentVideoIndex = rand() % playlist.size();
    playNextVideoInPlaylist();

  }

  controlbuttonslayout->setVideoSliderValue(0);
}

void MainWindow::updateTimeLabels(){
  if(currentVideoUrl == "") return;
  controlbuttonslayout->updatePlayBackTimer(player->position());
  controlbuttonslayout->updateDurationTimer(player->duration());
}



/*
  this function is to work around a bug that break the audio when changing position,
  by deleting the audio output widget and create a new one
  every time we change position.
*/

void MainWindow::changePlayBackPosition(int newpos) {
  QMediaMetaData meta = player->metaData();
  QVariant audioCodec = meta.value(QMediaMetaData::AudioCodec);

  if(audioCodec.isValid()){
    float oldvol = audio->volume();
    player->pause();
    player->setAudioOutput(nullptr);
    delete audio;
    audio = new QAudioOutput();
    player->setPosition(newpos);
    player->setAudioOutput(audio);
    audio->setVolume(oldvol);
    if (!videoIsPaused){
      player->play();
      videoIsPaused = false;
    }

  }else{
    player->setPosition(newpos);
  }
}

void MainWindow::toggleVolume() {
  static float oldVolumeSliderPos = 500;
  int currentVolumeSliderPos = controlbuttonslayout->getVolumeValue();
  if (currentVolumeSliderPos > 0.0f) {
    oldVolumeSliderPos = currentVolumeSliderPos;
    setVolumeSliderPosition(0);
  } else {
    setVolumeSliderPosition(oldVolumeSliderPos);
  }
}

// managing the interactions with the volume slider
void MainWindow::setVolumeSliderPosition(qreal newPosition) {
  controlbuttonslayout->setVolumeSliderPosition(newPosition);
  updateVolumeButtonIcon();
  Settings["defaultVolume"] = (float) newPosition / 1000;
}

void MainWindow::closeVideo(){
  setPlayerDefaultState();
  playlist.clear();
  currentPlayerMode = PlayerMode::Single;
}

bool lineIsEmpty(const std::string str){
  return str.empty() || std::all_of(str.begin(), str.end(), [](unsigned char c){
      return std::isspace(c);
  });
}

bool stringIsInteger(std::string text){
  if(text.empty()) return false;
  for(char c : text){
    if(c < '0' || c > '9') {
      if(c != ' ' && c != '\r') return false;
    }
  }
  return true;
}

bool lineContainsTimeArrow(std::string text){
  return (text.find("-->") != std::string::npos);
}

double formatStringTime(const std::string& stringTimer) {
  std::istringstream ss(stringTimer);
  char step;
  int h, m, s, ms;
  ss >> h >> step >> m >> step >> s >> step >> ms;
  return (h*60*60 + m*60 + s) * 1000 + ms;
}

std::pair<double,double> parseTimeString(std::string stringTimeInterval){
  std::istringstream ss(stringTimeInterval);
  int h1, min1, sec1, milsec1, h2, min2, sec2, milsec2;
  std::string arrow;
  char step;
  ss>>h1>>step>>min1>>step>>sec1>>step>>milsec1;
  ss>>std::ws>>arrow>>std::ws;
  ss>>h2>>step>>min2>>step>>sec2>>step>>milsec2;

  // calculating the starting time from the string line (hour, minutes, seconds, ms)
  double firstTime = (h1*60*60 + min1*60 + sec1) * 1000 + milsec1;// ms

  // calculating the ending time from the string line (hour, minutes, seconds, ms)
  double secondTime = (h2*60*60 + min2*60 + sec2) * 1000 + milsec2; // ms
  return std::pair<double,double> (firstTime, secondTime);
}

void MainWindow::parsingAssSubsFile(std::string subpath) {
  std::ifstream file(subpath);
  if(!file){
    std::cerr << "[ WARN ] Cannot find .ass subtitle File: "<<subpath<<"\n";
    return;
  }

  std::string line;
  currentLoadedSubList.clear();
  int startTimeIndex=1; 
  int endTimeIndex=2;
  int textIndex=9;

  while(getline(file,line)){
    if (line.find("Format:") != std::string::npos) {
      line.erase(std::remove(line.begin(),line.end(),' '), line.end());
      line.erase(std::remove(line.begin(),line.end(),'\r'), line.end());
      QStringList splitedLine = QString::fromStdString(line).split(",");
      for(int i=0;i<splitedLine.size();i++){
        if(splitedLine[i] == "Start") startTimeIndex = i;
        if(splitedLine[i] == "End") endTimeIndex = i;
        if(splitedLine[i] == "Text") textIndex = i;
      }

    } else if(line.find("Dialogue:") != std::string::npos) {
      QStringList splitedDialogLine = QString::fromStdString(line).split(",");

      std::string subText="";
      for(int i = textIndex; i<splitedDialogLine.size(); i++){
        subText += splitedDialogLine[i].toStdString();
        subText += ",";
      }
      subText.erase(subText.size()-1);

      size_t pos = 0;
      while ((pos = subText.find("\\N", pos)) != std::string::npos) {
        subText.replace(pos, 2, "<br/>");
        pos += 1;
      }
      while ((pos = subText.find("\\n", pos)) != std::string::npos) {
        subText.replace(pos, 2, "<br/>");
        pos += 1;
      }

      double startTime = formatStringTime(splitedDialogLine[startTimeIndex].toStdString());
      double endTime = formatStringTime(splitedDialogLine[endTimeIndex].toStdString());

      SubObject * newSubObj = new SubObject;
      newSubObj->textContent = subText;
      newSubObj->startTime = startTime;
      newSubObj->endTime = endTime;
      currentLoadedSubList.push_back(newSubObj);
    }
  }
  file.close();
}

void MainWindow::parsingSrtLikeSubsFile(std::string subpath) {
  std::ifstream file(subpath);
  if(!file){
    std::cerr << "[ WARN ] Cannot open subtitle File: "<<subpath<<"\n";
    return;
  }

  std::string currentLine;
  SubObject *newSubObj = nullptr;

  // looping the lines in the file
  while (getline(file, currentLine)) {
    if (lineContainsTimeArrow(currentLine)) {
      std::pair <double,double> timesPair = parseTimeString(currentLine);
      double firstTime = timesPair.first;
      double secondTime = timesPair.second;

      if (newSubObj != nullptr)
        currentLoadedSubList.push_back(newSubObj);

      newSubObj = new SubObject;
      newSubObj->startTime = firstTime;
      newSubObj->endTime = secondTime;

    } else if(newSubObj != nullptr && !stringIsInteger(currentLine)) {
      currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\r'), currentLine.end());

      if (newSubObj->textContent.empty()) {
        newSubObj->textContent = currentLine;

      } else if(!lineIsEmpty(currentLine)) {
        newSubObj->textContent += "<br/>" + currentLine;

      }
    }
  }

  if(newSubObj != nullptr)
    currentLoadedSubList.push_back(newSubObj);

  file.close();
}

// scraping the subtitles from the sub file
void MainWindow::SubFileParsing(std::string subpath) {
  for (SubObject* ptr:currentLoadedSubList) delete ptr;
  currentLoadedSubList.clear();
  if(std::filesystem::path(subpath).extension().string() == ".ass" ||
     std::filesystem::path(subpath).extension().string() == ".ssa")
    parsingAssSubsFile(subpath);

  else parsingSrtLikeSubsFile(subpath);
}

//function to resize ui elements
void MainWindow::resizeMainUiElement(){
  resizeVideoContainers();
  repositionFloatingControllPannel();
  repositionSubtitles();
  // showingthings("",0,0,0);
}

void MainWindow::resizeVideoContainers() {
  int VIEWWIDTH = view->size().width();
  int VIEWHEIGHT = view->size().height();
  video->setSize(QSize(VIEWWIDTH + 2, VIEWHEIGHT + 2));
  scene->setSceneRect(0, 0, VIEWWIDTH - 1, VIEWHEIGHT - 1);
  view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

void MainWindow::repositionFloatingControllPannel(int animationTime) {
  int VIEWWIDTH = view->size().width();
  int VIEWHEIGHT = view->size().height();
  int floatingPannel_width = floatingControlPannelProxy->boundingRect().width();
  int floatingPannel_height = floatingControlPannelProxy->boundingRect().height();
  QPoint hiding_position = QPoint((VIEWWIDTH - floatingPannel_width) / 2, (VIEWHEIGHT + floatingPannel_height));
  moveSomethingToPos(floatingControlPannelProxy,  hiding_position, animationTime);
}

void MainWindow::repositionSubtitles() {
  int VIEWWIDTH = view->size().width();
  int VIEWHEIGHT = view->size().height();
  int SUBWIDTH = sublabel->boundingRect().width();
  int SUBHEIGHT = sublabel->boundingRect().height();
  sublabel->setPos((VIEWWIDTH - SUBWIDTH) / 2, (VIEWHEIGHT - SUBHEIGHT / 2) - currentSubBottomSpace);
}

//resizing window logic
void MainWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  resizeMainUiElement();
}

void MainWindow::enterFullScreen() {
  setTopbarLayoutVisible(false);
  int currentVolumeSliderPos = controlbuttonslayout->getVolumeValue();

  createBottomLayout();//recreating bottom layout
  floatingControlPannelContainerLayout->addLayout(controlbuttonslayout);

  this->showFullScreen();
  mainlayout->setContentsMargins(0, 0, 0, 0);
  video->setSize(this->size());

  if (floatingControlPannelWidget->isHidden())
    floatingControlPannelWidget->show();

  setVolumeSliderPosition(currentVolumeSliderPos);
  updateButtonsIcon();
  updateTimeLabels();

  fullScreenEnabled = true;
}

void MainWindow::exitFullScreen() {
  setTopbarLayoutVisible(true);
  int currentVolumeSliderPos = controlbuttonslayout->getVolumeValue();

  createBottomLayout();//recreating bottom layout
  mainlayout->addLayout(controlbuttonslayout);

  this->showMaximized();
  mainlayout->setContentsMargins(10, 10, 10, 10);

  setVolumeSliderPosition(currentVolumeSliderPos);
  updateButtonsIcon();
  updateTimeLabels();

  fullScreenEnabled = false;
}

//function to toggle the fullscreen
void MainWindow::toggleFullScreen() {
  fullScreenEnabled ? exitFullScreen() : enterFullScreen();
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
void MainWindow::setTopbarLayoutVisible(bool visible) {
  for (int i = 0; i < topbarlayout->count(); i++) {
    QWidget *searchtoolbutton = topbarlayout->itemAt(i)->widget();
    if (searchtoolbutton) {
      searchtoolbutton->setVisible(visible);
    }
  }
}

void MainWindow::updateButtonsIcon(){
  updatePlayPauseButtonIcon();
  updateVolumeButtonIcon();
  updateRepeatButtonIcon();
}

void MainWindow::updatePlayPauseButtonIcon() {
  bool Paused = (!videoIsPaused || currentVideoUrl == "");
  controlbuttonslayout->updatePausePlayButtonIcon(Paused);
}

void MainWindow::updateVolumeButtonIcon() {
  controlbuttonslayout->updateVolumeButtonIcon();
}

void MainWindow::updateRepeatButtonIcon() {
  controlbuttonslayout->updateRepetitionButtonIcon(rep);
}

//function to save the position of a video after closing it
void MainWindow::savevideoposition(){
  if(currentVideoTitle.size() && player->position() > 5000){
    std::string LPPPath = (std::filesystem::path(SYSTEMPATHS->cachePath) / "playbackState").string();
    std::ofstream possavefile(LPPPath,std::ios::app);
    std::string line = currentVideoTitle+" ; "+QString::number(player->position()).toStdString()+'\n';
    possavefile<<line;
    possavefile.close();
  }
}

//load old position of a video if available
void MainWindow::getlastsavedposition(){
  if(currentVideoTitle == "") return;
  lastPlaybackPosition = 0;
  std::string LPPPath = (std::filesystem::path(SYSTEMPATHS->cachePath) / "playbackState").string();
  std::ifstream possavefile(LPPPath);
  if (possavefile){
    std::string line;
    while(getline(possavefile,line)){
      if(line.find(currentVideoTitle)!=std::string::npos){
        lastPlaybackPosition = std::stoi(line.substr(line.rfind(";")+1,line.size()-1));
      }
    }

    if(lastPlaybackPosition){
      controlbuttonslayout->showSkipButton();
    }
  }else{
      std::cerr << "[ WARN ] Failed to open LPP  File: "<<LPPPath<<"\n";
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
    toggleFullScreen();
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

//check the mouse is inside the floating panel
bool MainWindow::mouseInsideFloatingPanel(QEvent* event){
  QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
  
  QPoint mousePos = mouseEvent->pos();// getting the position of the mouse
  //getting the dimentions of the floating pannel
  int floatingPannel_height = floatingControlPannelProxy->boundingRect().height();
  int floatingPannel_width = floatingControlPannelProxy->boundingRect().width();

  // getting the position of each side of the floating pannel
  int floatingPannel_Top = floatingControlPannelProxy->pos().ry();
  int floatingPannel_Left = floatingControlPannelProxy->pos().rx();
  int floatingPannel_Bottom = floatingPannel_Top + floatingPannel_height;
  int floatingPannel_Right = floatingPannel_Left + floatingPannel_width;

  // checking if the mouse is inside the pannel X wise, and Y wise
  bool mouseInsideX = (mousePos.rx() >= floatingPannel_Left) && (mousePos.rx() <= floatingPannel_Right);
  bool mouseInsideY = (mousePos.ry() >= floatingPannel_Top) && (mousePos.ry() <= floatingPannel_Bottom);

  return (mouseInsideX && mouseInsideY);
}

//function to detect mouse movement
bool MainWindow::eventFilter(QObject *obj, QEvent *event){
  static bool floatingPannelDisplayed = false; //this variable stores if the floating pannel is displayed already or not
  
  // this if condition check if the event is a mouse event, and if the element the mouse is moving on top of is not a widget 
  if(event->type() == QEvent::MouseMove &&!static_cast<QWidget* >(obj)->isWidgetType())
    MouseIsInsideFloatingPanel = mouseInsideFloatingPanel(event); // check if mouse inside the floating panel 
  
  
  if (event->type() == QEvent::MouseMove && !floatingPannelDisplayed) {
    if(fullScreenEnabled){
      //getting the position of the floating pannel
      int viewheight = view->size().height();
      int floatingPannel_Xpos = floatingControlPannelProxy->pos().rx();
      int floatingPannel_height = floatingControlPannelProxy->boundingRect().height();

      // calculate the targetPosition that we want to move the pannel into, and then calling moveSomethingToPos function
      QPointF targetPos = QPointF(floatingPannel_Xpos,viewheight-floatingPannel_height);
      moveSomethingToPos(floatingControlPannelProxy,targetPos,200);

      // keeping the same subtitles margin
      currentSubBottomSpace = subBottomMargin + floatingPannel_height;
      repositionSubtitles();
      floatingPannelDisplayed = true; //the floating pannel is displayed

      QTimer::singleShot(800,[this](){
        if(!MouseIsInsideFloatingPanel && floatingPannelDisplayed){
          repositionFloatingControllPannel(200);
          currentSubBottomSpace = subBottomMargin;
          repositionSubtitles();
        }
        floatingPannelDisplayed = false;// the floating pannel is not displayed
      });
    }
    return false;
  }
  return QMainWindow::eventFilter(obj, event);//return the event if it's not a mouse event
}

void MainWindow::parseSettingsFile(){
  std::string settingsFilePath = SYSTEMPATHS->configPath + "/settings";
  std::ifstream file(settingsFilePath);
  std::string line;

  if(file){
    while(std::getline(file,line)){
      if(line.find(":") != std::string::npos){
        try{
          std::istringstream sstr(line);
          std::string temp;

          std::getline(sstr,temp,':');
          std::string item = temp;
          
          std::getline(sstr,temp);
          float value = std::stof(temp);

          Settings[item] = value;

        }catch(const std::invalid_argument& e){
          std::cout << "[ WARN ] Failed to fetch from Settings File: "<<e.what()<<"\n";
        }
      }
    }
  }else{
    std::cerr << "[ WARN ] Cannot find Settings File: "<<settingsFilePath<<"\n";
  }

}

void MainWindow::savingNewSettings(){
  std::string settingsFilePath = SYSTEMPATHS->configPath + "/settings";
  std::ofstream settingsFile(settingsFilePath);

  std::stringstream sstr;
  for(const auto& [key,value] : Settings){
    sstr << key << ":" << value << "\n";
  }
  std::string newSettings = sstr.str();
 
  settingsFile << newSettings;
  settingsFile.close();
}

//distractor
MainWindow::~MainWindow(){
  savingNewSettings();
  savevideoposition();
  for (SubObject* ptr:currentLoadedSubList){
    delete ptr;
  }
}
