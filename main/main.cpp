#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <iostream>
#include <QUrl>

int main(int argc,char* argv[]){
    QApplication a(argc, argv);
    QFile file("/home/pain/.config/VFW/cache/styles/style.css");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QString script = file.readAll();
        a.setStyleSheet(script);
        file.close();
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