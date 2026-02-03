#include "mainwindow.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <QApplication>
#include <QUrl>
#include <QString>
#include <string>

std::vector<std::string> supportedMediaFormats = {
    ".mp4", ".mkv", ".avi", ".mov", ".webm", ".wmv", ".m4v",
    ".mp3", ".wav", ".aac", ".m4a", ".wma", ".ogg"
};
std::vector<std::string> supportedSubtitlesFormats = {
  ".srt",".vtt",".ass",".ssa"
};

void loadAndApplyTheme(QApplication&,std::string);

int main(int argc,char* argv[]){
  QApplication app(argc, argv);

  MainWindow mainWindow;

  loadAndApplyTheme(app, mainWindow.SYSTEMPATHS->currentThemeDir);
  mainWindow.show();

  //if the user pass video paths as arguments
  if(argc>1){

    for(int i=1;i<argc;i++){
      std::filesystem::path path = std::filesystem::absolute(argv[i]);
      std::string fileExtention = path.extension().string();

      if (std::find(supportedMediaFormats.begin(), supportedMediaFormats.end(), fileExtention) != supportedMediaFormats.end()) {
        QUrl videoPath = QUrl::fromLocalFile(QString::fromStdString(path.string()));
        mainWindow.playlist.push_back(videoPath);
        std::cout << "Loading Video: " << path.string() << "\n";

      } else if (std::find(supportedSubtitlesFormats.begin(), supportedSubtitlesFormats.end(), fileExtention) != supportedSubtitlesFormats.end()) {
        QString subPath = QString::fromStdString(path.string());
        mainWindow.currentVideoSubtitlePaths.push_back(subPath);
        std::cout << "Loading Subtitle: " << path.string() << "\n";
      }
    }

    //run the Playlist when the app open
    if(mainWindow.playlist.size()){
      mainWindow.playNextVideoInPlaylist();
    }else{
      mainWindow.setPlayerDefaultState();
    }
    mainWindow.resize(750,551);
    mainWindow.currentPlayerMode = MainWindow::PlayerMode::Playlist;
  }

  app.installEventFilter(&mainWindow);
  return app.exec();
}

void loadAndApplyTheme(QApplication& app, std::string themeDirPath) {
  std::ifstream stylefile(themeDirPath+"/mainwindow.css");

  if(stylefile){
    std::string script;
    std::ostringstream ssrt;
    ssrt << stylefile.rdbuf();
    script = ssrt.str();
    app.setStyleSheet(QString::fromStdString(script));
  }
}
