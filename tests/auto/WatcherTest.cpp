/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include <QDebug>
#include <QDir>
#include <QTest>
#include <QEventLoop>
#include <QTimer>
#include <QFileSystemModel>


#include "WatcherTest.h"
#include "TestUtils.h"

#ifndef FILE_OPERATION_WAIT_MS
#define FILE_OPERATION_WAIT_MS 100
#endif

inline void sleep(int msec = FILE_OPERATION_WAIT_MS) {
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}

WatcherTest::WatcherTest(QObject *parent)
    : QObject(parent)
{
    m_watcher = new DirectoryWatcher(this);
    m_testWatcherProxy = new DirectoryWatcherTestProxy(this);
    connect(m_watcher,          SIGNAL(fileChanged(QFileInfo,DirectoryWatcher::FileChangeReason)),
            m_testWatcherProxy, SLOT(fileChanged(QFileInfo,DirectoryWatcher::FileChangeReason)));
    connect(m_watcher,          SIGNAL(watchError(QString)),
            m_testWatcherProxy, SLOT(watchError(QString)));

    m_tmpPath = QDir::tempPath() + "/watcherAutoTest/";
    QDir(m_tmpPath).mkpath(m_tmpPath);
    TestUtils::removeDir(m_tmpPath, false);
    m_watcher->setWatchDirectory(m_tmpPath);
}

WatcherTest::~WatcherTest()
{
}

void WatcherTest::creatingFiles()
{
    TestUtils::writeFile(m_tmpPath + "foo.txt", 0);
    TestUtils::writeFile(m_tmpPath + "bar.txt", 100);

    QStringList expectedFiles = QStringList()
                                << "foo.txt"
                                << "bar.txt"
                                   ;
    sleep();
    QCOMPARE(m_testWatcherProxy->m_errors, QStringList());
    QCOMPARE(m_testWatcherProxy->m_addedRecords, expectedFiles);

    m_testWatcherProxy->m_addedRecords.clear();
    const int testAddCount = 500;
    for (int i=0; i< testAddCount; i++)
        TestUtils::createRandomFile(m_tmpPath);
    sleep();
    QCOMPARE(m_testWatcherProxy->m_addedRecords.size(), testAddCount);
}

void WatcherTest::deleteFiles()
{
    const int testCount = 500;
    for (int i=0; i< testCount; i++)
        TestUtils::createRandomFile(m_tmpPath);
    TestUtils::writeFile(m_tmpPath + "bar.txt", 42);

    m_testWatcherProxy->clear();
    QFile::remove(m_tmpPath + "bar.txt");
    QStringList expectedFiles = QStringList()
                                << "bar.txt"
                                   ;
    sleep();
    QCOMPARE(m_testWatcherProxy->m_errors, QStringList());
    QCOMPARE(m_testWatcherProxy->m_removedRecords, expectedFiles);
    m_testWatcherProxy->clear();

    TestUtils::removeDir(m_tmpPath, false);
    sleep();
    QCOMPARE(m_testWatcherProxy->m_removedRecords.size(), testCount);

}

void WatcherTest::modifyingFiles()
{
    const int testCount = 50;
    const int changedCount = 5;
    for (int i=0; i< testCount; i++)
        TestUtils::createRandomFile(m_tmpPath);
    sleep();
    m_testWatcherProxy->clear();
    for (int i = 0; i< changedCount; i++)
        TestUtils::changeRandomFile(m_tmpPath, 1); //TODO: улучшить этот тест. При некоторых значениях могут возникнуть коллизии.
    sleep();
    QStringList changedUnique = m_testWatcherProxy->m_changedRecords;
    changedUnique.removeDuplicates();
    QCOMPARE(changedUnique.size(), changedCount);

}

void WatcherTest::changeDirectory()
{
    QDir(m_tmpPath).mkdir("sub");
    m_watcher->setWatchDirectory(m_tmpPath + "sub/");
    sleep();
    m_testWatcherProxy->clear();

    TestUtils::writeFile(m_tmpPath + "foo.txt", 0);
    TestUtils::writeFile(m_tmpPath + "bar.txt", 100);
    TestUtils::writeFile(m_tmpPath + "sub/baz.txt", 42);

    QStringList expectedFiles = QStringList()
                                << "baz.txt"
                                   ;
    sleep();
   // qDebug() << m_testWatcherProxy->m_errors;
    QCOMPARE(m_testWatcherProxy->m_errors, QStringList());
    QCOMPARE(m_testWatcherProxy->m_addedRecords, expectedFiles);

}

void WatcherTest::watcherBench()
{
    const int testAddCount = 5000;

    QBENCHMARK {
       for (int i=0; i< testAddCount; i++)
           TestUtils::writeFile(m_tmpPath + QString::number(i ) + ".txt", 0);
       m_testWatcherProxy->waitFilesAdded(testAddCount);
    }
}

void WatcherTest::qtBench()
{
    const int testAddCount = 5000;

    QFileSystemModel fsModel;
    fsModel.setRootPath(m_tmpPath);
   // QModelIndex i = fsModel.index(m_tmpPath);

    FileSystemModelTestProxy fsProxy;
    connect(&fsModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            &fsProxy,   SLOT(rowsInserted(QModelIndex,int,int)));

    QBENCHMARK {
       for (int i=0; i< testAddCount; i++)
           TestUtils::writeFile(m_tmpPath + QString::number(i ) + ".txt", 0);
         fsProxy.waitFilesAdded(testAddCount);
    }
}

void WatcherTest::cleanup()
{
    TestUtils::removeDir(m_tmpPath, false);
    sleep();
    m_testWatcherProxy->clear();
    m_watcher->setWatchDirectory(m_tmpPath);

}
//-----

DirectoryWatcherTestProxy::DirectoryWatcherTestProxy(QObject *parent)
    : QObject(parent), m_addCounter(0)
{
}

void DirectoryWatcherTestProxy::clear()
{
    m_changedRecords.clear();
    m_addedRecords.clear();
    m_removedRecords.clear();
    m_errors.clear();
}

void DirectoryWatcherTestProxy::waitFilesAdded(int count)
{
    m_addCounter = count;
    QEventLoop loop;
    connect(this, SIGNAL(addCounterZeroed()), &loop, SLOT(quit()), Qt::QueuedConnection);
    loop.exec();
}

void DirectoryWatcherTestProxy::fileChanged(QFileInfo fileInfo, DirectoryWatcher::FileChangeReason reason)
{
    switch (reason) {
        case DirectoryWatcher::AddedReason:
            m_addedRecords << fileInfo.fileName();
            if (m_addCounter > 0) {
                m_addCounter--;
                if (m_addCounter == 0) {
                    emit addCounterZeroed();
                }
            }
            break;
        case DirectoryWatcher::RemovedReason:
            m_removedRecords << fileInfo.fileName();
            break;
        case DirectoryWatcher::ModifiedReason:
        default:
            m_changedRecords << fileInfo.fileName();
            break;
    }
}

void DirectoryWatcherTestProxy::watchError(const QString &errText)
{
     m_errors << errText;
}

FileSystemModelTestProxy::FileSystemModelTestProxy(QObject *parent)
    : QObject(parent), m_addCounter(0)
{

}

void FileSystemModelTestProxy::waitFilesAdded(int count)
{
    m_addCounter = count;
    QEventLoop loop;
    connect(this, SIGNAL(addCounterZeroed()), &loop, SLOT(quit()), Qt::QueuedConnection);
    loop.exec();
}

void FileSystemModelTestProxy::rowsInserted(const QModelIndex &parent, int first, int last)
{
    int count = last - first + 1;
    m_addCounter -= count;
    if (m_addCounter == 0) {
        emit addCounterZeroed();
    }
}
