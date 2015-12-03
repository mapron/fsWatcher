/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#ifndef DIRECTORYSELECTINPUT_H
#define DIRECTORYSELECTINPUT_H

#include <QLineEdit>
#include <QPushButton>
/**
 * \brief Поле редактирование, которое отслеживает двойной клик по нему.
 */
class CustomEventsLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomEventsLineEdit(QWidget *parent = 0);

signals:
    void dblClick();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
};

/**
 * \brief Виджет выбора директории.
 */
class DirectorySelectInput : public CustomEventsLineEdit
{
    Q_OBJECT
public:
    DirectorySelectInput(QString openDialogText, QWidget *parent = 0);
    void setDirectory(QString directory);
    QString directory() const;
    void setAllowNonExistent(bool allow);

signals:
    void directoryChanged(QString directory);

protected slots:
    void textChangedS(QString text);
    void openWindow();

protected:
    QString m_openDialogText;
    bool m_allowNonExistent;
    QPushButton *m_button;
    QString m_lastExistent;

};



#endif //DIRECTORYSELECTINPUT_H
