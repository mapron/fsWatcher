/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "DirectorySelectInput.h"
#include <QFileDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QToolTip>

CustomEventsLineEdit::CustomEventsLineEdit( QWidget *parent)
    : QLineEdit(parent)
{
}

//------------------------------------------------------------------------------

void CustomEventsLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    QLineEdit::mouseDoubleClickEvent(event);
    emit dblClick();
}


//==============================================================================
// Виджет выбора директории
//==============================================================================
DirectorySelectInput::DirectorySelectInput(QString openDialogText, QWidget *parent)
    : CustomEventsLineEdit(parent), m_openDialogText(openDialogText)
{
    m_allowNonExistent = false;

    QHBoxLayout* layout=new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
    m_button = new QPushButton( "...", this);
    m_button->setFixedWidth(25);
    m_button->setCursor(Qt::PointingHandCursor);

    layout->addStretch();
    layout->addWidget(m_button);
    setTextMargins(0,0,m_button->width(),0);

    connect(m_button, SIGNAL(clicked()), this, SLOT(openWindow()));
    connect(this, SIGNAL(dblClick()), this, SLOT(openWindow()));
    connect(this, SIGNAL(textChanged(QString)), SLOT(textChangedS(QString)));
}

//------------------------------------------------------------------------------
void DirectorySelectInput::setDirectory(QString folder)
{
    this->setText(folder);
}

//------------------------------------------------------------------------------
QString DirectorySelectInput::directory() const
{
    QString text = this->text();
    QString folder = m_allowNonExistent ? text : ( QDir(text).exists() ?  text : m_lastExistent );
    folder.replace("\\", "/");
    if (folder.startsWith("//")) folder = QString("\\\\") + folder.mid(2); // если сетевая шара
    if (!folder.endsWith("/")) folder += "/";
    return folder;
}

//------------------------------------------------------------------------------
void DirectorySelectInput::setAllowNonExistent(bool allow)
{
    m_allowNonExistent = allow;
}

//------------------------------------------------------------------------------
void DirectorySelectInput::textChangedS(QString text)
{
    if (QDir(text).exists() )   m_lastExistent = text;
    emit directoryChanged(directory());
}

//------------------------------------------------------------------------------
void DirectorySelectInput::openWindow()
{
    QString dir = QFileDialog::getExistingDirectory( this, m_openDialogText,
                                       this->text() ,QFileDialog::ShowDirsOnly
                                       | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
         this->setText(dir);
    }
}
