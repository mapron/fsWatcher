/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef WATCHERTEST_H
#define WATCHERTEST_H

#include <QObject>
#include <QStringList>
#include <QModelIndex>

#include <DirectoryWatcher.h>

class DirectoryWatcherTestProxy;
/**
 * \brief объект, запускающий тесты.
 */
class WatcherTest : public QObject
{
    Q_OBJECT
public:
    WatcherTest(QObject* parent = 0);
    ~WatcherTest();

private slots:
    void creatingFiles();
    void deleteFiles();
    void modifyingFiles();
    void changeDirectory();

    void watcherBench();
    void qtBench();

    void cleanup();
private:
    DirectoryWatcher            *m_watcher;
    DirectoryWatcherTestProxy   *m_testWatcherProxy;
    QString                      m_tmpPath;
};

/**
 * \brief Обвязка над отслеживателем, позволяющая записать буфер изменений.
 */
class DirectoryWatcherTestProxy : public QObject
{
    Q_OBJECT
public:
    QStringList m_changedRecords;
    QStringList m_addedRecords;
    QStringList m_removedRecords;
    QStringList m_errors;


    DirectoryWatcherTestProxy(QObject *parent=0);
    void clear();
    /**
     * \brief Функия блокирует выполнение до тех пор, пока не накопитса count добавленных файлов.
     *
     * Создается QEventLoop.
     */
    void waitFilesAdded(int count);
signals:
    void addCounterZeroed();
public slots:
    void fileChanged(QFileInfo fileInfo, DirectoryWatcher::FileChangeReason reason);
    void watchError(const QString& errText);
private:
    int m_addCounter;
};
/**
 * \brief Промежуточный класс для тестирования добавленных строк в QFileSysteemModel.
 */
class FileSystemModelTestProxy : public QObject
{
    Q_OBJECT
public:
    FileSystemModelTestProxy(QObject *parent=0);
    /**
     * \brief Функия блокирует выполнение до тех пор, пока не накопитса count вставленных строк.
     *
     * Создается QEventLoop. Отслеживание строк в модели осуществляется через привязку rowsInserted()
     * сигнала нужной модели на одноименный слот rowsInserted().
     */
    void waitFilesAdded(int count);
signals:
    void addCounterZeroed();
public slots:
    void rowsInserted(const QModelIndex &parent, int first, int last);
private:
    int m_addCounter;
};
#endif // WATCHERTEST_H
