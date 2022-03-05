#pragma once

#include <string>

struct TableModel {
    std::string id;
    std::string status;
    std::string name;
    double progress;
    std::string folder_id;
    std::string file_id;

    bool isFile() const {
        return file_id != "";
    }

    bool isFolder() const {
        return folder_id != "" && file_id == "";
    }

    bool isRunning() const {
        return status == "running";
    }

    bool isFinished() const {
        return status == "finished" || status == "seeding";
    }
};