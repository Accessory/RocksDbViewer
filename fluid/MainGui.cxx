// generated by Fast Light User Interface Designer (fluid) version 1.0308

#include "MainGui.h"

Fl_Double_Window* MainGui::make_window() {
  { mainWindow = new Fl_Double_Window(995, 690, "RocksDbViewer");
    mainWindow->user_data((void*)(this));
    { Fl_Menu_Bar* o = new Fl_Menu_Bar(10, 0, 666, 26);
      o->box(FL_NO_BOX);
      o->color((Fl_Color)40);
    } // Fl_Menu_Bar* o
    { input_path = new Fl_Input(50, 25, 830, 30, "Path:");
    } // Fl_Input* input_path
    { browser_column_families = new Fl_Browser(15, 100, 215, 545, "Column Familes");
      browser_column_families->type(2);
      browser_column_families->align(Fl_Align(FL_ALIGN_TOP));
    } // Fl_Browser* browser_column_families
    { browser_keys = new Fl_Browser(235, 100, 340, 545, "Keys");
      browser_keys->type(2);
      browser_keys->align(Fl_Align(FL_ALIGN_TOP));
    } // Fl_Browser* browser_keys
    { text_value = new Fl_Text_Display(580, 100, 410, 580);
      Fl_Group::current()->resizable(text_value);
    } // Fl_Text_Display* text_value
    { button_connect = new Fl_Light_Button(900, 25, 90, 30, "Connect");
    } // Fl_Light_Button* button_connect
    { input_filter_column_families = new Fl_Input(50, 650, 180, 30, "Filter");
      input_filter_column_families->when(FL_WHEN_CHANGED);
    } // Fl_Input* input_filter_column_families
    { input_filter_keys = new Fl_Input(275, 650, 300, 30, "Filter");
      input_filter_keys->when(FL_WHEN_CHANGED);
    } // Fl_Input* input_filter_keys
    mainWindow->end();
  } // Fl_Double_Window* mainWindow
  return mainWindow;
}