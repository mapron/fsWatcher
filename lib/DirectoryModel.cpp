/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include <QDir>
#include <QDateTime>
#include <QDebug>

#include "DirectoryModel.h"
#include "DirectoryWatcher.h"

/**
 * \brief платформозависимое имя файла, которое можно использовать в хеш-таблице.
 *
 * Под Windows- приводится к нижнему регистру.
 */
inline QString indexedFileName(const QFileInfo &fileInfo) {
     QString fileName = fileInfo.fileName();
#ifdef Q_OS_WIN
     fileName= fileName.toLower();
#endif
     return fileName;
}
/**
 * \brief Отображение размера файла для пользователя.
 */
inline QString formatFileSize(qint64 size) {
    return QString::number(size);
}

DirectoryModel::DirectoryModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_watcher(new DirectoryWatcher(this))
{
    m_headers << tr("Name") << tr("Size") << tr("Modification");
    connect(m_watcher, SIGNAL(fileChanged(QFileInfo,DirectoryWatcher::FileChangeReason)),
            this,        SLOT(fileChanged(QFileInfo,DirectoryWatcher::FileChangeReason)));
    connect(m_watcher, SIGNAL(watchError(QString)),
            this,      SIGNAL(watchError(QString)));
}

bool DirectoryModel::setWatchDirectory(const QString &dirPath)
{
    bool changed = m_dirPath != dirPath;
    bool result = true;
    m_dirPath = dirPath;
    if (changed) {
        beginResetModel();
        m_watcher->setWatchDirectory("");
        m_files = QDir(m_dirPath).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name | QDir::DirsFirst);
        refreshFileIndex();
        result = m_watcher->setWatchDirectory(dirPath);
        endResetModel();
        if (m_files.size()) {
            emit rowCountChanged();
        }
    }
    return result;
}

int DirectoryModel::rowCount(const QModelIndex &parent) const
{
    return m_files.size();
}

int DirectoryModel::columnCount(const QModelIndex &parent) const
{
    return m_headers.size();
}

QVariant DirectoryModel::data(const QModelIndex &index, int role) const
{

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const QFileInfo &fileInfo = m_files[index.row()];
        switch (index.column()) {
            case ColName:
                return fileInfo.fileName();
            case ColSize:
                return fileInfo.isFile() ? formatFileSize(fileInfo.size()) : tr("Folder");
            case ColModified:
                return fileInfo.isFile() ? fileInfo.lastModified().toString(Qt::SystemLocaleLongDate) : "";
        }
    }
    return QVariant();
}

Qt::ItemFlags DirectoryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == ColName)
        f |= Qt::ItemIsEditable;
    return f;
}

bool DirectoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        QFileInfo &fileInfo = m_files[index.row()];
        if (index.column() == ColName) {
            return QFile::rename(fileInfo.absoluteFilePath(), fileInfo.absolutePath() + "/" + value.toString());
            // нам нет никакой необходимости делать здесь dataChanged(). В случае успешного переименования, мы получим
            // сигнал от Watcher-а и данные обновятся.
        }
    }
    return false;
}

QVariant DirectoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_headers.value(section);
    }
    return QVariant();
}

void DirectoryModel::fileChanged(QFileInfo fileInfo, DirectoryWatcher::FileChangeReason reason)
{
    QString fileName = indexedFileName(fileInfo);
    int insertionIndex = m_files.size();
    int rowIndex = m_namesIndex.value(fileName, -1);
    if (rowIndex > -1) {
        if (reason == DirectoryWatcher::RemovedReason) {
            beginRemoveRows(QModelIndex(),rowIndex, rowIndex);
            m_files.removeAt(rowIndex);
            bool foundInIndex = false;
            for (QHash<QString, int>::iterator i = m_namesIndex.begin(); i != m_namesIndex.end(); ) {
                int &value = i.value();
                if (value == rowIndex){
                    m_namesIndex.erase(i++);
                    foundInIndex = true;
                    continue;
                }else if (value > rowIndex){
                    value --;
                }
                i++;
            }
            if (!foundInIndex) {
                qWarning() << "DirectoryModel::fileChanged: Name index is unsyncronized, that should'n happen! Resetting index.";
                refreshFileIndex();
            }
            endRemoveRows();
            emit rowCountChanged();
        }else{
            m_files[rowIndex] = fileInfo;
            emit dataChanged(this->index(rowIndex,0), this->index(rowIndex, m_headers.size()-1));
        }
    }else if (reason != DirectoryWatcher::RemovedReason){
          beginInsertRows(QModelIndex(),insertionIndex, insertionIndex);
          m_files << fileInfo;
          m_namesIndex[fileName]=insertionIndex;
          endInsertRows();
          emit rowCountChanged();
    }
}

void DirectoryModel::refreshFileIndex()
{
     m_namesIndex.clear();
     for (int i = 0; i < m_files.size(); i++) {
         QString fileName = indexedFileName(m_files[i]);
         m_namesIndex[fileName] = i;
     }
}
