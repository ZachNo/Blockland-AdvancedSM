#include "mainwindow.h"
#include "ui_mainwindow.h"

//Load super admin list
void MainWindow::updateSAList(QString val)
{
    //Clear list first
    superAdminListModel->clear();

    val = val.remove("\"").trimmed();
    QStringList blids = val.split(" ");
    for(int i = 0; i < blids.size(); ++i)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(blids[i]);
        superAdminListModel->appendRow(item);
    }

    updateStatus("Super admin list successfully imported!");
}

//Load super admin list
void MainWindow::updateAList(QString val)
{
    //Clear list first
    adminListModel->clear();

    val = val.remove("\"").trimmed();
    QStringList blids = val.split(" ");
    for(int i = 0; i < blids.size(); ++i)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(blids[i]);
        adminListModel->appendRow(item);
    }

    updateStatus("Admin list successfully imported!");
}
