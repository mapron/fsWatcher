/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "WatcherTestDialog.h"
#include <DirectorySelectInput.h>
#include <DirectoryWidget.h>
#include <QBoxLayout>
#include <QMessageBox>
#include <TestUtils.h>
#include <QDir>
#include <QSpinBox>
#define FILE_OPERATION_COUNT 15

WatcherTestDialog::WatcherTestDialog(QWidget *parent)
    : QDialog(parent)
{

    m_dirSelect = new DirectorySelectInput(tr("Select folder to watch"), this);
    m_dirWidget =  new DirectoryWidget(this);
    m_operationsCounter = new QSpinBox(this);
    m_operationsCounter->setMinimum(1);
    m_operationsCounter->setMaximum(1000);
    m_operationsCounter->setValue(FILE_OPERATION_COUNT);
    QPushButton *btn_deleteRandom = new QPushButton(tr("Delete some files"), this);
    QPushButton *btn_createRandom = new QPushButton(tr("Create some files"), this);
    QPushButton *btn_modifyRandom = new QPushButton(tr("Change some files"), this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    mainLayout->addWidget(m_dirSelect);
    buttonsLayout->addWidget(m_operationsCounter);
    buttonsLayout->addWidget(btn_deleteRandom);
    buttonsLayout->addWidget(btn_createRandom);
    buttonsLayout->addWidget(btn_modifyRandom);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(m_dirWidget);

    connect(btn_deleteRandom, SIGNAL(clicked(bool)), this, SLOT(deleteRandom()));
    connect(btn_createRandom, SIGNAL(clicked(bool)), this, SLOT(createRandom()));
    connect(btn_modifyRandom, SIGNAL(clicked(bool)), this, SLOT(modifyRandom()));
    connect(m_dirSelect, SIGNAL(directoryChanged(QString)), this, SLOT(changeDirectory()));
    m_tmpPath = QDir::tempPath() + "/watcherManualTest/";
    QDir(m_tmpPath).mkpath(m_tmpPath);
    m_dirSelect->setDirectory(m_tmpPath);
    resize(700, 600);
}

WatcherTestDialog::~WatcherTestDialog()
{
    TestUtils::removeDir(m_tmpPath);
}

void WatcherTestDialog::watchError(const QString &errText)
{
    QMessageBox::critical(this, tr("Error"), tr("Error occured:\r\n") + errText);
}

void WatcherTestDialog::changeDirectory()
{
    m_dirWidget->setWatchDirectory(m_dirSelect->directory());
}
void WatcherTestDialog::deleteRandom()
{
    for (int i = 0; i < m_operationsCounter->value(); i++) {
        TestUtils::deleteRandomFile(m_dirSelect->directory());
    }
}
void WatcherTestDialog::createRandom()
{
    for (int i = 0; i < m_operationsCounter->value(); i++) {
        TestUtils::createRandomFile(m_dirSelect->directory(), TestUtils::random(100));
    }
}

void WatcherTestDialog::modifyRandom()
{
    for (int i = 0; i < m_operationsCounter->value(); i++) {
        TestUtils::changeRandomFile(m_dirSelect->directory(), TestUtils::random(100, -100));
    }
}


