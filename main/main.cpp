#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <iostream>
#include <QUrl>
#include <cstdlib>
#include <QString>
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>


int main(int argc,char* argv[]){
    QApplication a(argc, argv);
    PATHS path;
    std::string projectdir = path.Projectdir();
    std::ifstream stylefile(projectdir+"cache/styles/mainwindow.css");
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
    }
    return a.exec();
}