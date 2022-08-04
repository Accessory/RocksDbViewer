// clang-format off
#include <FlowDatabase/RocksDbDatabase.h>
#include "fluid/MainGui.h"
#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>
#include <FlowUtils/FlowFile.h>
#include <FlowUtils/FlowLog.h>
#include <FlowUtils/FlowString.h>
#include <memory>
#include <string>
#include <vector>
#include "Style.h"
#include <FlowUtils/IntervalRunner.h>
// clang-format on

MainGui mainGui;
std::string filter_column_families = "";
std::string filter_keys = "";
std::string browser_column_families_selected = "";
std::string browser_keys_selected = "";
std::vector<std::string> column_families;
std::vector<std::string> keys;
std::unique_ptr<RocksDbDatabase> db = nullptr;
Fl_Double_Window *window;
std::unique_ptr<Fl_Text_Buffer> text_buffer =
    std::make_unique<Fl_Text_Buffer>();

const std::string LAST_FILE = FlowFile::combinePath(
    FlowFile::homeFolder(), "/.config/RocksDbViewer/last.txt");

void set_icon(Fl_Double_Window *window) {
  if (FlowFile::exists("icon.png")) {
    window->icon(new Fl_PNG_Image("icon.png"));
    return;
  }
  if (FlowFile::exists("/usr/share/RocksDbViewer/icon.png")) {
    window->icon(new Fl_PNG_Image("/usr/share/RocksDbViewer/icon.png"));
    return;
  }
}

void manageActiveStatus() {
  if (!db || !db->IsOpen()) {
    mainGui.add_column_family_button->deactivate();
    mainGui.del_column_family_button->deactivate();
    mainGui.add_key_button->deactivate();
    mainGui.del_key_button->deactivate();
    mainGui.save_content_button->deactivate();
    mainGui.browser_column_families->deactivate();
    mainGui.browser_keys->deactivate();
    mainGui.text_value->deactivate();
    return;
  }

  mainGui.add_column_family_button->activate();
  mainGui.del_column_family_button->activate();
  mainGui.browser_column_families->activate();

  browser_column_families_selected.empty()
      ? mainGui.add_key_button->deactivate()
      : mainGui.add_key_button->activate();

  browser_column_families_selected.empty()
      ? mainGui.del_key_button->deactivate()
      : mainGui.del_key_button->activate();

  browser_column_families_selected.empty() ? mainGui.browser_keys->deactivate()
                                           : mainGui.browser_keys->activate();
  browser_keys_selected.empty() ? mainGui.save_content_button->deactivate()
                                : mainGui.save_content_button->activate();
  browser_keys_selected.empty() ? mainGui.text_value->deactivate()
                                : mainGui.text_value->activate();
}

void clearContent(size_t clear_from) {
  if (clear_from > 0 && text_buffer) {
    text_buffer->text("");
  }
  if (clear_from > 1) {
    mainGui.browser_keys->clear();
  }
  if (clear_from > 2) {
    mainGui.browser_column_families->clear();
  }
}

void fill_browser_column_families() {
  mainGui.browser_column_families->clear();
  for (const auto &cf : column_families) {
    if (cf.find(filter_column_families) != std::string::npos)
      mainGui.browser_column_families->add(cf.c_str());
  }
  clearContent(2);
}

void load_column_families() {
  do {
    column_families = db->ListColumnFamilies();
    std::this_thread::sleep_for(std::chrono::milliseconds(32));
  } while (column_families.empty());
  std::string browser_column_families_label =
      "Column Families - Size: " + std::to_string(column_families.size());
  mainGui.browser_column_families->copy_label(
      browser_column_families_label.c_str());
  fill_browser_column_families();
}

void fill_browser_keys() {
  mainGui.browser_keys->clear();
  for (const auto &key : keys) {
    if (key.find(filter_keys) != std::string::npos)
      mainGui.browser_keys->add(key.c_str());
  }
  mainGui.browser_keys->redraw();
  Fl::flush();
}

void load_browser_keys() {
  if (!db || !db->IsOpen() || browser_column_families_selected.empty()) {
    return;
  }

  keys = db->GetKeys(browser_column_families_selected);
  std::string browser_keys_label =
      "Keys - Size: " + std::to_string(keys.size());
  mainGui.browser_keys->copy_label(browser_keys_label.c_str());
  fill_browser_keys();
}

void cb_browser_column_families(Fl_Widget *widget) {
  const auto selected_item = mainGui.browser_column_families->value();
  if (selected_item == 0) {
    return;
  }
  browser_column_families_selected =
      mainGui.browser_column_families->text(selected_item);

  clearContent(1);
  load_browser_keys();
  manageActiveStatus();
  mainGui.mainWindow->redraw();
}

void cb_browser_keys(Fl_Widget *widget) {
  const auto selected_item_k = mainGui.browser_keys->value();
  if (selected_item_k == 0) {
    return;
  }
  browser_keys_selected = mainGui.browser_keys->text(selected_item_k);
  const auto value =
      db->Get(browser_keys_selected, browser_column_families_selected);

  text_buffer->text(value.c_str());
  manageActiveStatus();
}

void cb_connect(Fl_Widget *widget) {
  LOG_INFO << "Connect: " << std::boolalpha
           << (mainGui.button_connect->value() == 1);
  if (mainGui.button_connect->value() == 1) {
    const std::string path = mainGui.input_path->value();
    db = std::make_unique<RocksDbDatabase>(path);
    LOG_INFO << "Connection established: " << std::boolalpha << db->IsOpen();
    load_column_families();
  } else {
    db = nullptr;
    browser_column_families_selected = "";
    browser_keys_selected = "";
    clearContent(3);
    LOG_INFO << "Connection closed";
  }
  manageActiveStatus();
}

void load_last() {
  if (FlowFile::exists(LAST_FILE)) {
    const auto file_text = FlowFile::fileToString(LAST_FILE);
    const auto path =
        FlowString::getFromRegexGroup(file_text, "Path: ([^<>\"|?* \\n]+)", 1);
    mainGui.input_path->value(path.c_str());
  }
}

void save_last() {
  const std::string value = mainGui.input_path->value();
  if (value != "") {
    FlowFile::createDirIfNotExist(LAST_FILE, true);
    FlowFile::stringToFile(LAST_FILE, "Path: " + value);
  }
}

IntervalRunner runner([]() { fill_browser_keys(); },
                      std::chrono::milliseconds(333),
                      std::chrono::milliseconds(333), true);

void cb_filter_keys(Fl_Widget *widget) {
  std::string tmp = mainGui.input_filter_keys->value();
  if (tmp == filter_keys) {
    return;
  }
  filter_keys = std::move(tmp);
  runner.IsRunning() ? runner.ResetTimer() : runner.Run();
}

void cb_filter_column_families(Fl_Widget *widget) {
  std::string tmp = mainGui.input_filter_column_families->value();
  if (tmp == filter_column_families) {
    return;
  }
  filter_column_families = std::move(tmp);
  fill_browser_column_families();
}

void cb_main_window(Fl_Widget *widget) {
  db = nullptr;
  save_last();
  window->hide();
}

void cb_save_content(Fl_Widget *widget) {
  if (db->IsOpen() && browser_column_families_selected.empty() &&
      browser_keys_selected.empty()) {
    return;
  }

  const auto value = std::string(mainGui.text_value->buffer()->text(),
                                 mainGui.text_value->buffer()->length());

  db->Put(browser_column_families_selected, browser_keys_selected, value);
}

void cb_add_column_family(Fl_Widget *widget) {
  if (!db || !db->IsOpen()) {
    return;
  }

  db->CreateColumnFamilyIfNotExists(filter_column_families);
  load_column_families();
  for (int t = 1; t <= mainGui.browser_column_families->size(); ++t) {
    if (mainGui.browser_column_families->text(t) == filter_column_families) {
      mainGui.browser_column_families->select(t);
      return;
    }
  }
}

void cb_del_column_family(Fl_Widget *widget) {
  if (!db || !db->IsOpen() || browser_column_families_selected.empty()) {
    return;
  }

  db->DeleteColumnFamily(browser_column_families_selected);
  load_column_families();
}

void cb_add_key(Fl_Widget *widget) {
  if (!db || !db->IsOpen() || browser_column_families_selected.empty()) {
    return;
  }

  db->Put(browser_column_families_selected, filter_keys, "");
  load_browser_keys();

  for (int t = 1; t <= mainGui.browser_column_families->size(); ++t) {
    if (mainGui.browser_keys->text(t) == filter_keys) {
      mainGui.browser_keys->select(t);
      return;
    }
  }
  Fl::flush();
}

void cb_del_key(Fl_Widget *widget) {
  if (!db || !db->IsOpen() || browser_column_families_selected.empty() ||
      browser_keys_selected.empty()) {
    return;
  }

  db->DeleteKey(browser_column_families_selected, browser_keys_selected);
  load_browser_keys();
}

int main(int argc, char **argv) {
  Fl::scheme("pastic");
  Fl::foreground(FOREGROUND.r, FOREGROUND.g, FOREGROUND.b);
  Fl::background(BACKGROUND.r, BACKGROUND.g, BACKGROUND.b);
  Fl::background2(BACKGROUND2.r, BACKGROUND2.g, BACKGROUND2.b);
  Fl::set_color(FL_BORDER_BOX, BORDER_BOX.r, BORDER_BOX.g, BORDER_BOX.b);

  window = mainGui.make_window();
  window->callback(cb_main_window);

  mainGui.button_connect->callback(cb_connect);
  mainGui.browser_column_families->callback(cb_browser_column_families);
  mainGui.browser_keys->callback(cb_browser_keys);
  mainGui.text_value->buffer(text_buffer.get());

  mainGui.input_filter_column_families->callback(cb_filter_column_families);
  mainGui.input_filter_keys->callback(cb_filter_keys);

  mainGui.add_column_family_button->callback(cb_add_column_family);
  mainGui.del_column_family_button->callback(cb_del_column_family);
  mainGui.add_key_button->callback(cb_add_key);
  mainGui.del_key_button->callback(cb_del_key);
  mainGui.save_content_button->callback(cb_save_content);

  window->end();
  set_icon(window);

  load_last();
  window->show(argc, argv);
  manageActiveStatus();
  return Fl::run();
}