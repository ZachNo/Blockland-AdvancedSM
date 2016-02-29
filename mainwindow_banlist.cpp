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
        QString line = ban.readLine();
        if(line.trimmed() == tr(""))
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
