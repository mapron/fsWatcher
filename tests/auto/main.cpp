/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include <QtTest/QTest>
#include <QTimer>
#include "WatcherTest.h"

int main(int argc, char *argv[])
{
    QApplication application( argc, argv );
    WatcherTest testWatcher;
    QTest::qExec(&testWatcher , argc, argv);
    QTimer::singleShot(100, &application, SLOT(quit()));
    return application.exec();
}
