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

int main(int argc,char* argv[]){
  QApplication a(argc, argv);
  std::ifstream stylefile(STYLESDIRECTORY+"mainwindow.css");

  //load style file#include <QFile>

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
      w.playlist.push_back(QUrl(argv[i]));
    }
    //run the Playlist when the app open
    w.mediaplayer("play a list");
    w.resize(750,551);
    w.playertype = "playlist";
  }
  a.installEventFilter(&w);
  return a.exec();
}
