/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef DIRECTORYWATCHER_H
#define DIRECTORYWATCHER_H

#include <QFileInfo>
#include <QObject>
#include <QSharedPointer>
#include <QMetaType>
class DirectoryWatcherPrivate;
/**
 * \brief Класс наблюдателя - публичный интерфейс.
 *
 * Для отслеживания директории создается объект этого класса; директория выставляется через setWatchDirectory.
 * В случае ошибки,
 */
class DirectoryWatcher : public QObject
{
    Q_OBJECT
public:
    /**
        \enum FileChangeReason

        Тип измнения файла.

        \value UnknownReason Нижележащая реализация не может определить тип изменения, его нужно делать самостоятельно.
        \value AddedReason файл добавлен
        \value RemovedReason файл удален
        \value ModifiedReason файл изменен
        */
    enum FileChangeReason { UnknownReason, AddedReason, RemovedReason, ModifiedReason };
    /**
        \enum WatcherOption

        Платформозависимые опции, передаваемые в setOption().
        В зависимости от реализации, некоторые из них могут игнорироваться.

        \value FileRecordBufferSize [int] Размер буфера в байтах, выделяемого для отслеживания изменений (Windows).
        \value DoWatchSubtree [bool] Осуществлять ли наблюдение за поддиректориями (по умолчанию false)
        */
    enum WatcherOption { FileRecordBufferSize, DoWatchSubtree } ;

    DirectoryWatcher(QObject *parent = 0);
    /**
     * \brief Директория для отслеживания изменений.
     */
    bool setWatchDirectory(const QString &dirPath);
    /**
     * \brief Тексто ошибки, если setWatchDirectory вернул false.
     */
    QString lastError() const;
    /**
     * \brief Платформозависимое выставление опций. См. FileChangeReason.
     */
    void setOption(WatcherOption option, QVariant value);
signals:
    /**
     * \brief Сигнал, когда прозошла какая-либо ошибка.
     */
    void watchError(const QString &errText);
    /**
     * \brief Сигнал при изменениях в файловой системе.
     *
     * \param fileInfo - информация о файле или директории.
     * \param reason - причина изменения.
     */
    void fileChanged(QFileInfo fileInfo, DirectoryWatcher::FileChangeReason reason);

private:
    QSharedPointer<DirectoryWatcherPrivate> m_p;
};
Q_DECLARE_METATYPE( DirectoryWatcher::FileChangeReason )
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_DECLARE_METATYPE( QFileInfo )
#endif


#endif // DIRECTORYWATCHER_H
