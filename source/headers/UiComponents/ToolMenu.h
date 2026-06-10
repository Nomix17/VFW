#ifndef TOOLMENU
#define TOOLMENU

#include <functional>
#include "MenuButton.h"

class ToolMenu {
  protected:
    std::vector<QAction*> createdActions;

    QList<QString> actionsCategories = {
      "Media","Playback","Audio","Video","Subtitle","View","Help"
    };

    QList<QList<QString>> actionsList = {
      {"Open File","Open Folder","Open Media","Quit"},
      {"Jump Backward","Jump Forward","Jump to Time","Move To Next Chapter","Move To Previous Chapter","Start Segment Loop"},
      {"Audio Tracks","Full Volume","Mute"},
      {"Stretch to Fit"},
      {"Add Subtitle File","Load Subtitle","Hide Subtitles","Add Delay","Reduce Delay","Subtitle Settings"},
      {"Video Title","Display Chapters Indicators","Change Theme"},
      {"Shortcuts Instructions"}
    };

    void buildButtons(std::function<void(MenuButton*)> onButtonCreated) {
      for (int i = 0; i < actionsCategories.length(); i++) {
        MenuButton* btn = new MenuButton(actionsCategories[i]);
        btn->buildMenuActions(actionsList[i], this->createdActions.size());

        auto list = btn->getButtonActionsList();
        createdActions.insert(createdActions.end(), list.begin(), list.end());

        onButtonCreated(btn);
      }
    }

  public:
    enum controlbuttonslayout_LABEL_ELEMENTS {
      Open_file, Open_folder, Open_media, Quit,
      JUMP_BACKWARD, JUMP_FORWARD, JUMP_TO_TIME,
      JUMP_TO_NEXT_CHAP, JUMP_TO_PREV_CHAP, TOGGLE_LOOPSEGMENT,
      AUDIO_TRACKS, FULL_VOLUME, MUTE, TOGGLE_ASPRadio,
      TOGGLE_SUB, LOADSUBTITLES, TOGGLE_SUBDISPLAY,
      ADDDELAY, REDUCEDELAY, SUBSETTINGS,
      TITLE, TOGGLE_CHAPTERSINDICATORS, THEME, SHORTCUTS,
    };
    std::vector<QAction*> getActionsObjects() {
      return createdActions;
    }

};

#endif
