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

    ui->pref_serverName->disconnect();
    ui->pref_maxPlayers->disconnect();
    ui->pref_APass->disconnect();
    ui->pref_SAPass->disconnect();
    ui->pref_joinPass->disconnect();
    ui->pref_brickLimit->disconnect();
    ui->pref_mbs->disconnect();
    ui->pref_tooFar->disconnect();
    ui->pref_chatLen->disconnect();
    ui->pref_randColor->disconnect();
    ui->pref_tardFilter->disconnect();
    ui->pref_fallDamage->disconnect();
    ui->pref_randColor->disconnect();
    ui->pref_pSchedules->disconnect();
    ui->pref_pMisc->disconnect();
    ui->pref_pProjectiles->disconnect();
    ui->pref_pItems->disconnect();
    ui->pref_pLights->disconnect();
    ui->pref_pBots->disconnect();
    ui->pref_pVehicles->disconnect();
    ui->pref_gVehicles->disconnect();
    ui->pref_gBots->disconnect();

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
        {
            updateSAList(val);
            superAdminListIndex = i;
        }
        else if(var == "$Pref::Server::AutoAdminList")
        {
            updateAList(val);
            adminListIndex = i;
        }

        //Make item and add it to list
        PrefItem *item = new PrefItem();
        item->setText(var);
        prefListModel->setItem(i, 0, item);
        item = new PrefItem();
        item->setText(val);
        prefListModel->setItem(i, 1, item);

        val.remove("\"");
        //Initialize connections (clusterfuck of variable names)
        //There's a better way to do this, but I want function right now.
        //(Especially since they're all similar!)
        if(var == "$Pref::Server::Name")
        {
            connect(ui->pref_serverName, SIGNAL(textEdited(QString)), item, SLOT(updateText(QString)));
            ui->pref_serverName->setText(val);
        }
        else if(var == "$Pref::Server::MaxPlayers")
        {
            connect(ui->pref_maxPlayers, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_maxPlayers->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::AdminPassword")
        {
            connect(ui->pref_APass, SIGNAL(textEdited(QString)), item, SLOT(updateText(QString)));
            ui->pref_APass->setText(val);
        }
        else if(var == "$Pref::Server::SuperAdminPassword")
        {
            connect(ui->pref_SAPass, SIGNAL(textEdited(QString)), item, SLOT(updateText(QString)));
            ui->pref_SAPass->setText(val);
        }
        else if(var == "$Pref::Server::Password")
        {
            connect(ui->pref_joinPass, SIGNAL(textEdited(QString)), item, SLOT(updateText(QString)));
            ui->pref_joinPass->setText(val);
        }
        else if(var == "$Pref::Server::BrickLimit")
        {
            connect(ui->pref_brickLimit, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_brickLimit->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::MaxBricksPerSecond")
        {
            connect(ui->pref_mbs, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_mbs->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::TooFarDistance")
        {
            connect(ui->pref_tooFar, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_tooFar->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::MaxChatLen")
        {
            connect(ui->pref_chatLen, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_chatLen->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::RandomBrickColor")
        {
            connect(ui->pref_randColor, SIGNAL(toggled(bool)), item, SLOT(updateText(bool)));
            ui->pref_randColor->setChecked(val.toInt());
        }
        else if(var == "$Pref::Server::ETardFilter")
        {
            connect(ui->pref_tardFilter, SIGNAL(toggled(bool)), item, SLOT(updateText(bool)));
            ui->pref_tardFilter->setChecked(val.toInt());
        }
        else if(var == "$Pref::Server::FallingDamage")
        {
            connect(ui->pref_fallDamage, SIGNAL(toggled(bool)), item, SLOT(updateText(bool)));
            ui->pref_fallDamage->setChecked(val.toInt());
        }
        else if(var == "$Pref::Server::RandomBrickColor")
        {
            connect(ui->pref_randColor, SIGNAL(toggled(bool)), item, SLOT(updateText(bool)));
            ui->pref_randColor->setChecked(val.toInt());
        }
        else if(var == "$Pref::Server::Quota::Schedules")
        {
            connect(ui->pref_pSchedules, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_pSchedules->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::Quota::Misc")
        {
            connect(ui->pref_pMisc, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_pMisc->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::Quota::Projectile")
        {
            connect(ui->pref_pProjectiles, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_pProjectiles->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::Quota::Item")
        {
            connect(ui->pref_pItems, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_pItems->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::Quota::Environment")
        {
            connect(ui->pref_pLights, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_pLights->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::Quota::Player")
        {
            connect(ui->pref_pBots, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_pBots->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::Quota::Vehicle")
        {
            connect(ui->pref_pVehicles, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_pVehicles->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::MaxPhysVehicles_Total")
        {
            connect(ui->pref_gVehicles, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_gVehicles->setValue(val.toInt());
        }
        else if(var == "$Pref::Server::MaxPlayerVehicles_Total")
        {
            connect(ui->pref_gBots, SIGNAL(valueChanged(QString)), item, SLOT(updateText(QString)));
            ui->pref_gBots->setValue(val.toInt());
        }

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

    if(connection->isConnected())
        connection->sendCommand("exec(\"config/server/prefs.cs\");");

    updateStatus("Pref list successfully saved!");
}

void MainWindow::toggleAdvancedPref(bool onOff)
{
    if(onOff)
        ui->prefListView->show();
    else
        ui->prefListView->hide();
}

void PrefItem::updateText(QString data)
{
    setText(data.prepend("\"").append("\""));
}

void PrefItem::updateText(bool tf)
{
    setText(tf?"1":"0");
}
