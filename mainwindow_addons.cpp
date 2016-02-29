#include "mainwindow.h"
#include "ui_mainwindow.h"

//Load ADD_ON_LIST and create list view from it.
void MainWindow::loadAddonList()
{
    //Clear list first
    addonListModel->clear();

    QString path = (*basePath).trimmed().append("config/server/ADD_ON_LIST.cs");
    QString addonPath = (*basePath).trimmed().append("Add-ons/");

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
        //delete $AddOn__ and end ;
        line = line.remove(line.length()-1,1).remove(0,8);

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
        if(!QFile::exists(addonPath.trimmed().append(line)) && !QFile::exists(addonPath.trimmed().append(line).append(".zip")))
            continue;

        //Make item and add it to list
        QStandardItem *item = new QStandardItem();
        item->setCheckable(true);
        if(checked)
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        item->setText(line);
        addonListModel->appendRow(item);
    }

    add.close();

    updateStatus("Add-on list successfully imported!");
}

//Save add-on list to ADD_ON_LIST.cs
void MainWindow::saveAddonList()
{
    QString path = (*basePath).trimmed().append("config/server/ADD_ON_LIST.cs");

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
    for(int i = 0; i < addonListModel->rowCount(); ++i)
    {
        //Assemble line text
        QString line = tr("$AddOn__").append(addonListModel->item(i)->text()).append(" = ");

        //Get checked state
        if(addonListModel->item(i)->checkState() == Qt::Checked)
            line.append("1;\r\n");
        else
            line.append("-1;\r\n");

        //Write line to file
        add.write(line.toStdString().c_str());
    }

    add.close();

    updateStatus("Add-on list successfully saved!");
}

//Set check for all addons
void MainWindow::allAddons()
{
    for(int i = 0; i < addonListModel->rowCount(); ++i)
        addonListModel->item(i)->setCheckState(Qt::Checked);
}

//Set uncheck for all addons
void MainWindow::noAddons()
{
    for(int i = 0; i < addonListModel->rowCount(); ++i)
        addonListModel->item(i)->setCheckState(Qt::Unchecked);
}

//Set only default addons
void MainWindow::defaultAddons()
{
    for(int i = 0; i < addonListModel->rowCount(); ++i)
    {
        QString txt = addonListModel->item(i)->text();
        QStringList defaults; //Make a list of all the default add-ons...
        defaults << "Bot_Blockhead" << "Bot_Hole" << "Bot_Horse" << "Bot_Shark" << "Bot_Zombie"
                 << "Brick_Arch" << "Brick_Checkpoint" << "Brick_Christmas_Tree" << "Brick_Doors" << "Brick_Halloween" << "Brick_Large_Cubes"
                    << "Brick_Teledoor" << "Brick_Treasure_Chest" << "Brick_V15"
                 << "Emote_Alarm" << "Emote_Confusion" << "Emote_Hate" << "Emote_Love"
                 << "Item_Key" << "Item_Skis" << "Item_Sports"
                 << "Light_Animated" << "Light_Basic"
                 << "Particle_Basic" << "Particle_FX_Cans" << "Particle_Grass" << "Particle_Player" << "Particle_Tools"
                 << "Player_Fuel_Jet" << "Player_Jump_Jet" << "Player_Leap_Jet" << "Player_No_Jet" << "Player_Quake"
                 << "Print_1x2f_BLPRemote" << "Print_1x2f_Default" << "Print_2x2f_Default" << "Print_2x2r_Default" << "Print_Letters_Default"
                 << "Projectile_GravityRocket" << "Projectile_Pinball" << "Projectile_Pong" << "Projectile_Radio_Wave"
                 << "Sound_Beeps" << "Sound_Phone" << "Sound_Synth4"
                 << "Support_Doors"
                 << "Vehicle_Ball" << "Vehicle_Flying_Wheeled_Jeep" << "Vehicle_Horse" << "Vehicle_Jeep" << "Vehicle_Magic_Carpet"
                    << "Vehicle_Pirate_Cannon" << "Vehicle_Rowboat" << "Vehicle_Tank"
                 << "Weapon_Bow" << "Weapon_Gun" << "Weapon_Guns_Akimbo" << "Weapon_Horse_Ray" << "Weapon_Push_Broom"
                    << "Weapon_Rocket_Launcher" << "Weapon_Spear" << "Weapon_Sword";
        if(defaults.contains(txt))
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else
            addonListModel->item(i)->setCheckState(Qt::Unchecked);
    }
}
