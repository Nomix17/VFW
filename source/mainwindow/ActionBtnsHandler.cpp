#include "../headers/main/mainwindow.h"
#include <QFileDialog>
#include <QApplication>
#include <QAction>
#include <QDialog>
#include <QString>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iostream>

void MainWindow::onToolMenuAction(int actionNumber) {
  QString url;
  switch (actionNumber) {
    case ToolMenu::Open_file: {
      QString displaydir;
      exitFullScreen();
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
        prepareVideoFile(url);
        currentPlayerMode = PlayerMode::Single;
      }
      playlist.clear();
      break;
    }

    case ToolMenu::Open_folder: {
      QString displaydir;
      exitFullScreen();
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

    case ToolMenu::Open_media: {
      UrlWindow x(SYSTEMPATHS->currentThemeDir);
      exitFullScreen();
      x.exec();
      url = x.url;
      if (!url.isEmpty()) {
        playlist.clear();
        currentPlayerMode = PlayerMode::Single;
        prepareVideoURL(url);
      }
      break;
    }

    case ToolMenu::Quit: {
      QApplication::quit();
      break;
    }

    case ToolMenu::JUMP_BACKWARD: {
      changePlayBackPosition(player->position() - 5000);
      break;
    }

    case ToolMenu::JUMP_FORWARD: {
      changePlayBackPosition(player->position() + 5000);
      break;
    }

    case ToolMenu::JUMP_TO_TIME: {
      JumpTime x(SYSTEMPATHS->currentThemeDir);
      exitFullScreen();
      x.exec();
      if (x.targettime >= 0) {
        changePlayBackPosition(x.targettime * 1000);
      }
      break;
    }

    case ToolMenu::JUMP_TO_NEXT_CHAP:{
      moveToNextChapter();
      break;
    }

    case ToolMenu::JUMP_TO_PREV_CHAP:{
      moveToPrevChapter();
      break;
    }

    case ToolMenu::TOGGLE_LOOPSEGMENT: {
      QAction * toggleLoopAction = toolMenuActionsObjectsList[ToolMenu::TOGGLE_LOOPSEGMENT];
      if(!currentVideoUrl.isEmpty()){
        if(!segmentLoopEnabled){
          SRepeatWindow win(SYSTEMPATHS->currentThemeDir);
          exitFullScreen();
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

    case ToolMenu::AUDIO_TRACKS: {
      static int audioTrackIndex = 0;
      AudioTracksManager audioTracksManagerWindow(SYSTEMPATHS->currentThemeDir, SYSTEMPATHS->currentIconsDir, audioTracksMetaDataVector, audioTrackIndex);
      exitFullScreen();
      audioTracksManagerWindow.exec();
      if(audioTracksManagerWindow.newAudioTrackIndex >= 0) {
        audioTrackIndex = audioTracksManagerWindow.newAudioTrackIndex;
        player->setActiveAudioTrack(audioTrackIndex);
      }
      break;
    }

    case ToolMenu::FULL_VOLUME: {
      setVolumeSliderPosition(1000);
      break;
    }

    case ToolMenu::MUTE: {
      setVolumeSliderPosition(0);
      break;
    }

    case ToolMenu::TOGGLE_ASPRadio: {
      QAction * toggleFillAction = toolMenuActionsObjectsList[ToolMenu::TOGGLE_ASPRadio];
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

    case ToolMenu::TOGGLE_SUB: {
      QAction * ToggleSubs = toolMenuActionsObjectsList[ToolMenu::TOGGLE_SUB];
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
        subtitlesItem->setOpacity(0);
        currentLoadedSubList.clear();
        currentLoadedSubPath = "";
        ToggleSubs->setText("Add Subtitles");
      }
      break;
    } 

    case ToolMenu::LOADSUBTITLES:{
      subWindow subWin(SYSTEMPATHS->currentThemeDir,SYSTEMPATHS->currentIconsDir,currentVideoSubtitlePaths,currentLoadedSubPath);
      exitFullScreen();
      subWin.exec();
      if(!subWin.clickedSubPath.isEmpty()){
        currentLoadedSubPath = subWin.clickedSubPath;
        subtitlesItem->setOpacity(0);
        currentLoadedSubList.clear();
        SubFileParsing(currentLoadedSubPath.toStdString());
        std::cout<<"[ INFO ] Subtitles were Loaded: "<<currentLoadedSubPath.toStdString()<<"\n";
        QAction * ToggleSubs = toolMenuActionsObjectsList[ToolMenu::TOGGLE_SUB];
        ToggleSubs->setText("Remove Subtitles");
      }
      break;
    }

    case ToolMenu::TOGGLE_SUBDISPLAY: {
      QAction * ToggleSubsDisplay = toolMenuActionsObjectsList[ToolMenu::TOGGLE_SUBDISPLAY];
      toggleSubtitles();
      if(ShowSubs){
        ToggleSubsDisplay->setText("Hide Subtitles");
      }else{
        ToggleSubsDisplay->setText("Display Subtitles");
      }
      break;
    }

    case ToolMenu::TOGGLE_CHAPTERSINDICATORS: {
      QAction * toggleChapters = toolMenuActionsObjectsList[ToolMenu::TOGGLE_CHAPTERSINDICATORS];
      toggleChaptersIndicators();
      if(showChaptersIndicators){
        toggleChapters->setText("Hide Chapters Indicators");
      }else{
        toggleChapters->setText("Display Chapters Indicators");
      }
      break;
    }

    case ToolMenu::ADDDELAY: {
      increaseSubtitlesDelay();
      break;
    }

    case ToolMenu::REDUCEDELAY: {
      decreaseSubtitlesDelay();
      break;
    }

    case ToolMenu::SUBSETTINGS: {
      SubConfig win(
        SYSTEMPATHS->configPath,
        SYSTEMPATHS->fontsDir,
        SYSTEMPATHS->currentThemeDir
      );
      exitFullScreen();
      win.gui();
      win.exec();
      TextItem::setHtmlTemplate(win.makehtml());
      TextItem::setBottomMargin(win.marginbottom);
      syncSubtitles(player->position());
      break;
    }

    case ToolMenu::TITLE: {
      if (currentVideoTitle.size()) {
        renderOverlayText(currentVideoTitle, TextPosition::BOTTOM, 3000);
      }
      break;
    }

    case ToolMenu::THEME:{
      ChangeThemeWindow win(
        SYSTEMPATHS->configPath,
        SYSTEMPATHS->themesDir,
        SYSTEMPATHS->currentThemeDir
      );
      exitFullScreen();
      win.exec();
      if(win.changetotheme!="") {
        std::string text = "You Need to reset the Application to apply the new Theme";
        renderOverlayText(text, TextPosition::CENTER, 4000);
      }
      break;
    }

    case ToolMenu::SHORTCUTS: {
      ShortcutsInst win(
        nullptr,
        SYSTEMPATHS->currentThemeDir,
        SYSTEMPATHS->configPath
      );
      exitFullScreen();
      if(win.exec() == QDialog::Accepted) {
        currentShortcuts = win.currentShortcuts;
        setupShortcuts();
      }
      break;
    }

  }
}

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
      PlaylistManager win(
        SYSTEMPATHS->currentThemeDir,
        SYSTEMPATHS->currentIconsDir,
        playlist,
        currentVideoUrl
      );
      exitFullScreen();
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
