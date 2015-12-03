/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef DIRECTORYWATCHER_P_H
#define DIRECTORYWATCHER_P_H

#include <QObject>
#ifdef Q_OS_WIN32
#include <qt_windows.h>
#endif
#include <QSharedPointer>

#include "DirectoryWatcher.h"
class DirectoryWatcherPrivateWorker;
/**
 * \brief Класс наблюдателя - закрытый интерфейс.
 *
 * Внимание! Этот класс не является публичным API!
 * Могут произойти любые несовместимые изменения в дальнейшем.
 */
class DirectoryWatcherPrivate : public QObject
{
    Q_OBJECT
public:
    DirectoryWatcherPrivate(QObject *parent = 0);
    ~DirectoryWatcherPrivate();
    bool setWatchDirectory(const QString &dirPath);
    bool startWatching();
    QString lastError() const;
    /**
     * \brief Блокирующее ожидание изменений в ФС. Запускается через DirectoryWatcherPrivateWorker
     */
    bool waitForChanges();
    void setOption(DirectoryWatcher::WatcherOption option, QVariant value);
signals:
    void fileChanged(QFileInfo fileInfo, DirectoryWatcher::FileChangeReason reason);
    void watchError(const QString &errText);
private:
    void showError(const QString &errText);
    QString lastWindowsError() const;
    void close();

    void readNotifications();
    QString m_lastError;
    QString m_dirPath;
    bool    m_doWatchSubtree;
#ifdef Q_OS_WIN32
    int         m_bufferSize;
    QByteArray  m_changesBuffer;
    HANDLE      m_dirHandle;
    QSharedPointer<QThread> m_thread;
    QSharedPointer<DirectoryWatcherPrivateWorker> m_worker;
#else
//TODO:
#endif

};

/**
 * \brief Объект, который связан с потоком отслеживания изменений.
 *
 * Внимание! Этот класс не является публичным API!
 * Могут произойти любые несовместимые изменения в дальнейшем.
 */
class DirectoryWatcherPrivateWorker : public QObject {
    Q_OBJECT
public:
    DirectoryWatcherPrivateWorker(DirectoryWatcherPrivate *watcher,QObject *parent=0)
        :QObject(parent),m_watcher(watcher) {
    }

public slots:
    void process();

signals:
    void finished();
private:
    DirectoryWatcherPrivate *m_watcher;
};

#endif // DIRECTORYWATCHER_H

