#include "mainwindow.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <QApplication>
#include <QUrl>
#include <QString>
#include <string>

PATHS path;
QString homedir = path.homedir;
std::string projectdir = path.Projectdir();
std::string CONFIGSDIRECTORY = projectdir + "/assets/configs/";
std::string FONTSDIRECTORY = projectdir + "/assets/fonts/";
std::string theme = path.GETTHEME(CONFIGSDIRECTORY);
QString ICONSDIRECTORY = QString::fromStdString(projectdir + "/assets/icons/"+theme+"/");
std::string STYLESDIRECTORY = projectdir + "/assets/styles/";
std::string THEMEDIRECTORY = STYLESDIRECTORY+theme+"/";

std::array <std::string,5> essentialDirectories = {
  CONFIGSDIRECTORY,
  FONTSDIRECTORY,
  ICONSDIRECTORY.toStdString(),
  STYLESDIRECTORY,
  THEMEDIRECTORY
};

std::vector<std::string> supportedMediaFormats = {
    ".mp4", ".mkv", ".avi", ".mov", ".webm", ".wmv", ".m4v",
    ".mp3", ".wav", ".aac", ".m4a", ".wma", ".ogg"
};
std::vector<std::string> supportedSubtitlesFormats = {
  ".srt",".ass"
};

void createMissingDirectories();

int main(int argc,char* argv[]){
  QApplication a(argc, argv);
  createMissingDirectories();

  std::ifstream stylefile(THEMEDIRECTORY+"mainwindow.css");

  if(stylefile){
    std::string script;
    std::ostringstream ssrt;
    ssrt << stylefile.rdbuf();
    script = ssrt.str();
    a.setStyleSheet(QString::fromStdString(script));
  }

  MainWindow w;
  w.show();
  //if the user pass video paths as arguments
  if(argc>1){

    for(int i=1;i<argc;i++){
      std::filesystem::path path = std::filesystem::absolute(argv[i]);
      std::string fileExtention = path.extension().string();

      if (std::find(supportedMediaFormats.begin(), supportedMediaFormats.end(), fileExtention) != supportedMediaFormats.end()) {
        QUrl videoPath = QUrl::fromLocalFile(QString::fromLocal8Bit(path.c_str()));
        w.playlist.push_back(videoPath);
        std::cout << "Loading Video: " << path.string() << "\n";

      } else if (std::find(supportedSubtitlesFormats.begin(), supportedSubtitlesFormats.end(), fileExtention) != supportedSubtitlesFormats.end()) {
        QString subPath = QString::fromLocal8Bit(path.c_str());
        w.currentVideoSubtitlePaths.push_back(subPath);
        std::cout << "Loading Subtitle: " << path.string() << "\n";
      }
    }

    //run the Playlist when the app open
    if(w.playlist.size()){
      w.playNextVideoInPlaylist();
    }else{
      w.setPlayerDefaultState();
    }
    w.resize(750,551);
    w.currentPlayerMode = MainWindow::PlayerMode::Playlist;
  }
  a.installEventFilter(&w);
  return a.exec();
}

void createMissingDirectories(){
  // create all the essential directoies
  for(const std::string& dirPath : essentialDirectories){
    bool created = std::filesystem::create_directories(dirPath);
    if(created)
      std::cerr << "[ WARNING ] Missing directory created: " << dirPath << "\n";
  }
}
