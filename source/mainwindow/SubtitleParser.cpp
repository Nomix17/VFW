#include "../headers/main/mainwindow.h"
#include <QString>
#include <QStringList>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <sstream>

std::pair<double,double> parseTimeString(std::string stringTimeInterval);
double formatStringTime(const std::string& stringTimer);
bool stringIsInteger(std::string text);
bool lineIsEmpty(const std::string str);
bool lineContainsTimeArrow(std::string text);

void MainWindow::SubFileParsing(std::string subpath) {
  for (SubObject* ptr:currentLoadedSubList) delete ptr;
  currentLoadedSubList.clear();
  if(std::filesystem::path(subpath).extension().string() == ".ass" ||
     std::filesystem::path(subpath).extension().string() == ".ssa")
    parsingAssSubsFile(subpath);

  else parsingSrtLikeSubsFile(subpath);
}

void MainWindow::parsingAssSubsFile(std::string subpath) {
  std::ifstream file(subpath);
  if(!file){
    std::cerr << "[ WARN ] Cannot find .ass subtitle File: "<<subpath<<"\n";
    return;
  }

  std::string line;
  currentLoadedSubList.clear();
  int startTimeIndex=1; 
  int endTimeIndex=2;
  int textIndex=9;

  while(getline(file,line)){
    if (line.find("Format:") != std::string::npos) {
      line.erase(std::remove(line.begin(),line.end(),' '), line.end());
      line.erase(std::remove(line.begin(),line.end(),'\r'), line.end());
      QStringList splitedLine = QString::fromStdString(line).split(",");
      for(int i=0;i<splitedLine.size();i++){
        if(splitedLine[i] == "Start") startTimeIndex = i;
        if(splitedLine[i] == "End") endTimeIndex = i;
        if(splitedLine[i] == "Text") textIndex = i;
      }

    } else if(line.find("Dialogue:") != std::string::npos) {
      QStringList splitedDialogLine = QString::fromStdString(line).split(",");

      std::string subText="";
      for(int i = textIndex; i<splitedDialogLine.size(); i++){
        subText += splitedDialogLine[i].toStdString();
        subText += ",";
      }
      subText.erase(subText.size()-1);

      size_t pos = 0;
      while ((pos = subText.find("\\N", pos)) != std::string::npos) {
        subText.replace(pos, 2, "<br/>");
        pos += 1;
      }
      while ((pos = subText.find("\\n", pos)) != std::string::npos) {
        subText.replace(pos, 2, "<br/>");
        pos += 1;
      }

      double startTime = formatStringTime(splitedDialogLine[startTimeIndex].toStdString());
      double endTime = formatStringTime(splitedDialogLine[endTimeIndex].toStdString());

      SubObject * newSubObj = new SubObject;
      newSubObj->textContent = subText;
      newSubObj->startTime = startTime;
      newSubObj->endTime = endTime;
      currentLoadedSubList.push_back(newSubObj);
    }
  }
  file.close();
}

void MainWindow::parsingSrtLikeSubsFile(std::string subpath) {
  std::ifstream file(subpath);
  if(!file){
    std::cerr << "[ WARN ] Cannot open subtitle File: "<<subpath<<"\n";
    return;
  }

  std::string currentLine;
  SubObject *newSubObj = nullptr;

  // looping the lines in the file
  while (getline(file, currentLine)) {
    if (lineContainsTimeArrow(currentLine)) {
      std::pair <double,double> timesPair = parseTimeString(currentLine);
      double firstTime = timesPair.first;
      double secondTime = timesPair.second;

      if (newSubObj != nullptr)
        currentLoadedSubList.push_back(newSubObj);

      newSubObj = new SubObject;
      newSubObj->startTime = firstTime;
      newSubObj->endTime = secondTime;

    } else if(newSubObj != nullptr && !stringIsInteger(currentLine)) {
      currentLine.erase(std::remove(currentLine.begin(), currentLine.end(), '\r'), currentLine.end());

      if (newSubObj->textContent.empty()) {
        newSubObj->textContent = currentLine;

      } else if(!lineIsEmpty(currentLine)) {
        newSubObj->textContent += "<br/>" + currentLine;

      }
    }
  }

  if(newSubObj != nullptr)
    currentLoadedSubList.push_back(newSubObj);

  file.close();
}

std::pair<double,double> parseTimeString(std::string stringTimeInterval) {
  std::istringstream ss(stringTimeInterval);
  int h1, min1, sec1, milsec1, h2, min2, sec2, milsec2;
  std::string arrow;
  char step;
  ss>>h1>>step>>min1>>step>>sec1>>step>>milsec1;
  ss>>std::ws>>arrow>>std::ws;
  ss>>h2>>step>>min2>>step>>sec2>>step>>milsec2;

  // calculating the starting time from the string line (hour, minutes, seconds, ms)
  double firstTime = (h1*60*60 + min1*60 + sec1) * 1000 + milsec1;// ms

  // calculating the ending time from the string line (hour, minutes, seconds, ms)
  double secondTime = (h2*60*60 + min2*60 + sec2) * 1000 + milsec2; // ms
  return std::pair<double,double> (firstTime, secondTime);
}

double formatStringTime(const std::string& stringTimer) {
  std::istringstream ss(stringTimer);
  char step;
  int h, m, s, ms;
  ss >> h >> step >> m >> step >> s >> step >> ms;
  return (h*60*60 + m*60 + s) * 1000 + ms;
}

bool stringIsInteger(std::string text) {
  if(text.empty()) return false;
  for(char c : text){
    if(c < '0' || c > '9') {
      if(c != ' ' && c != '\r') return false;
    }
  }
  return true;
}

bool lineIsEmpty(const std::string str) {
  return str.empty() || std::all_of(str.begin(), str.end(), [](unsigned char c){
      return std::isspace(c);
  });
}

bool lineContainsTimeArrow(std::string text) {
  return (text.find("-->") != std::string::npos);
}
