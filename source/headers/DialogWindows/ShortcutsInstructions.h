#ifndef SHORTCUTS_INST
#define SHORTCUTS_INST

#include <algorithm>
#include <array>
#include <cctype>
#include <ios>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QLineEdit>
#include <QStyle>
#include <QPointer>
#include <QTimer>
#include <QString>
#include <QKeySequence>

#include "../utils/stringTreatment.h"

struct ShortcutObj {
  QString displayName;
  QString keyMapName;
  QLineEdit* lineEdit;
};

class ShortcutsInst:public QDialog {
  Q_OBJECT;
  private:
    std::filesystem::path shortcutsFilePath;
    QVBoxLayout *mainlayout = new QVBoxLayout(this);
    QGridLayout *tablelayout = new QGridLayout;
    QHBoxLayout *btnsLayout = new QHBoxLayout;
    QPushButton *saveBtn = new QPushButton("Save");
    QPushButton *cancelBtn = new QPushButton("Cancel");
    std::vector<ShortcutObj> shortcutObjs = {};
    QTimer* timer = nullptr;

  std::vector<QString> bannedKeys = {
    "Control",
    "Meta",
    "Alt",
    "CapsLock",
    "Shift",
    "ScrollLock",
    "NumLock",
  };

  public:
    std::map<std::string, QKeySequence> currentShortcuts;
    ShortcutsInst(QWidget *parent,std::string StyleDirectory,std::filesystem::path configPath):QDialog(parent){
      this->setWindowTitle("Shortcuts");

      //load style file
      std::filesystem::path styleFullPath(std::filesystem::path(StyleDirectory) / "shortcutsinstructions.css");
      std::ifstream stylefile(styleFullPath);
      if(stylefile){
        std::ostringstream sstr;
        std::string script;
        sstr<<stylefile.rdbuf();
        script = sstr.str();
        this->setStyleSheet(QString::fromStdString(script));
        stylefile.close();
      }

      shortcutsFilePath = (configPath / "Shortcuts_Instructions").string();    
      std::vector<std::string> shortcutRows = loadShortcutsFromFile(configPath);
      createShortcutsTable(shortcutRows);

      saveBtn->setFocusPolicy(Qt::NoFocus);
      cancelBtn->setFocusPolicy(Qt::NoFocus);

      connect(saveBtn,&QPushButton::clicked,[this](){
        saveCurrentShortcuts();
        QDialog::accept();
      });
      connect(cancelBtn,&QPushButton::clicked,[this](){
        QDialog::close();
      });

      btnsLayout->addWidget(saveBtn);
      btnsLayout->addWidget(cancelBtn);

      mainlayout->addLayout(tablelayout);
      mainlayout->addLayout(btnsLayout);
      setLayout(mainlayout);
    }

    static std::vector <std::string> loadShortcutsFromFile(std::filesystem::path configPath) {
      std::filesystem::path shortcutsFilePath = (configPath / "Shortcuts_Instructions").string();
      std::ifstream shortcutfile(shortcutsFilePath);
      std::string shortcutsfilecontent;
      std::vector <std::string> shortcutRows;

      if (shortcutfile) {
        std::string line;
        while(getline(shortcutfile,line)){
          shortcutRows.push_back(line);
        }

      } else {
        //put the default configs into the vector
        shortcutRows = {
          "Pause/Unpause:TOGGLE_PAUSE:Space",
          "Seek Forward (+5s):SEEK_FORWARD:Right",
          "Seek Backward (-5s):SEEK_BACKWARD:Left",
          "Volume Up:VOLUME_UP:Up",
          "Volume Down:VOLUME_DOWN:Down",
          "Toggle Fullscreen:TOGGLE_FULLSCREEN:F",
          "Exit Fullscreen:EXIT_FULLSCREEN:Esc",
          "Display Title:DISPLAY_TITLE:T",
          "Toggle Subtitles:TOGGLE_SUBTITLES:V",
          "Toggle Chapter Indicators:TOGGLE_CHAPTER_INDICATORS:C",
          "Next Chapter:NEXT_CHAPTER:N",
          "Previous Chapter:PREVIOUS_CHAPTER:B",
          "Mute/Unmute:TOGGLE_MUTE:M",
          "Reduce Subtitles Delay:REDUCE_SUBTITLES_DELAY:G",
          "Increase Subtitles Delay:INCREASE_SUBTITLES_DELAY:H",
        };
        writeShortcusIntoFile(shortcutRows, shortcutsFilePath);
      }
      return shortcutRows;
    }

    static void writeShortcusIntoFile(std::vector<std::string> shortcutsRows, std::filesystem::path shortcutsFilePath) {
      std::ofstream defaultshortcutsfile(shortcutsFilePath);
      for(size_t i=0;i<shortcutsRows.size();i++) 
        defaultshortcutsfile << shortcutsRows[i]+'\n';
      defaultshortcutsfile.close();
    }

    void createShortcutsTable(std::vector<std::string> shortcutRows) {
      //creating table header 
      QLabel* headerFunctionalityLabel = new QLabel(this);
      QLabel* headerShortcutKey = new QLabel(this);
      headerFunctionalityLabel->setObjectName("tablehead");
      headerShortcutKey->setObjectName("tablehead");
      headerFunctionalityLabel->setText("Actions");
      headerShortcutKey->setText("Shortcuts");
      headerFunctionalityLabel->setAlignment(Qt::AlignCenter);
      headerShortcutKey->setAlignment(Qt::AlignCenter);
      tablelayout->addWidget(headerFunctionalityLabel,0,0);
      tablelayout->addWidget(headerShortcutKey,0,1);

      //rest of the table
      for(int i = 0; i < shortcutRows.size(); i++) {
        auto [shortcutDisplayName, shortcutKeyName, shortcutKey] = parseShortcutRow(shortcutRows[i]);
        QLabel *functionalityLabel = new QLabel(QString::fromStdString(shortcutDisplayName));
        QLineEdit *keyLineEdit = new QLineEdit(this);
        keyLineEdit->setObjectName("KeyEditInput");
        keyLineEdit->setText(QString::fromStdString(shortcutKey));
        keyLineEdit->installEventFilter(this);
        keyLineEdit->setReadOnly(true);
        keyLineEdit->setAlignment(Qt::AlignCenter);

        tablelayout->addWidget(functionalityLabel, i + 1, 0);
        tablelayout->addWidget(keyLineEdit, i + 1, 1);

        ShortcutObj newShortcutObj;
        newShortcutObj.displayName = QString::fromStdString(shortcutDisplayName);
        newShortcutObj.keyMapName = QString::fromStdString(shortcutKeyName);
        newShortcutObj.lineEdit = keyLineEdit;
        shortcutObjs.push_back(newShortcutObj);
      }
    }

    bool eventFilter(QObject *watched, QEvent *event) {
      if (event->type() == QEvent::FocusOut) {
        QLineEdit* lineEditObj = static_cast<QLineEdit*>(watched); 
        lineEditObj->setProperty("state", "");
        lineEditObj->style()->unpolish(lineEditObj);
        lineEditObj->style()->polish(lineEditObj);
      }

      if (event->type() == QEvent::KeyPress) {
        QLineEdit* lineEditObj = static_cast<QLineEdit*>(watched); 
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QString stringfiedKey = QKeySequence(keyEvent->key()).toString(QKeySequence::PortableText);

        if(std::find(bannedKeys.begin(), bannedKeys.end(), stringfiedKey) == bannedKeys.end()) {
          lineEditObj->setText(stringfiedKey);
        } else {
          lineEditObj->setProperty("state", "error");
          if (timer != nullptr) delete timer;
          timer = new QTimer(this);
          timer->setSingleShot(true);

          QPointer<QLineEdit> safeLineEdit(lineEditObj);
          connect(timer, &QTimer::timeout, this, [this, lineEditObj, safeLineEdit]() {
            if(safeLineEdit) {
              safeLineEdit->setProperty("state", "");
              safeLineEdit->style()->unpolish(safeLineEdit);
              safeLineEdit->style()->polish(safeLineEdit);
            }
          });

          timer->start(200);
          lineEditObj->style()->unpolish(lineEditObj);
          lineEditObj->style()->polish(lineEditObj);
        }

        return true;
      }
      return QObject::eventFilter(watched, event);
    }

    void saveCurrentShortcuts() {
      std::vector<std::string> stringShortcutRows = {};

      for(ShortcutObj shortcut : shortcutObjs) {
        std::string shortcutDisplayName = shortcut.displayName.toStdString();
        std::string shortcutKeyName = shortcut.keyMapName.toStdString();
        QString shortcutKeyAsString = shortcut.lineEdit->text();

        stringShortcutRows.push_back(shortcutDisplayName + ":" + shortcutKeyName + ":" + shortcutKeyAsString.toStdString());
        currentShortcuts[shortcutKeyName] = QKeySequence(shortcutKeyAsString, QKeySequence::PortableText);
      }
      writeShortcusIntoFile(stringShortcutRows, shortcutsFilePath);
    }

    static std::array<std::string, 3> parseShortcutRow(std::string row) {
      std::stringstream line(row);
      std::string shortcutDisplayName;
      std::string shortcutKeyName;
      std::string shortcutKeyAsString;
      std::getline(line, shortcutDisplayName, ':');
      std::getline(line, shortcutKeyName, ':');
      std::getline(line, shortcutKeyAsString, ':');
      trim(shortcutDisplayName);
      trim(shortcutKeyName);
      trim(shortcutKeyAsString);
      return { shortcutDisplayName, shortcutKeyName, shortcutKeyAsString };
    }

    static std::map<std::string, QKeySequence> getShortcutMap(std::filesystem::path configPath) {
      std::map<std::string, QKeySequence> currentShortcuts = {};
      std::vector<std::string> shortcutRows = loadShortcutsFromFile(configPath);
      for(int i = 0; i < shortcutRows.size(); i++){
        auto [shortcutDisplayName, shortcutKeyName, shortcutKeyAsString] = parseShortcutRow(shortcutRows[i]);
        currentShortcuts[shortcutKeyName] = QKeySequence(QString::fromStdString(shortcutKeyAsString), QKeySequence::PortableText);
      }
      return currentShortcuts;
    }
};

#endif
