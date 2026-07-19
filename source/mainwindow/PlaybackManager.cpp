#include "../headers/main/mainwindow.h"

void MainWindow::prepareVideoFile(QString filePath) {
  currentVideoUrl = filePath;
  cleanSubtitles();
  std::filesystem::path currentPath(filePath.toStdString());
  currentVideoTitle = currentPath.stem().generic_string();
  currentVideoParentDirectory = currentPath.parent_path().generic_string();

  std::cout<<"\n";
  ExtractingChapterData(filePath);
  ExtractingBuiltInSubs(filePath);
  std::cout<<"\n";
  LoadingInDirectorySubtitles(filePath);
  std::cout<<"\n";

  startVideoPlayer(
    QUrl::fromLocalFile(filePath),
    QString::fromStdString(currentVideoTitle)
  );
}

void MainWindow::prepareVideoURL(QString videoUrl) {
  currentVideoUrl = videoUrl;
  cleanSubtitles();
  QUrl url(videoUrl);
  QString urlPath = url.path();
  QFileInfo fileInfo(urlPath);

  currentVideoTitle = fileInfo.fileName().toStdString();
  currentVideoParentDirectory = "";

  std::cout<<"\n";
  ExtractingChapterData(videoUrl);
  ExtractingBuiltInSubs(videoUrl);
  std::cout<<"\n";

  startVideoPlayer(url, QString::fromStdString(currentVideoTitle));
}

void MainWindow::startVideoPlayer(QUrl videoPath, QString videoTitle) {
  video->setSize(view->size());
  player->setSource(videoPath);
  player->setVideoOutput(video);
  player->setAudioOutput(audio);

  std::cout<<"Playing: "<<videoPath.toString().toStdString()<<"\n";

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
  renderOverlayText(videoTitle.toStdString(), TextPosition::BOTTOM, 2000);

  //load the last saved position if it's availble
  getlastsavedposition();

  //save the position of the video what was playing before
  savevideoposition();
}

void MainWindow::playNextVideoInPlaylist() {
  if (currentVideoIndex > playlist.size()) {
    setPlayerDefaultState();
    return;
  }
  QString nextToPlayPath = playlist[currentVideoIndex].toLocalFile();
  prepareVideoFile(nextToPlayPath);
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

void MainWindow::playbackPositionUpdated(qint64 playbackPosition) {
  controlbuttonslayout->setVideoSliderValue(playbackPosition); //syncing the slider with the player position

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

void MainWindow::changePlayBackPosition(int newpos) {
  // QMediaMetaData meta = player->metaData();
  // QVariant audioCodec = meta.value(QMediaMetaData::AudioCodec);

  // if(audioCodec.isValid()){
  //   float oldvol = audio->volume();
  //   player->pause();
  //   player->setAudioOutput(nullptr);
  //   delete audio;
  //   audio = new QAudioOutput();
  //   player->setPosition(newpos);
  //   player->setAudioOutput(audio);
  //   audio->setVolume(oldvol);
  //   if (!videoIsPaused){
  //     player->play();
  //     videoIsPaused = false;
  //   }

  // }else{
    player->setPosition(newpos);
  // }
}

void MainWindow::closeVideo() {
  setPlayerDefaultState();
  playlist.clear();
  currentPlayerMode = PlayerMode::Single;
}

void MainWindow::setPlayerDefaultState() {
  currentVideoUrl = "";
  videoIsPaused = true;
  player->setSource({});
  clearVector(audioTracksMetaDataVector);
  clearVector(subTracksMetaDataVector);
  killAllSubtitlesExtractionProcesses();
  controlbuttonslayout->hideSkipButton();
  controlbuttonslayout->setDefaultState();
  setVolumeSliderPosition(Settings["defaultVolume"] * 1000);
  ChaptersVectors.clear();
  updateButtonsIcon();
}

void MainWindow::savevideoposition() {
  if(currentVideoTitle.size() && player->position() > 5000){
    std::string LPPPath = (SYSTEMPATHS->cachePath / "playbackState").string();
    std::ofstream possavefile(LPPPath,std::ios::app);
    std::string line = currentVideoTitle+" ; "+QString::number(player->position()).toStdString()+'\n';
    possavefile<<line;
    possavefile.close();
  }
}

//load old position of a video if available
void MainWindow::getlastsavedposition() {
  if(currentVideoTitle == "") return;
  lastPlaybackPosition = 0;
  std::string LPPPath = (SYSTEMPATHS->cachePath / "playbackState").string();
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
    } else {
      controlbuttonslayout->hideSkipButton();
    }
  }else{
      std::cerr << "[ WARN ] Failed to open LPP  File: "<<LPPPath<<"\n";
  }
}

void MainWindow::setVideoSliderRange(qint64 playbackPosition) {
  controlbuttonslayout->setVideoSliderRange(0, playbackPosition); 
}

void MainWindow::setVolumeSliderPosition(qreal newPosition) {
  controlbuttonslayout->setVolumeSliderPosition(newPosition);
  updateVolumeButtonIcon();
  Settings["defaultVolume"] = (float) newPosition / 1000;
}
