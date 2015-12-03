/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef WATCHERDIALOG_H
#define WATCHERDIALOG_H

#include <QMainWindow>
class DirectorySelectInput;
class DirectoryWidget;
/**
 * \brief Пример приложения - диалог с таблицей файлов.
 */
class WatcherDialog : public QMainWindow
{
    Q_OBJECT
public:
    WatcherDialog(QWidget* parent = 0);

private slots:
    void watchError(const QString &errText);
    void changeDirectory();

private:
    DirectorySelectInput *m_dirSelect;
    DirectoryWidget      *m_dirWidget;
};

#endif // WATCHERDIALOG_H


