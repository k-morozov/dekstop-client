#include <QApplication>

#include "control/control.h"

#include <cstdlib> // std::getenv
#include <string>

int main(int argc, char** argv) {
    QApplication a(argc, argv);

    Storage::SqliteConfig config;
    config.folder_path = std::string{std::getenv("HOME")} + "/AppChat/";
    config.connection_string = "file://" + config.folder_path + "cache.db";

    Control control(config);
    control.run_app(argc, argv);

    return a.exec();
}
