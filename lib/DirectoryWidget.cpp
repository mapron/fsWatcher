/**
  * Code in this file is licensed under BSD license. See LICENSE file for more information.
  */
#include "DirectoryWidget.h"
#include <QBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include "DirectoryModel.h"

DirectoryWidget::DirectoryWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this); // возможно, понадобится добавить сюда тулбар.
    QTableView* tableView = new QTableView(this);
    m_model = new DirectoryModel(this);
    tableView->verticalHeader()->hide();
    tableView->setAlternatingRowColors(true);
    tableView->setSortingEnabled(false);
    tableView->setModel(m_model);

    layout->addWidget(tableView);

    connect(m_model,    SIGNAL(watchError(QString)),
            this,       SIGNAL(watchError(QString)));
    connect(m_model,    SIGNAL(rowCountChanged()),
            tableView,  SLOT(resizeColumnsToContents()));
    tableView->resizeColumnsToContents();

}

bool DirectoryWidget::setWatchDirectory(const QString &dirPath)
{
    return m_model->setWatchDirectory(dirPath);
}
