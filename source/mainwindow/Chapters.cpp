#include "../headers/main/mainwindow.h"
#include <algorithm>

void MainWindow::moveToNextChapter(){
  int currentChapIndex = findCurrentChapterIndex();
  if(currentChapIndex < 0) return;
  ChapterObject newChapter = ChaptersVectors[std::min(static_cast<size_t>(currentChapIndex + 1), ChaptersVectors.size() - 1)];
  changePlayBackPosition(newChapter.startTime * 1000 + 1);
  renderOverlayText(newChapter.title.toStdString(), TextPosition::TOP_LEFT, 2000); 
}

void MainWindow::moveToPrevChapter(){
  int currentChapIndex = findCurrentChapterIndex();
  if(currentChapIndex < 0) return;
  ChapterObject newChapter = ChaptersVectors[std::max(0, currentChapIndex - 1)];
  changePlayBackPosition(newChapter.startTime * 1000 + 1);
  renderOverlayText(newChapter.title.toStdString(), TextPosition::TOP_LEFT, 2000); 
}

void MainWindow::toggleChaptersIndicators(){
  showChaptersIndicators =!showChaptersIndicators;
  std::string displayMessage = showChaptersIndicators ? "Chapters Indicators On" : "Chapters Indicators Off";
  renderOverlayText(displayMessage, TextPosition::CENTER, 2000);
  controlbuttonslayout->setChaptersMarks(ChaptersVectors, showChaptersIndicators);
}

int MainWindow::findCurrentChapterIndex() {
  return findChapterIndexByTime(player->position());
}

int MainWindow::findChapterIndexByTime(int time) {
  for(size_t i=0;i<ChaptersVectors.size();i++) {
    if(
      time >= ChaptersVectors[i].startTime * 1000 &&
      time <= ChaptersVectors[i].endTime * 1000
    ) {
      return i;
    }
  }
  return -1;
}
