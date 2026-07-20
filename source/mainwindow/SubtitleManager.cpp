#include "../headers/main/mainwindow.h"
#include <QAction>
#include <QString>
#include <filesystem>
#include <algorithm>
#include <iostream>

void MainWindow::syncSubtitles(qint64 playbackPosition) {
  for(const SubObject* subObj : currentLoadedSubList){
    bool hasPlaybackPassedSubStart = (playbackPosition >= (subObj->startTime + currentSubDelay));
    bool isPlaybackBeforeSubEnd = (playbackPosition <= (subObj->endTime + currentSubDelay));

    if (ShowSubs && hasPlaybackPassedSubStart && isPlaybackBeforeSubEnd) {
      subtitlesItem->setContent(QString::fromStdString(subObj->textContent));
      subtitlesItem->setOpacity(1);
      subtitlesItem->repositionText();
      return; 
    }
  }
  subtitlesItem->clearContent();
}

void MainWindow::toggleSubtitles() {
  ShowSubs =! ShowSubs;
  if(!ShowSubs) subtitlesItem->clearContent();
  std::string displayMessage = ShowSubs ? "Subtitles On" : "Subtitles Off";
  renderOverlayText(displayMessage, TextPosition::CENTER, 2000);
}

void MainWindow::increaseSubtitlesDelay() {
  if (currentLoadedSubList.size()) {
    currentSubDelay += 100;
    if (-100 < currentSubDelay && currentSubDelay < 100)
      currentSubDelay = 0;
    std::string text = "Subtitles Delay: " + std::to_string((int)(currentSubDelay)) + " ms";
    renderOverlayText(text, TextPosition::CENTER, 1000);
  }
}

void MainWindow::decreaseSubtitlesDelay() {
  if (currentLoadedSubList.size()) {
    currentSubDelay -= 100;
    if (-100 < currentSubDelay && currentSubDelay < 100)
      currentSubDelay = 0;
    std::string text = "Subtitles Delay: " + std::to_string((int)(currentSubDelay)) + " ms";
    renderOverlayText(text, TextPosition::CENTER, 1000);
  }
}

void MainWindow::cleanSubtitles() {
  QAction * ToggleSubs = toolMenuActionsObjectsList[ToolMenu::TOGGLE_SUB];
  ToggleSubs->setText("Add Subtitles");
  currentVideoSubtitlePaths.clear();
  currentLoadedSubPath = "";
}

void MainWindow::LoadingInDirectorySubtitles(QString currenturl) {
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
            QAction * ToggleSubs = toolMenuActionsObjectsList[ToolMenu::TOGGLE_SUB];
            ToggleSubs->setText("Remove Subtitles");
          }
        }
      }
    }
  }
}

void MainWindow::setCurrentLoadedSubPath(QString fileSubPath) {
  currentLoadedSubPath = fileSubPath;
  SubFileParsing(currentLoadedSubPath.toStdString());
  QAction * ToggleSubs = toolMenuActionsObjectsList[ToolMenu::TOGGLE_SUB];
  ToggleSubs->setText("Remove Subtitles");
}

void MainWindow::loadSubtitleStyle() {
  SubConfig win(SYSTEMPATHS->configPath, SYSTEMPATHS->fontsDir, SYSTEMPATHS->currentThemeDir);
  win.loadFonts();
  TextItem::setHtmlTemplate(win.makehtml());
  TextItem::setBottomMargin(win.marginbottom);
}
