#include "../headers/main/mainwindow.h"
#include <QString>
#include <QStringList>
#include <QProcess>
#include <filesystem>
#include <iostream>

void MainWindow::ExtractingChapterData(QString currenturl) {
  ChaptersVectors.clear();
  QProcess *ChaptersProcess = new QProcess(this);
  QString ffprobePath = QString::fromStdString(SYSTEMPATHS->ffprobeBinPath);
  QStringList processArgs = { "-i",currenturl,"-v", "quiet", "-show_chapters" };

  connect(ChaptersProcess, &QProcess::finished, this, [this, ChaptersProcess]() {
    QStringList Chapters = QString(ChaptersProcess->readAllStandardOutput()) 
      .split("[CHAPTER]",Qt::SkipEmptyParts);

    std::cout<<"[ INFO ] Number Of Chapters Detected: "<<Chapters.size()<<"\n";

    for(int i=0;i<Chapters.size();i++){
      QStringList chapterStreams = QString(Chapters[i]).split("\n");
      ChapterObject newChapter; 
      newChapter.title = "Unknown";
      for(int j=0;j<chapterStreams.size();j++){
        if(chapterStreams[j] != "[/CHAPTER]"){
          QStringList SplitedLine = QString(chapterStreams[j]).split("=");
          if (SplitedLine.size() < 2) continue;
          if(SplitedLine[0] == "id") newChapter.id = SplitedLine[1];
          else if(SplitedLine[0] == "TAG:title") newChapter.title = SplitedLine[1];
          else if(SplitedLine[0] == "start_time") newChapter.startTime = SplitedLine[1].toFloat();
          else if(SplitedLine[0] == "end_time") newChapter.endTime = SplitedLine[1].toFloat();
        }
      }
      ChaptersVectors.push_back(newChapter);
    }
    controlbuttonslayout->setChaptersMarks(ChaptersVectors, showChaptersIndicators);
    ChaptersProcess->deleteLater();
  });

  ChaptersProcess->start(ffprobePath, processArgs);
}

void MainWindow::ExtractingBuiltInSubs(QString currenturl) {
  QString ffprobePath = QString::fromStdString(SYSTEMPATHS->ffprobeBinPath);
  QProcess lookForSubs;
  lookForSubs.setProgram(ffprobePath);
  lookForSubs.setArguments({
    "-v", "error",
    "-select_streams", "s",
    "-show_entries", "stream=index",
    "-of", "csv=p=0",
    currenturl
  });
  lookForSubs.start();
  lookForSubs.waitForFinished(-1);

  QStringList subStreams = QString(lookForSubs.readAllStandardOutput())
    .split("\n", Qt::SkipEmptyParts);

  std::cout<<"[ INFO ] Number Of Subs Detected: "<<subStreams.size()<<"\n";
  killAllSubtitlesExtractionProcesses();

  for(int i = 0; i < subStreams.size(); i++) {
    std::filesystem::path currentVideoPath(currenturl.toStdString());
    std::string subPathWithoutExtension = (SYSTEMPATHS->tmpPath / std::filesystem::path(currentVideoPath.stem())).string();
    if (!std::filesystem::exists(SYSTEMPATHS->tmpPath) || !std::filesystem::is_directory(SYSTEMPATHS->tmpPath)) {
      if(!std::filesystem::create_directories(SYSTEMPATHS->tmpPath)) return;
    }

    QString fileSubPath = QString("%1_Sub%2.srt").arg(QString::fromStdString(subPathWithoutExtension)).arg(i);
    QString subId = QString("0:s:%1").arg(i);

    int currentProcessIndex = i;
    QString ffmpegPath = QString::fromStdString(SYSTEMPATHS->ffmpegBinPath);
    QProcess* ffmpegProcess = new QProcess(this);
    ffmpegProcess->setProgram(ffmpegPath);
    ffmpegProcess->setArguments({
      "-y",
      "-i", currenturl,
      "-map", subId, fileSubPath
    });
    connect(
      ffmpegProcess,
      QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      ffmpegProcess,
      [this, currentProcessIndex, fileSubPath]() {
        onffmpegProcessFinshed(currentProcessIndex, fileSubPath);
      }
    );
    subExtractionProcessesList.push_back(ffmpegProcess);
  }

  if(!subExtractionProcessesList.empty())
    subExtractionProcessesList[0]->start();
}

void MainWindow::onffmpegProcessFinshed(int currentProcessIndex, QString fileSubPath) {
  currentVideoSubtitlePaths.push_back(fileSubPath);
  std::cout <<"[ INFO ] Extract subtitle to: " << fileSubPath.toStdString() <<"\n";

  if(subExtractionProcessesList[currentProcessIndex] != nullptr) {
    subExtractionProcessesList[currentProcessIndex]->deleteLater();
    subExtractionProcessesList[currentProcessIndex] = nullptr;
  }
  if(currentProcessIndex == 0)
    setCurrentLoadedSubPath(fileSubPath);
  if(subExtractionProcessesList.size() > currentProcessIndex + 1) {
    if(subExtractionProcessesList[currentProcessIndex + 1] != nullptr)
      subExtractionProcessesList[currentProcessIndex + 1]->start();
  } else {
    clearVector(subExtractionProcessesList);
  }
}

void MainWindow::killAllSubtitlesExtractionProcesses() {
  for(QProcess* process : subExtractionProcessesList) {
    if(process == nullptr) continue;
    process->disconnect();
    process->terminate();
    if (!process->waitForFinished(1000)) {
      process->kill();
    }
    process->deleteLater();
  }
  subExtractionProcessesList.clear();
}
