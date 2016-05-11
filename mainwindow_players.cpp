#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::updatePlayers(QString input)
{
    //Clear list first
    model->removeRows(0, model->rowCount());

    if(input.trimmed() == tr(""))
        return;

    QStringList players = input.split('|');

    //Loop through file
    for(int i = 0; i < players.size(); ++i)
    {
        QStringList values = players[i].split('\t');
        for(int j = 0; j < values.size(); ++j)
        {
            QStandardItem *item = new QStandardItem();
            item->setText(values[j]);
            model->setItem(i, j, item);
        }
    }
}
