/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef DIRECTORYMODEL_H
#define DIRECTORYMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QHash>
#include <QStringList>
#include "DirectoryWatcher.h"
/**
 * \brief Табличная модель содержимого директории.
 *
 * Содержимое обновляется автоматически.
 * Отслеживаемая директория выставляется в методе setWatchDirectory.
 * Для отслеживания ошибок, используется сигнал watchError().
 *
 * Модель использовать как обычную табличную модель для QTableView, например.
 */
class DirectoryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DirectoryModel(QObject *parent = 0);
    bool setWatchDirectory(const QString &dirPath);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

signals:
    void watchError(const QString &errText);
    void rowCountChanged();

private slots:
    void fileChanged(QFileInfo fileInfo, DirectoryWatcher::FileChangeReason reason);

private:
    void refreshFileIndex();
    enum InternalColumns { ColName, ColSize, ColModified };
    QString         m_dirPath;
    QStringList     m_headers;
    QList<QFileInfo> m_files;
    QHash<QString, int> m_namesIndex;
    DirectoryWatcher *m_watcher;
};

#endif // DIRECTORYMODEL_H
