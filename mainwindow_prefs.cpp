#include "mainwindow.h"
#include "ui_mainwindow.h"

//Load prefs file and create list view from it.
void MainWindow::loadPrefList()
{
    //Clear list first
    prefListModel->removeRows(0,prefListModel->rowCount());

    QString path = (*basePath).trimmed().append("config/server/prefs.cs");

    //Load the file
    QFile add;
    add.setFileName(path);
    if(!add.open(QIODevice::ReadOnly)) //First open file to only read, we'll rewrite the file later
    {
        updateStatus(tr("Unable to open ").append(path).append(" !"));
        return;
    }

    unsigned i = 0;
    //Loop through file
    while(!add.atEnd())
    {
        //Get line, then check if empty
        QString line = add.readLine();
        if(line.trimmed() == tr(""))
            break;
        //delete end ;
        line = line.remove(";");
        line.replace(" = ", " ");
        int split = line.indexOf(" ");
        QString var = line.mid(0, split);
        QString val = line.mid(split, line.size() - split).trimmed();

        //Get special prefs
        if(var == "$Pref::Server::AutoSuperAdminList")
            updateSAList(val);
        if(var == "$Pref::Server::AutoAdminList")
            updateAList(val);

        //Make item and add it to list
        QStandardItem *item = new QStandardItem();
        item->setText(var);
        prefListModel->setItem(i, 0, item);
        item = new QStandardItem();
        item->setText(val);
        prefListModel->setItem(i, 1, item);
        ++i;
    }

    add.close();

    updateStatus("Prefs list successfully imported!");
}

//Save add-on list to ADD_ON_LIST.cs
void MainWindow::savePrefList()
{
    QString path = (*basePath).trimmed().append("config/server/prefs.cs");

    //Delete existing file to start fresh
    if(QFile::exists(path))
        QFile::remove(path);

    //Load the file
    QFile add;
    add.setFileName(path);
    if(!add.open(QIODevice::WriteOnly)) //Try to open file
    {
        updateStatus(tr("Unable to open ").append(path).append(" !"));
        return;
    }

    //Loop through addons
    for(int i = 0; i < prefListModel->rowCount(); ++i)
    {
        //Assemble line text
        QString line = prefListModel->item(i, 0)->text().trimmed().append(" = ").append(prefListModel->item(i, 1)->text()).append(";\r\n");

        //Write line to file
        add.write(line.toStdString().c_str());
    }

    add.close();

    updateStatus("Pref list successfully saved!");
}
