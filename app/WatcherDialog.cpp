/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "WatcherDialog.h"
#include <DirectorySelectInput.h>
#include <DirectoryWidget.h>
#include <QBoxLayout>
#include <QMessageBox>
#include <QDir>
#include <QSpinBox>
#include <QApplication>
#include <QMenuBar>
#include <QLabel>

WatcherDialog::WatcherDialog(QWidget *parent)
    : QMainWindow(parent)
{
    m_dirSelect = new DirectorySelectInput(tr("Select folder to watch"), this);

    m_dirWidget =  new DirectoryWidget(this);

    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout* selectLayout = new QHBoxLayout();

    mainLayout->addLayout(selectLayout);
    mainLayout->addWidget(m_dirWidget);
    selectLayout->addWidget(new QLabel(tr("Select directory to watch files:"), this));
    selectLayout->addWidget(m_dirSelect);


    QAction* exitAction = new QAction(tr("Exit"), this);
    QMenu * fileMenu=menuBar()->addMenu(tr("File"));
    fileMenu->addAction(exitAction);


    connect(m_dirSelect, SIGNAL(directoryChanged(QString)), this, SLOT(changeDirectory()));
    connect(exitAction, SIGNAL(triggered(bool)), qApp, SLOT(quit()));

    m_dirSelect->setDirectory(QApplication::applicationDirPath());

    resize(700, 600);
}
void WatcherDialog::watchError(const QString &errText)
{
    QMessageBox::critical(this, tr("Error"), tr("Error occured:\r\n") + errText);
}

void WatcherDialog::changeDirectory()
{
    m_dirWidget->setWatchDirectory(m_dirSelect->directory());
}
