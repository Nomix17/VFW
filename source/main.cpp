#include "./headers/main/mainwindow.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include <QApplication>
#include <QFileInfo>
#include <QUrl>
#include <QString>
#include <string>

std::vector<std::string> supportedMediaFormats = {
    ".mp4", ".mp3", ".mkv", ".avi", ".wav",
    ".mov", ".flac", ".m4a", ".webm", ".aac",
    ".wmv", ".ogg", ".m4v", ".mpg", ".mpeg",
    ".3gp", ".flv", ".ts", ".vob", ".opus",
    ".aiff", ".alac"
};

std::vector<std::string> supportedSubtitlesFormats = {
  ".srt",".vtt",".ass",".ssa"
};

void setAudioBackend();
void loadAndApplyTheme(QApplication&,std::string);

int main(int argc, char* argv[]) {
  setAudioBackend();
  QApplication app(argc, argv);

  MainWindow mainWindow;

  loadAndApplyTheme(app, mainWindow.SYSTEMPATHS->currentThemeDir);
  mainWindow.show();

  // Process command-line arguments
  QStringList args = QCoreApplication::arguments();
  for (int i = 1; i < args.size(); ++i) {
    QString arg = args[i];
    QString fileExtension = QFileInfo(arg).suffix();

    std::string extWithDot = "." + fileExtension.toStdString();

    if (std::find(supportedMediaFormats.begin(), supportedMediaFormats.end(), extWithDot)
      != supportedMediaFormats.end()) {
      QUrl videoPath = QUrl::fromLocalFile(arg);
      mainWindow.playlist.push_back(videoPath);
      std::cout << "Loading Video: " << arg.toUtf8().constData() << "\n";

    } else if (std::find(supportedSubtitlesFormats.begin(), supportedSubtitlesFormats.end(), extWithDot)
      != supportedSubtitlesFormats.end()) {
      mainWindow.currentVideoSubtitlePaths.push_back(arg);
      std::cout << "Loading Subtitle: " << arg.toUtf8().constData() << "\n";
    }
  }

  // Run playlist or set default state
  if (!mainWindow.playlist.empty()) {
    mainWindow.playNextVideoInPlaylist();
  } else {
    mainWindow.setPlayerDefaultState();
  }

  mainWindow.resize(750, 551);
  mainWindow.currentPlayerMode = MainWindow::PlayerMode::Playlist;

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

void setAudioBackend() {
#ifndef _WIN32
    setenv("QT_AUDIO_BACKEND", "pulseaudio", 1);
#endif
}
