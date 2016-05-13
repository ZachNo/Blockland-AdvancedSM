#include "mainwindow.h"
#include "ui_mainwindow.h"

//Load super admin list
void MainWindow::updateSAList(QString val)
{
    //Clear list first
    superAdminListModel->clear();

    val = val.remove("\"").trimmed();

    if(val == tr(""))
        return;

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

    if(val == tr(""))
        return;

    QStringList blids = val.split(" ");
    for(int i = 0; i < blids.size(); ++i)
    {
        QStandardItem *item = new QStandardItem();
        item->setText(blids[i]);
        adminListModel->appendRow(item);
    }

    updateStatus("Admin list successfully imported!");
}

void MainWindow::adminFocused()
{
    lastSelectedSAorA = false;
}

void MainWindow::sAdminFocused()
{
    lastSelectedSAorA = true;
}

void MainWindow::removeAdmin()
{
    if(lastSelectedSAorA)
        superAdminListModel->removeRow(ui->superAdminList->currentIndex().row());
    else
        adminListModel->removeRow(ui->adminList->currentIndex().row());
    rebuildAdminList();
}

void MainWindow::addAdmin()
{
    QStandardItem *item = new QStandardItem();
    item->setText(ui->adminBLID->text());
    adminListModel->appendRow(item);
    ui->adminBLID->clear();
    rebuildAdminList();
}

void MainWindow::addSuperAdmin()
{
    QStandardItem *item = new QStandardItem();
    item->setText(ui->adminBLID->text());
    superAdminListModel->appendRow(item);
    ui->adminBLID->clear();
    rebuildAdminList();
}

void MainWindow::demoteSA()
{
    QStandardItem *item = new QStandardItem();
    item->setText(superAdminListModel->item(ui->superAdminList->currentIndex().row())->text());
    adminListModel->appendRow(item);
    superAdminListModel->removeRow(ui->superAdminList->currentIndex().row());
    rebuildAdminList();
}

void MainWindow::rebuildAdminList()
{
    QString adminList = "\"";
    QString superAdminList = "\"";
    for(int i = 0; i < adminListModel->rowCount(); ++i)
        adminList.append(" ").append(adminListModel->item(i)->text());
    for(int i = 0; i < superAdminListModel->rowCount(); ++i)
        superAdminList.append(" ").append(superAdminListModel->item(i)->text());
    adminList.append("\"");
    superAdminList.append("\"");
    prefListModel->item(adminListIndex, 1)->setText(adminList);
    prefListModel->item(superAdminListIndex, 1)->setText(superAdminList);
}
