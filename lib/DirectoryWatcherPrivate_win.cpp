/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */

#include <QEventLoop>
#include <QThread>
#include <QVariant>
#include <QDebug>

#include "DirectoryWatcherPrivate.h"

#define DEFAULT_BUFFER_SIZE_BYTES (1024 * 128 ) // 128 Kb.

DirectoryWatcherPrivate::DirectoryWatcherPrivate(QObject *parent)
    :QObject(parent)
{
    m_dirHandle = INVALID_HANDLE_VALUE;
    m_bufferSize = DEFAULT_BUFFER_SIZE_BYTES;
    m_doWatchSubtree = false;
}

bool DirectoryWatcherPrivate::startWatching()
{
    if (m_dirPath.isEmpty()) {
        showError("Directory path for watching is empty.");
        return false;
    }
    // Откроем наблюдаему директорию для чтения.
    m_dirHandle = CreateFile(
        m_dirPath.toStdWString().c_str(),   // file name
        FILE_LIST_DIRECTORY,                // access (read/write) mode
        // Share mode MUST be the following to avoid problems with renames via Explorer!
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode
        NULL,                               // security descriptor
        OPEN_EXISTING,                      // how to create
        FILE_FLAG_BACKUP_SEMANTICS,         // file attributes
        NULL                                // file with attributes to copy
    );
    if (m_dirHandle == INVALID_HANDLE_VALUE) {
        showError("Failed to open directory: " + lastWindowsError());
        return false;
    }
    m_changesBuffer.resize(m_bufferSize);
    m_thread = QSharedPointer<QThread> (new QThread);
    m_worker = QSharedPointer<DirectoryWatcherPrivateWorker> (new DirectoryWatcherPrivateWorker(this));
    m_worker->moveToThread(m_thread.data());
    connect(m_thread.data(), SIGNAL(started()),  m_worker.data(), SLOT(process()));
    connect(m_worker.data(), SIGNAL(finished()), m_thread.data(), SLOT(quit()));
    m_thread->start();
    return true;
}


bool DirectoryWatcherPrivate::waitForChanges()
{
    DWORD BytesReturned;

    // осуществляем блокирующий вызов. Когда функция вернет результат, у нас есть либо изменения в ФС, либо ошибка.
    bool result = ReadDirectoryChangesW(
                    m_dirHandle,                          // handle to directory
                    m_changesBuffer.data(),               // read results buffer
                    m_changesBuffer.size(),               // length of buffer
                    m_doWatchSubtree,                     // monitoring option
                    FILE_NOTIFY_CHANGE_SECURITY |
                    FILE_NOTIFY_CHANGE_CREATION |
                    FILE_NOTIFY_CHANGE_LAST_WRITE |
                    FILE_NOTIFY_CHANGE_SIZE |
                    FILE_NOTIFY_CHANGE_ATTRIBUTES |
                    FILE_NOTIFY_CHANGE_DIR_NAME |
                    FILE_NOTIFY_CHANGE_FILE_NAME,  // filter conditions
                    &BytesReturned,                // bytes returned
                    NULL,                          // overlapped buffer
                    NULL                           // completion routine
                );
    if (result) {
        readNotifications();
    }
    else{
        showError("Failed to read directory change: " + lastWindowsError());
    }
    return result;
}

void DirectoryWatcherPrivate::setOption(DirectoryWatcher::WatcherOption option, QVariant value)
{
    switch ( option) {
        case DirectoryWatcher::FileRecordBufferSize: {
            static const int minimalSize = 128;
            m_bufferSize = qMax(minimalSize, value.toInt());
        }
            break;
        case DirectoryWatcher::DoWatchSubtree:
            m_doWatchSubtree = value.toBool();
            break;
    }
}

void DirectoryWatcherPrivate::showError(const QString &errText)
{
    m_lastError = errText;
    emit watchError(errText);
}

QString DirectoryWatcherPrivate::lastWindowsError() const
{
    int lastError = GetLastError();
    wchar_t error[1024];
    DWORD length= FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lastError, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), error, sizeof(error), 0);
    return QString::fromWCharArray(error, length);
}

void DirectoryWatcherPrivate::close()
{
    if (m_dirHandle) {
        ::CancelIoEx(m_dirHandle, NULL);
        ::CloseHandle(m_dirHandle);
    }
    if (m_thread && m_thread->isRunning()) {
        QEventLoop loop;
        connect(m_thread.data(), SIGNAL(finished()), &loop, SLOT(quit()) );
        m_thread->exit();
        loop.exec();
    }
    m_dirHandle = INVALID_HANDLE_VALUE;
}


void DirectoryWatcherPrivate::readNotifications()
{
    // Прочитаем изменения из буффера. В нем хранится список FILE_NOTIFY_INFORMATION переменного размера.
    // смещение следующего FILE_NOTIFY_INFORMATION указано в NextEntryOffset.
    char * pBase = m_changesBuffer.data();
    bool condition = true;
    for (  FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)pBase
         ; condition
         ; condition = fni->NextEntryOffset, pBase += fni->NextEntryOffset, fni =(FILE_NOTIFY_INFORMATION*)pBase
         ) {

        QString filename = QString::fromWCharArray(fni->FileName, fni->FileNameLength/sizeof(wchar_t));
        if (filename == "\\") { // пропустим саму директорию
            continue;
        }

        DirectoryWatcher::FileChangeReason reason = DirectoryWatcher::UnknownReason;
        QFileInfo fileInfo(m_dirPath + filename);
        if (fileInfo.isFile() && fni->Action == FILE_ACTION_RENAMED_NEW_NAME) {
            continue; // мы и так получим FILE_ACTION_MODIFIED.
        }
        // приведем Action под стандарты нашего публичного API.
        switch (fni->Action) {
            case FILE_ACTION_ADDED:
                reason = DirectoryWatcher::AddedReason;
                break;
            case FILE_ACTION_REMOVED:
                reason = DirectoryWatcher::RemovedReason;
                break;
            case FILE_ACTION_MODIFIED:
                reason = DirectoryWatcher::ModifiedReason;
                break;
            case FILE_ACTION_RENAMED_OLD_NAME:
                reason = DirectoryWatcher::RemovedReason;
                break;
            default:
                break;
        }

        emit fileChanged(fileInfo, reason);
    };
}
