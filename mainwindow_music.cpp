#include "mainwindow.h"
#include "ui_mainwindow.h"

//Load ADD_ON_LIST and create list view from it.
void MainWindow::loadMusicList()
{
    //Clear list first
    musicListModel->clear();

    QString path = (*basePath).trimmed().append("config/server/musicList.cs");
    QString musicPath = (*basePath).trimmed().append("Add-ons/Music/");

    //Load the file
    QFile add;
    add.setFileName(path);
    if(!add.open(QIODevice::ReadOnly)) //First open file to only read, we'll rewrite the file later
    {
        updateStatus(tr("Unable to open ").append(path).append(" !"));
        return;
    }

    //Loop through file
    while(!add.atEnd())
    {
        //Get line, then check if empty
        QString line = add.readLine();
        if(line.trimmed() == tr(""))
            break;
        //delete $Music__ and end ;
        line = line.remove(0,8);
        line.chop(1);
        line.replace("DASH","-");

        //See if it should have a check mark or not
        bool checked;
        if(line.contains("-1"))
            checked = false;
        else
            checked = true;

        //Get index of space after addon name
        int firstSpace = line.indexOf(" ");
        //Finalize the string of the addon name
        line = line.remove(firstSpace,line.length() - firstSpace);

        //If add-on isn't in Add-ons folder, ignore value for it
        if(!QFile::exists(musicPath.trimmed().append(line).append(".ogg")))
            continue;

        //Make item and add it to list
        QStandardItem *item = new QStandardItem();
        item->setCheckable(true);
        if(checked)
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        item->setText(line);
        musicListModel->appendRow(item);
    }

    add.close();

    updateStatus("Music list successfully imported!");
}

//Save add-on list to ADD_ON_LIST.cs
void MainWindow::saveMusicList()
{
    QString path = (*basePath).trimmed().append("config/server/musicList.cs");

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
    for(int i = 0; i < musicListModel->rowCount(); ++i)
    {
        //Assemble line text
        QString line = tr("$Music__").append(musicListModel->item(i)->text()).append(" = ");

        //Get checked state
        if(musicListModel->item(i)->checkState() == Qt::Checked)
            line.append("1;\r\n");
        else
            line.append("-1;\r\n");

        //Write line to file
        add.write(line.toStdString().c_str());
    }

    add.close();

    updateStatus("Music list successfully saved!");
}

//Set check for all addons
void MainWindow::allMusic()
{
    for(int i = 0; i < musicListModel->rowCount(); ++i)
        musicListModel->item(i)->setCheckState(Qt::Checked);
}

//Set uncheck for all addons
void MainWindow::noMusic()
{
    for(int i = 0; i < musicListModel->rowCount(); ++i)
        musicListModel->item(i)->setCheckState(Qt::Unchecked);
}

//Set only default addons
void MainWindow::defaultMusic()
{
    QStringList defaults; //Make a list of all the default add-ons...
    defaults << "After_School_Special" << "Ambient_Deep" << "Bass_1" << "Bass_2" << "Bass_3"
             << "Creepy" << "Distort" << "Drums" << "Factory" << "Icy" << "Jungle" << "Peaceful"
             << "Piano_Bass" << "Rock" << "Stress_";
    for(int i = 0; i < musicListModel->rowCount(); ++i)
    {
        QString txt = musicListModel->item(i)->text();
        if(defaults.contains(txt))
            musicListModel->item(i)->setCheckState(Qt::Checked);
        else
            musicListModel->item(i)->setCheckState(Qt::Unchecked);
    }
}
