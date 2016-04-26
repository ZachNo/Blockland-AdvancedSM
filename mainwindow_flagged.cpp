#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unordered_map>
#include <string>

// Reimplemented Torque CRC function
int blCRC(QString fileName)
{
    static unsigned crcTable[256];
    static bool crcValid = false;

    if(!crcValid)
    {
        unsigned val = 0;
        for(int i = 0; i < 256; i++)
        {
            val = i;
            for(int j = 0; j < 8; j++)
            {
                if(val & 0x01)
                    val = 0xedb88320 ^ (val >> 1);
                else
                    val = val >> 1;
            }
            crcTable[i] = val;
        }
        crcValid = true;
    }

    QFile f(fileName);
    if(!f.open(QFile::ReadOnly))
        return 0;

    char c[1];
    unsigned crcVal = 0xffffffff;
    while(f.read(c,1))
        crcVal = crcTable[(crcVal ^ *c) & 0xff] ^ (crcVal >> 8);

    return static_cast<int>(crcVal);
}

void MainWindow::scanForFlagged()
{
    QString addonPath = (*basePath).trimmed().append("Add-ons/");

    std::unordered_map<int, char> badCRCs;
    std::unordered_map<std::string, char> badNames;
    QFile crcFile("./crapOns.txt");

    if(!crcFile.open(QFile::ReadOnly))
    {
        updateStatus("Failed to open bad CRCs file!");
        return;
    }

    flaggedListModel->removeRows(0,flaggedListModel->rowCount());

    QString line;
    bool crcOrName = false;
    while(!crcFile.atEnd())
    {
        line = crcFile.readLine();
        if(line[0] == '#')
        {
            if(line.size() > 1 && line[1] == '#')
                crcOrName = !crcOrName;
            continue;
        }
        if(crcOrName)
            badCRCs.emplace(line.toInt(), 0);
        else
            badNames.emplace(line.trimmed().toLower().toStdString(), 0);
    }
    crcFile.close();

    //Iterate through add-ons and get gamemodes
    QDirIterator iter(addonPath, QDir::Files | QDir::Dirs);
    unsigned i = 0;
    while (iter.hasNext())
    {
        iter.next();
        QString fileName(iter.fileName().toLower());
        int crc = 0;
        if(iter.fileInfo().isFile())
        {
            crc = blCRC(addonPath.trimmed().append(fileName));
            fileName.chop(4);
        }
        if(badNames.find(fileName.toStdString()) != badNames.end())
        {
            QStandardItem *item = new QStandardItem();
            item->setText(fileName);
            flaggedListModel->setItem(i, 0, item);
            item = new QStandardItem();
            item->setText("Bad name");
            flaggedListModel->setItem(i, 1, item);
            ++i;
        }
        if(badCRCs.find(crc) != badCRCs.end())
        {
            QStandardItem *item = new QStandardItem();
            item->setText(fileName);
            flaggedListModel->setItem(i, 0, item);
            item = new QStandardItem();
            item->setText("Bad CRC");
            flaggedListModel->setItem(i, 1, item);
            ++i;
        }
    }
    updateStatus("Finished scanning");
}
