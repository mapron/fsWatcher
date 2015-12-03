/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "WatcherDialog.h"
#include <QApplication>

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    WatcherDialog w;
    w.show();

    return app.exec();
}
