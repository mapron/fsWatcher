/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef DIRECTORYWIDGET_H
#define DIRECTORYWIDGET_H


#include <QWidget>
class DirectoryModel;
/**
 * \brief DirectoryWidget - простой виджет, отображающий таблицу (QTableView).
 *
 * Для установки отслеживаемой директории, вызываем setWatchDirectory().
 */
class DirectoryWidget : public QWidget   // не наследуемся от QTableView, потому что с другими моделями мы не можем работать.
{
    Q_OBJECT
public:
    DirectoryWidget(QWidget *parent = 0);

    bool setWatchDirectory(const QString &dirPath);
signals:
    void watchError(const QString &errText);
private:
    DirectoryModel *m_model;
};

#endif // DIRECTORYMODEL_H
