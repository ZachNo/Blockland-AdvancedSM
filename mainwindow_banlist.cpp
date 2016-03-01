#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::loadBanlist()
{
    //Clear list first
    banlistModel->removeRows(0,banlistModel->rowCount());

    QString path = (*basePath).trimmed().append("config/server/BANLIST.txt");

    //Load the file
    QFile ban;
    ban.setFileName(path);
    if(!ban.open(QIODevice::ReadOnly)) //First open file to only read, we'll rewrite the file later
    {
        updateStatus(tr("Unable to open ").append(path).append(" !"));
        return;
    }

    //Loop through file
    int row = 0;
    while(!ban.atEnd())
    {
        //Get line, then check if empty
        QString line = ban.readLine().trimmed();
        if(line == tr(""))
            continue;
        QStringList values = line.split('\t');

        for(int i = 0; i < values.size(); ++i)
        {
            QStandardItem *item = new QStandardItem();
            item->setText(values[i]);
            banlistModel->setItem(row, i, item);
        }
        ++row;
    }

    ban.close();

    updateStatus("Banlist successfully imported!");
}

void MainWindow::saveBanlist()
{
    QString path = (*basePath).trimmed().append("config/server/BANLIST.txt");

    //Delete existing file to start fresh
    if(QFile::exists(path))
        QFile::remove(path);

    //Load the file
    QFile ban;
    ban.setFileName(path);
    if(!ban.open(QIODevice::WriteOnly)) //Try to open file
    {
        updateStatus(tr("Unable to open ").append(path).append(" !"));
        return;
    }
    //Loop through addons
    for(int i = 0; i < banlistModel->rowCount(); ++i)
    {
        QString line;
        for(int j = 0; j < banlistModel->columnCount(); ++j)
        {
            //Assemble line text
            QStandardItem *item = banlistModel->item(i,j);
            line.append(item->text()).append("\t");
        }
        line.remove(line.size()-1,1).append("\r\n");
        //Write line to file
        ban.write(line.toStdString().c_str());
    }
    ban.close();

    updateStatus("Banlist successfully saved!");
}
