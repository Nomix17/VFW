#include "mainwindow.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <QApplication>
#include <QUrl>
#include <QString>

PATHS path;
QString homedir = path.homedir;
std::string projectdir = path.Projectdir();
std::string CONFIGSDIRECTORY = projectdir + "cache/configs/";
std::string FONTSDIRECTORY = projectdir + "cache/fonts/";
std::string theme = path.GETTHEME(CONFIGSDIRECTORY);
QString ICONSDIRECTORY = QString::fromStdString(projectdir + "cache/icons/"+theme+"/");
std::string STYLESDIRECTORY = projectdir + "cache/styles/"+theme+"/";

std::array <std::string,4> essentialDirectories = {
  CONFIGSDIRECTORY,
  FONTSDIRECTORY,
  ICONSDIRECTORY.toStdString(),
  STYLESDIRECTORY
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

  std::ifstream stylefile(STYLESDIRECTORY+"mainwindow.css");

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
      //add the paths to the playlist
      std::string fileExtention = std::filesystem::path(argv[i]).extension();
      if(std::find(supportedMediaFormats.begin(),supportedMediaFormats.end(),fileExtention) != supportedMediaFormats.end())
        w.playlist.push_back(QUrl(argv[i]));
    }
    //run the Playlist when the app open
    if(w.playlist.size()){
      w.mediaplayer("play a list");
    }else{
      w.mediaplayer("blackscreen");
    }
    w.resize(750,551);
    w.playertype = "playlist";
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
