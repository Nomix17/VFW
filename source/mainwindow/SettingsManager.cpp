#include "../headers/main/mainwindow.h"

void MainWindow::parseSettingsFile() {
  std::string settingsFilePath = (SYSTEMPATHS->configPath / "settings").string();
  std::ifstream file(settingsFilePath);
  std::string line;

  if(file) {
    while(std::getline(file,line)) {
      if(line.find(":") != std::string::npos){
        try{
          std::istringstream sstr(line);
          std::string temp;

          std::getline(sstr,temp,':');
          std::string item = temp;
          
          std::getline(sstr,temp);
          float value = std::stof(temp);

          Settings[item] = value;

        }catch(const std::invalid_argument& e){
          std::cout << "[ WARN ] Failed to fetch from Settings File: "<<e.what()<<"\n";
        }
      }
    }
  }else{
    std::cerr << "[ WARN ] Cannot find Settings File: "<<settingsFilePath<<"\n";
  }

}

void MainWindow::savingNewSettings() {
  std::string settingsFilePath = (SYSTEMPATHS->configPath / "settings").string();
  std::ofstream settingsFile(settingsFilePath);

  std::stringstream sstr;
  for(const auto& [key,value] : Settings){
    sstr << key << ":" << value << "\n";
  }
  std::string newSettings = sstr.str();
 
  settingsFile << newSettings;
  settingsFile.close();
}
