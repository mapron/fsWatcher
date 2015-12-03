/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "WatcherTestDialog.h"
#include <QApplication>

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    WatcherTestDialog w;
    w.show();

    return app.exec();
}
