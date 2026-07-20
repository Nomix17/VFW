#include "../headers/main/mainwindow.h"
#include <QMediaMetaData>

void MainWindow::getSubtitleTracksFromMetaData() {
  clearVector(subTracksMetaDataVector);
  std::cout << "SUBTITLES TRACKS:\n";
  for (int i=0 ; i < player->subtitleTracks().size() ; i++) {

    QMediaMetaData subTrack = player->subtitleTracks()[i];
    std::string title = subTrack.stringValue(QMediaMetaData::Title).toStdString();
    std::string language = subTrack.stringValue(QMediaMetaData::Language).toStdString();

    if (!title.empty()) std::cout << "  Title: " <<title<< "\n";
    if (!language.empty()) std::cout << "  Language: " <<language<< "\n";

    if (!title.empty() || !language.empty()) {
      MetaDataTrack *subTrackMetaData = new MetaDataTrack;
      subTrackMetaData->title = title;
      subTrackMetaData->language = language;
      subTrackMetaData->index = i;
      subTracksMetaDataVector.push_back(subTrackMetaData);

    } else {
      std::cout << "  (No metadata available)\n";
    }
  }
}

void MainWindow::getAudioTracksFromMetaData() {
  clearVector(audioTracksMetaDataVector);
  std::cout << "AUDIO TRACKS:\n";
  for (int i=0 ; i < player->audioTracks().size() ; i++) {
    QMediaMetaData audioTrack = player->audioTracks()[i];
    std::string title = audioTrack.stringValue(QMediaMetaData::Title).toStdString();
    std::string language = audioTrack.stringValue(QMediaMetaData::Language).toStdString();
   
    if (!title.empty()) std::cout << "  Title: " << title<< "\n";
    if (!language.empty()) std::cout << "  Language: " <<language<< "\n";

    if (!title.empty() || !language.empty()) {
      MetaDataTrack *audioTrackMetaData = new MetaDataTrack;
      audioTrackMetaData->title = title;
      audioTrackMetaData->language = language;
      audioTrackMetaData->index = i;
      audioTracksMetaDataVector.push_back(audioTrackMetaData);

    } else {
      std::cout << "  (No metadata available)\n";
    }
  }
}
