QT += core widgets multimedia multimediawidgets
CONFIG += c++17 debug
SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += ./headers/main/mainwindow.h \
           ./headers/main/CustomSlider.h \
           ./headers/main/Paths.h \
           ./headers/DialogWindows/MediaUrl.h \
           ./headers/DialogWindows/JumpToTime.h \
           ./headers/DialogWindows/LoadedSubsWindow.h \
           ./headers/DialogWindows/SRepeatWindow.h \
           ./headers/DialogWindows/ShortcutsInstructions.h \
           ./headers/DialogWindows/AudioTracksManager.h \
           ./headers/DialogWindows/PlaylistManager.h \
           ./headers/DialogWindows/ChangeThemeWindow.h \
           ./headers/DialogWindows/SubsConfigsWindow.h \
           ./headers/UiComponents/TopBar.h \
           ./headers/UiComponents/TopBarButton.h \
           ./headers/UiComponents/BottomControlPanel.h \

RC_FILE = app_icon.rc
