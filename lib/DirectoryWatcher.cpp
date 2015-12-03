/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "DirectoryWatcher.h"
#include "DirectoryWatcherPrivate.h"
#include <QDebug>
#include <QThread>

DirectoryWatcher::DirectoryWatcher(QObject *parent)
    : QObject(parent)
    , m_p(new DirectoryWatcherPrivate())
    // SharedPointer сделан, чтобы избежать проблем, если мы не захотим потом
    // приватный класс делать QObject-ом, по каким-либо причинам.
{
    qRegisterMetaType<QFileInfo>();
    qRegisterMetaType<DirectoryWatcher::FileChangeReason>();

    connect(m_p.data(), SIGNAL(fileChanged(QFileInfo,DirectoryWatcher::FileChangeReason)),
            this,       SIGNAL(fileChanged(QFileInfo,DirectoryWatcher::FileChangeReason)),
            Qt::QueuedConnection);

    connect(m_p.data(), SIGNAL(watchError(QString)),
            this,       SIGNAL(watchError(QString)),
            Qt::QueuedConnection);
}

bool DirectoryWatcher::setWatchDirectory(const QString &dirPath)
{
    return m_p->setWatchDirectory(dirPath);
}

QString DirectoryWatcher::lastError() const
{
    return m_p->lastError();
}

//-------------- Платформонезависимый код для DirectoryWatcherPrivate. ------------
QString DirectoryWatcherPrivate::lastError() const
{
    return m_lastError;
}
DirectoryWatcherPrivate::~DirectoryWatcherPrivate()
{
    close();
}

bool DirectoryWatcherPrivate::setWatchDirectory(const QString &dirPath)
{
    QString absPath = dirPath.isEmpty()
                    ? ""
                    : QFileInfo(dirPath).absoluteFilePath() + "/";
    if (m_dirPath == absPath) {
        return true;
    }
    close();
    m_dirPath = absPath;
    if (m_dirPath.size()) {
        return startWatching();
    }
    return true;
}
void DirectoryWatcherPrivateWorker::process()
{
    while( m_watcher->waitForChanges() ) {
        ;
    }
    emit finished();
}



