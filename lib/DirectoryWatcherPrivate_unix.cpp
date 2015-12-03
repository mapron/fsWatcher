/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "DirectoryWatcherPrivate.h"
#include <QDebug>

DirectoryWatcherPrivate::DirectoryWatcherPrivate(QObject *parent)
    :QObject(parent)
{

}


bool DirectoryWatcherPrivate::startWatching()
{
    qFatal("DirectoryWatcherPrivate::startWatching is not implemented on this platform!");
    return false;
}

void DirectoryWatcherPrivate::setOption(DirectoryWatcher::WatcherOption option, QVariant value)
{
    qFatal("DirectoryWatcherPrivate::setOption is not implemented on this platform!");
}

bool DirectoryWatcherPrivate::waitForChanges()
{
    return false;
}

void DirectoryWatcherPrivate::close()
{

}
