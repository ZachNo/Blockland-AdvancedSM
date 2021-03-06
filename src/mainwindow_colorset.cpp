#include "mainwindow.h"
#include "ui_mainwindow.h"

QColor getColor(QString line)
{
    QColor color;
    QStringList values = line.split(' ');
    if(values[0].contains("."))
    {
        //If file uses 0-1 fomart, set floats
        color.setRgbF(values[0].toFloat(),
                values[1].toFloat(),
                values[2].toFloat(),
                values[3].toFloat());
        return color;
    }
    //File uses 0-255 format, set ints
    color.setRgb(values[0].toInt(),
            values[1].toInt(),
            values[2].toInt(),
            values[3].toInt());
    return color;
}

void MainWindow::parseColorsetLine(QString line, int &row, int &column)
{
    if(line.contains("DIV:"))
    {
        QStandardItem *item = new QStandardItem();
        item->setToolTip(line);
        item->setSelectable(false);
        item->setEditable(false);
        ui->colorSetTable->setColumnWidth(column, 36);
        ui->colorSetTable->setRowHeight(row, 36);
        colorsetModel->setItem(row, column, item);
        ++column;
        row = -1;
    }
    else
    {
        QStandardItem *item = new QStandardItem();
        QImage image(":/checkerboard.png");
        QPainter painter(&image);
        QColor actualColor = getColor(line);
        painter.fillRect(0,0,38,38,QBrush(actualColor));
        item->setToolTip(tr(" ").prepend(QString::number(actualColor.red()))
                         .append(QString::number(actualColor.green())).append(" ")
                         .append(QString::number(actualColor.blue())).append(" ")
                         .append(QString::number(actualColor.alpha())));
        item->setBackground(QBrush(image));
        item->setSelectable(false);
        item->setEditable(false);
        colorsetModel->setItem(row, column, item);
        ui->colorSetTable->setRowHeight(row, 36);
    }
    ++row;
}

void MainWindow::loadColorset()
{    
    static bool firstLoad = true;
    QString addonPath = (*basePath).trimmed().append("Add-ons/");

    if(firstLoad)
    {
        //It will trigger it again if we leave it connected
        ui->allColorsets->disconnect();

        //Remove all previous gamemodes...
        ui->allColorsets->clear();
        ui->allColorsets->addItem("Current colorset");

        //Iterate through add-ons and get gamemodes
        QDirIterator iter(addonPath, QDir::Files | QDir::Dirs);
        while (iter.hasNext())
        {
            iter.next();
            if(iter.fileName().toLower().contains("colorset_"))
            {
                QString colorset = iter.fileName().remove(0,9); //remove colorset_
                if(iter.fileInfo().isFile())
                    colorset = colorset.remove(colorset.length()-4,4); //remove .zip
                ui->allColorsets->addItem(colorset);
            }
        }

        //Reconnect because we're done changing stuff
        connect(ui->allColorsets, SIGNAL(currentIndexChanged(int)), this, SLOT(loadColorset()));
        firstLoad = false;
    }

    //Clear list first
    colorsetModel->clear();

    QString path;

    //If we are loading the first set, or selecting the server colorset, just load the config file
    if(firstLoad || ui->allColorsets->currentIndex() == 0)
        path = (*basePath).trimmed().append("config/server/colorSet.txt");
    else
    {
        //Now we have to find the corresponding file from the combo box since we don't know if it's a zip
        QDirIterator findIter(addonPath, QDir::Files | QDir::Dirs);
        while (findIter.hasNext())
        {
            findIter.next();
            if(findIter.fileName().contains(ui->allColorsets->currentText()))
                break;
        }
        QString colorsetPath = findIter.path().append(findIter.fileName());
        if(findIter.fileInfo().isFile())
        { //It's a zip, we have to handle this differently...
            QuaZip infile(colorsetPath);
            if(!infile.open(QuaZip::mdUnzip))
            {
                updateStatus(colorsetPath.prepend("Unable to open colorset zip at "));
                infile.close();
                return;
            }
            if(!infile.setCurrentFile("colorset.txt",QuaZip::csInsensitive))
            {
                updateStatus("Unable to set colorset.txt in zip");
                infile.close();
                return;
            }
            QuaZipFile colorsetFile(&infile);
            if(!colorsetFile.open(QIODevice::ReadOnly))
            {
                updateStatus("Unable to open colorset.txt in zip");
                colorsetFile.close();
                infile.close();
                return;
            }
            //Do the same as normal, but read from the zipped file
            int row = 0;
            int column = 0;
            while(!colorsetFile.atEnd())
            {
                //Get line, then check if empty
                QString line = colorsetFile.readLine().trimmed();
                if(line == tr(""))
                    continue;

                parseColorsetLine(line, row, column);
            }
            colorsetFile.close();
            infile.close();
            updateStatus("Colorset successfully imported!");
            return;
        }
        else
            path = colorsetPath.append("/colorSet.txt");
    }

    //Load the file
    QFile color;
    color.setFileName(path);
    if(!color.open(QIODevice::ReadOnly)) //First open file to only read, we'll rewrite the file later
    {
        updateStatus(tr("Unable to open ").append(path).append(" !"));
        return;
    }

    //Loop through file
    int row = 0;
    int column = 0;
    while(!color.atEnd())
    {
        //Get line, then check if empty
        QString line = color.readLine().trimmed();
        if(line == tr(""))
            continue;

        parseColorsetLine(line, row, column);
    }

    color.close();

    updateStatus("Colorset successfully imported!");
}

void MainWindow::saveColorset()
{
    QString path = (*basePath).trimmed().append("config/server/colorSet.txt");

    //Delete existing file to start fresh
    if(QFile::exists(path))
        QFile::remove(path);

    //Load the file
    QFile color;
    color.setFileName(path);
    if(!color.open(QIODevice::WriteOnly)) //Try to open file
    {
        updateStatus(tr("Unable to open ").append(path).append(" !"));
        return;
    }
    //Loop through addons
    for(int i = 0; i < colorsetModel->columnCount(); ++i)
    {
        for(int j = 0; j < colorsetModel->rowCount(); ++j)
        {
            //Assemble line text
            QStandardItem *item = colorsetModel->item(j,i);
            if(item == 0)
                continue;
            QString line = item->toolTip();
            if(line.contains("DIV:"))
                line.append("\r\n");
            line.append("\r\n");
            //Write line to file
            color.write(line.toStdString().c_str());
        }
    }
    color.close();

    updateStatus("Colorset successfully saved!");
}

