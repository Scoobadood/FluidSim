#include <QApplication>
#include "main_window.h"
#include "spdlog/cfg/env.h"

int main(int argc, char *argv[])
{
    spdlog::cfg::load_env_levels();
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
