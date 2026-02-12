QT += core widgets multimedia multimediawidgets
CONFIG += c++17 debug
SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h \
           MediaUrl.h \
           JumpToTime.h \
           LoadedSubsWindow.h \
           SRepeatWindow.h \
           CustomObjects.h \
           ShortcutsInstructions.h \
           PlaylistManager.h \
           ChangeThemeWindow.h \
           SubsConfigsWindow.h \
           TopBar.h \
           TopBarButton.h \
           BottomControlPanel.h \
           Paths.h

RC_FILE = app_icon.rc
