/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef WATCHERTESTDIALOG_H
#define WATCHERTESTDIALOG_H

#include <QDialog>
QT_BEGIN_NAMESPACE
class QSpinBox;
QT_END_NAMESPACE

class DirectorySelectInput;
class DirectoryWidget;
/**
 * \brief Тестовое приложение.
 *
 * Позволяет вручную протестировать интерфейс списка файлов.
 * Есть кнопки для создания/удаления/изменения случайных файлов.
 * По умолчанию открывает временную директорию, при уничтожении объекта, очищает эту временную директорию.
 */
class WatcherTestDialog : public QDialog
{
    Q_OBJECT
public:
    WatcherTestDialog(QWidget *parent = 0);
    ~WatcherTestDialog();

private slots:
    void watchError(const QString &errText);
    void changeDirectory();
    void deleteRandom();
    void createRandom();
    void modifyRandom();

private:
    DirectorySelectInput *m_dirSelect;
    DirectoryWidget      *m_dirWidget;
    QSpinBox             *m_operationsCounter;
    QString               m_tmpPath;
};

#endif // WATCHERTESTDIALOG_H
