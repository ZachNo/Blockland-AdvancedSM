#include "mainwindow.h"
#include "ui_mainwindow.h"

//MainWindow constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //Initialize server to NULL
    server = NULL;

    //Set basepath to nothing
    basePath = new QString;

    //Populate GUI based off the .ui file
    ui->setupUi(this);

    //Create the Log window, Qt was being dumb so populate this ourselves
    logW = new LogWindow(this);
    logW->setWindowFlags(Qt::Tool);
    logW->hide();

    logText = &logW->logText;
    logText->append("Blockland: Advanced Server Manager\nCreated by ZSNO");

    //Create about window
    aboutW = new AboutWindow(this);
    aboutW->setWindowFlags(Qt::Tool);
    aboutW->hide();

    //Create the container for the current player data
    model = new QStandardItemModel;
    model->setHorizontalHeaderLabels(
        QStringList() << tr("Admin")
                      << tr("Name")
                      << tr("BLID")
                      << tr("Ping")
                      << tr("Clan Prefix")
                      << tr("Clan Suffix")
                      << tr("Score")
                      << tr("IP Address"));

    ui->players_table->setModel(model);
    ui->players_table->verticalHeader()->hide();
    ui->players_table->horizontalHeader()->setStretchLastSection(true);
    ui->players_table->setColumnWidth(0,50);
    ui->players_table->setColumnWidth(1,175);
    ui->players_table->setColumnWidth(2,75);
    ui->players_table->setColumnWidth(3,50);
    ui->players_table->setColumnWidth(4,75);
    ui->players_table->setColumnWidth(5,75);
    ui->players_table->setColumnWidth(6,75);

    //Try to connect to Blockland Server
    connection = new ServerConnection(this);

    //Add tool bar stuff
    connect(ui->actionReconnect, SIGNAL(triggered(bool)), connection, SLOT(reconnect()));
    connect(ui->actionLog_Window, SIGNAL(triggered(bool)), this, SLOT(openLog()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(openAbout()));

    //Add gamemodes
    ui->gamemodeBox->addItem("Select a Gamemode...");
    //Function to search for Gamemodes

    //Add types of searches to combo box
    //ui->searchType->addItem("BLID");
    //ui->searchType->addItem("IP");
    //ui->searchType->addItem("Name");

    //Link SQLite Database
    //db = new ABMDatabase(logText);
    //db->initDatabase();

    //Connect buttons to functions
    connect(ui->searchExec, SIGNAL(clicked(bool)), this, SLOT(openBLExec()));
    connect(ui->saveFileBrowse, SIGNAL(clicked(bool)), this, SLOT(openSavefile()));
    connect(ui->startServer, SIGNAL(clicked(bool)), this, SLOT(startServer()));
    connect(ui->stopServer, SIGNAL(clicked(bool)), this, SLOT(stopServer()));
    connect(ui->killServer, SIGNAL(clicked(bool)), this, SLOT(killServer()));
    keyPress = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(keyPress, SIGNAL(activated()), this, SLOT(sendCommand()));
    connect(ui->sendCommand, SIGNAL(clicked(bool)), this, SLOT(sendCommand()));
    connect(ui->addonsSave, SIGNAL(clicked(bool)), this, SLOT(saveAddonList()));
    connect(ui->addonsAll, SIGNAL(clicked(bool)), this, SLOT(allAddons()));
    connect(ui->addonsNone, SIGNAL(clicked(bool)), this, SLOT(noAddons()));
    connect(ui->addonsDefault, SIGNAL(clicked(bool)), this, SLOT(defaultAddons()));
    connect(ui->addonsReload, SIGNAL(clicked(bool)), this, SLOT(loadAddonList()));

    //Initialize Addon List Model
    addonListModel = new QStandardItemModel;
    ui->addonsView->setModel(addonListModel);

    //Try to load config file
    loadSettings();
}

//MainWindow destructor
MainWindow::~MainWindow()
{
    saveSettings();

    if(server && server->state() != QProcess::NotRunning)
        server->kill();

    delete ui;
    delete model;
    delete addonListModel;
    delete connection;
    delete logW;
    //delete db;
    delete keyPress;
    delete basePath;
    delete aboutW;
}

//Open log window
void MainWindow::openLog()
{
    logW->show();
}

//Open about window
void MainWindow::openAbout()
{
    aboutW->show();
}

//Open file browser for Blockland.exe
void MainWindow::openBLExec()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"), "/", tr("*.exe"));
    if(filename == tr(""))
        return;
    ui->blExec->setText(filename);
    basePathBuild();
    saveSettings();
}

//Set base path and build gamemode list
void MainWindow::basePathBuild()
{
    //Get base path from the path to Blockland.exe
    *basePath = ui->blExec->text().trimmed().replace('\\','/').replace("Blockland.exe","");
    //QFile only accepts / as directory seperator, get rid of Blockland.exe
    buildGamemodeList();
    loadAddonList();
}

//Open file browser for save files
void MainWindow::openSavefile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"), "/", tr("*.bls"));
    ui->saveFile->setText(filename);
    return;
}

//Start a BL server process
void MainWindow::startServer()
{
    //Ignore if we don't have a .exe seleceted
    if(ui->blExec->text().trimmed() == tr(""))
    {
        updateStatus("Tried to start server without entering an exe.");
        return;
    }

    //Ignore if the file doesn't exist
    if(!QFile::exists(ui->blExec->text().trimmed()))
    {
        updateStatus("Entered exe doesn't exist!");
        return;
    }

    //Ignore if no gamemode selected
    if(ui->gamemodeBox->currentText().trimmed() == tr("Select a Gamemode..."))
    {
        updateStatus("No gamemode has been selected!");
        return;
    }

    //Move main.cs.dso and replace with our modified version to add functionality
    //Make file path of current main.cs.dso and backup
    QString mainFile = (*basePath).trimmed().append("base/main.cs.dso");
    QString mainFileNew = (*basePath).trimmed().append("base/main_backup.cs.dso");
    //Make file path of our file and where to put it
    QString repFile = QDir::currentPath().append("/replace.cs");
    QString repFileNew = (*basePath).trimmed().append("base/main.cs");
    //Start replacing the files
    if(QFile::exists(mainFileNew))
    {
        if(!QFile::remove(mainFileNew))
        {
            updateStatus(tr("Failed to remove ").append(mainFileNew));
            return;
        }
    }
    if(!QFile::copy(mainFile,mainFileNew))
    {
        updateStatus(tr("Failed to copy ").append(mainFile).append(" to ").append(mainFileNew));
        return;
    }
    if(!QFile::copy(repFile,repFileNew))
    {
        updateStatus(tr("Failed to copy ").append(repFile).append(" to ").append(repFileNew));
        return;
    }
    if(!QFile::remove(mainFile))
    {
        updateStatus(tr("Failed to remove ").append(mainFile));
        return;
    }

    //Display and log messages
    updateStatus("Attemping to start Blockland server...");

    //Switch button states
    ui->startServer->setDisabled(1);
    ui->stopServer->setDisabled(0);
    ui->killServer->setDisabled(0);

    //If we already have a server process, delete it
    if(server)
    {
        if(server->state() == QProcess::Running)
            server->kill();
        delete server;
    }

    //Create new server instance, start it with the selected .exe
    server = new QProcess(this);
    server->setProcessChannelMode(QProcess::MergedChannels);

    //Generate args list based on things entered into GUI
    QStringList args;
    args << "ptlaaxobimwroe";
    if(ui->lanDed->isChecked())
        args << "-dedicatedLan";
    else
        args << "-dedicated";
    args << "-port"
         << QString::number(ui->portNumber->value())
         << "-maxPlayers"
         << QString::number(ui->maxPlayers->value())
         << "-serverName"
         << ui->serverName->text().trimmed()
         << "-console";
    if(ui->gamemodeBox->currentText() != tr("Custom"))
        args << "-gamemode" << ui->gamemodeBox->currentText().trimmed();

    if(ui->saveFile->text().trimmed() != tr("") && ui->gamemodeBox->currentText() == tr("Custom"))
        args << "-loadBLS" << ui->saveFile->text().trimmed();

    //Start server process with everything
    server->start(ui->blExec->text().trimmed(), args);
    serverStarting = true;

    //Connect it to update other things
    connect(server,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));
    connect(server,SIGNAL(readyRead()),this,SLOT(updateOutput()));
    connect(server,SIGNAL(finished(int)),this,SLOT(serverStopped()));

    return;
}

//Ask server to stop
void MainWindow::stopServer()
{
    //Check if the server is running or not
    if(server->state() != QProcess::NotRunning)
        connection->sendCommand("quit();");
    ui->stopServer->setDisabled(1);
    updateStatus("Attempting to stop server...");
    return;
}

//Called when server process stops
void MainWindow::serverStopped()
{
    ui->stopServer->setDisabled(1);
    ui->startServer->setDisabled(0);
    ui->killServer->setDisabled(1);
    delete server;
    server = NULL;
    updateStatus("Server stopped successfully");
}

//Called when kill button is clicked
void MainWindow::killServer()
{
    //Check if the server is running or not
    if(server->state() != QProcess::NotRunning)
        server->kill();
    ui->stopServer->setDisabled(1);
    ui->killServer->setDisabled(1);
    updateStatus("Attempting to kill server...");
    return;
}

//Replace files that we messed with previously
void MainWindow::cleanFiles()
{
    //Make file path of current main.cs.dso and backup
    QString mainFileNew = (*basePath).trimmed().append("base/main.cs.dso");
    QString mainFile = (*basePath).trimmed().append("base/main_backup.cs.dso");
    //Make file path of our file and where to put it
    QString repFile = (*basePath).trimmed().append("base/main.cs");
    //Start replacing the files
    if(!QFile::copy(mainFile,mainFileNew))
        updateStatus(tr("Failed to copy ").append(mainFile).append(" to ").append(mainFileNew));
    if(!QFile::remove(mainFile))
        updateStatus(tr("Failed to remove ").append(mainFile));
    if(!QFile::remove(repFile))
        updateStatus(tr("Failed to remove ").append(repFile));
}

//Spits out error to statusbar is something went bad
void MainWindow::processError(QProcess::ProcessError err)
{
    if(err == QProcess::FailedToStart)
        updateStatus("Failed to start server");
    if(err == QProcess::Crashed)
        updateStatus("Server crashed?");
    if(err == QProcess::Timedout)
        updateStatus("Server timedout (hung)");
    if(err == QProcess::WriteError)
        updateStatus("Write error");
    if(err == QProcess::ReadError)
        updateStatus("Read error");
    if(err == QProcess::UnknownError)
        updateStatus("Something went and borked");
    return;
}

//Update the textbox with the console output
void MainWindow::updateOutput()
{
    //TODO: Format output to be nicer
    QString output = server->readAllStandardOutput();

    //Trim spaces, as long as it isn't a tab!
    output = output.replace("%","");
    int n = output.size() - 1;
    while(n >= 0)
    {
        if (output.at(n).isSpace())
            output.remove(n,1);
        else
            break;
        --n;
    }
    while(output.size())
    {
        if (output.at(0) == tr("\n") || output.at(0) == tr("\r"))
            output.remove(0,1);
        else
            break;
    }

    //Trigger that our stuff is done executing
    if(serverStarting && output.contains("--------- End Executing Custom Code ---------"))
    {
        connection->reconnect();
        cleanFiles();
        serverStarting = false;
    }

    if(output.contains("): ") || output.contains("datablocks added.") || output.contains("will not execute"))
        output = tr("<font color=#777777>").append(output).append("</font>");

    if(output.contains("Loading Add-On: "))
    {
        output = tr("<font color=#0000ff>").append(output).append("</font>");
        int breakPos = output.indexOf("(CRC:");
        output.insert(breakPos,"</font><font color=#777777>");
    }

    if(output.contains("error "))
        output = tr("<font color=#ff0000>").append(output).append("</font>");

    //Finally write it to console
    ui->consoleOutput->append(output);
}

//Add message to log and display on status bar
void MainWindow::updateStatus(QString mes)
{
    ui->statusBar->showMessage(mes);
    logText->append("\n");
    logText->append(mes);
}

//Write command data to stdin
void MainWindow::sendCommand()
{
    if(ui->commandLine->text().trimmed() == tr(""))
        return;
    connection->sendCommand(ui->commandLine->text().trimmed());
    ui->commandLine->clear();
}

//Use basepath, then iterate to find all Gamemodes
void MainWindow::buildGamemodeList()
{    
    //Remove all previous gamemodes...
    while(ui->gamemodeBox->count())
        ui->gamemodeBox->removeItem(0);
    //Add defaults
    ui->gamemodeBox->addItem("Select a Gamemode...");
    ui->gamemodeBox->addItem("Custom");

    //Iterate through add-ons and get gamemodes
    QString path = (*basePath).trimmed().append("Add-ons/");
    QDirIterator iter(path, QDir::Files | QDir::Dirs);
    while (iter.hasNext())
    {
        iter.next();
        if(iter.fileName().toLower().contains("gamemode_"))
        {
            QString gamemode = iter.fileName().remove(0,9); //remove Gamemode_
            if(iter.fileInfo().isFile())
                gamemode = gamemode.remove(gamemode.length()-4,4); //remove .zip
            ui->gamemodeBox->addItem(gamemode);
        }
    }
}

//Save all valued settings to file
//Returns false on failure
bool MainWindow::saveSettings()
{
    //Make path, open the file to write
    QString path = QDir::currentPath().append("/config.ini");

    //Delete config if it exists...
    if(QFile::exists(path))
        QFile::remove(path);

    QFile save;
    save.setFileName(path);
    if(!save.open(QIODevice::WriteOnly))
    {
        updateStatus("Unable to open config.ini for write!");
        return false;
    }


    //Start saving things
    QString exportTxt;

    //Export blockland.exe path
    exportTxt = tr("BLExecPath ").append(ui->blExec->text().trimmed()).append("\r\n");
    save.write(exportTxt.toStdString().c_str());

    //Export gamemode
    exportTxt = tr("ServerGame ").append(ui->gamemodeBox->currentText()).append("\r\n");
    save.write(exportTxt.toStdString().c_str());

    //Export server var fields
    exportTxt = tr("ServerSettings ").append(ui->maxPlayers->text()).append(" ").append(ui->portNumber->text()).append(" ");
    if(ui->lanDed->isChecked())
        exportTxt.append("1\r\n");
    else
        exportTxt.append("0\r\n");
    save.write(exportTxt.toStdString().c_str());

    //Export sever name seperately because it can have spaces
    exportTxt = tr("ServerName ").append(ui->serverName->text()).append("\r\n");
    save.write(exportTxt.toStdString().c_str());

    //Export savefile location
    exportTxt = tr("ServerSave ").append(ui->saveFile->text()).append("\r\n");
    save.write(exportTxt.toStdString().c_str());

    save.close();
    return true;
}

//Load all valued settings from file
//Returns false on failures
bool MainWindow::loadSettings()
{
    //Make path, open the file to write
    QString path = QDir::currentPath().append("/config.ini");
    if(!QFile::exists(path))
    {
        updateStatus("Unable to load config.ini because it doesn't exist.");
        return false;
    }

    QFile save;
    save.setFileName(path);
    if(!save.open(QIODevice::ReadOnly))
    {
        updateStatus("Unable to open config.ini for read!");
        return false;
    }

    //Start saving things
    QString importTxt;

    //Import bl.exe path
    importTxt = save.readLine();
    ui->blExec->setText(importTxt.remove("BLExecPath ").trimmed());
    basePathBuild();

    //import gamemode
    importTxt = save.readLine();
    int gamemode = ui->gamemodeBox->findText(importTxt.remove("ServerGame ").trimmed());
    if(gamemode == -1)
    {
        ui->gamemodeBox->setCurrentIndex(0);
        updateStatus("Failed to find saved gamemode!");
    }
    else
        ui->gamemodeBox->setCurrentIndex(gamemode);

    //import simple server settings
    importTxt = save.readLine();
    ui->maxPlayers->setValue(importTxt.section(' ',1,1).toInt());
    ui->portNumber->setValue(importTxt.section(' ',2,2).toInt());
    ui->lanDed->setChecked(importTxt.section(' ',3,3).toInt());

    //import server name
    importTxt = save.readLine();
    ui->serverName->setText(importTxt.remove("ServerName ").trimmed());

    //import server save file
    importTxt = save.readLine();
    ui->saveFile->setText(importTxt.remove("ServerSave ").trimmed());

    save.close();
    return true;
}

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

    //Commented out because it doesn't work too well
    /*//Try to load other things that may have been added, but not run yet.
    QDirIterator iter(addonPath, QDir::Files | QDir::Dirs);
    while (iter.hasNext())
    {
        iter.next();
        QString filename = iter.fileName();
        QString compFN = filename.toLower();
        if(compFN.contains(".zip"))
            compFN.remove(compFN.length()-4,4);

        //Remove all of the definite not server add-ons from the list
        if(!addonListModel->findItems(compFN,Qt::MatchContains).isEmpty()) //skip addons already in list
            continue;
        if(filename == "Music")
            continue;
        if(compFN.contains("speedkart_"))
            continue;
        if(compFN.contains("sky_"))
            continue;
        if(compFN.contains("map_"))
            continue;
        if(compFN.contains("speedkart_"))
            continue;
        if(compFN.contains("client_"))
            continue;
        if(compFN.contains("colorset_"))
            continue;
        if(compFN.contains("daycycle_"))
            continue;
        if(compFN.contains("decal_"))
            continue;
        if(compFN.contains("face_"))
            continue;
        if(compFN.contains("ground_"))
            continue;
        if(compFN.contains("gui_"))
            continue;
        if(compFN.contains("risinglava_"))
            continue;
        if(compFN.contains("water_"))
            continue;

        //Add item
        QStandardItem *item = new QStandardItem();
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setText(compFN);
        addonListModel->appendRow(item);
    }*/

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
        //Do lots of compares... idk how to do this more efficiently rn (Organized in categories just cause)
        if(txt == "Bot_Blockhead" || txt == "Bot_Hole" || txt == "Bot_Horse" || txt == "Bot_Shark" || txt == "Bot_Zombie")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Brick_Arch" || txt == "Brick_Checkpoint" || txt == "Brick_Christmas_Tree" || txt == "Brick_Doors" || txt == "Brick_Halloween" || txt == "Brick_Large_Cubes" || txt == "Brick_Teledoor" || txt == "Brick_Treasure_Chest" || txt == "Brick_V15")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Emote_Alarm" || txt == "Emote_Confusion" || txt == "Emote_Hate" || txt == "Emote_Love")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Item_Key" || txt == "Item_Skis" || txt == "Item_Sports")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Light_Animated" || txt == "Light_Basic")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Particle_Basic" || txt == "Particle_FX_Cans" || txt == "Particle_Grass" || txt == "Particle_Player" || txt == "Particle_Tools")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Player_Fuel_Jet" || txt == "Player_Jump_Jet" || txt == "Player_Leap_Jet" || txt == "Player_No_Jet" || txt == "Player_Quake")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Print_1x2f_BLPRemote" || txt == "Print_1x2f_Default" || txt == "Print_2x2f_Default" || txt == "Print_2x2r_Default" || txt == "Print_Letters_Default")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Projectile_GravityRocket" || txt == "Projectile_Pinball" || txt == "Projectile_Pong" || txt == "Projectile_Radio_Wave")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Sound_Beeps" || txt == "Sound_Phone" || txt == "Sound_Synth4")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Support_Doors")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Vehicle_Ball" || txt == "Vehicle_Flying_Wheeled_Jeep" || txt == "Vehicle_Horse" || txt == "Vehicle_Jeep" || txt == "Vehicle_Magic_Carpet" || txt == "Vehicle_Pirate_Cannon" || txt == "Vehicle_Rowboat" || txt == "Vehicle_Tank")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else if(txt == "Weapon_Bow" || txt == "Weapon_Gun" || txt == "Weapon_Guns_Akimbo" || txt == "Weapon_Horse_Ray" || txt == "Weapon_Push_Broom" || txt == "Weapon_Rocket_Launcher" || txt == "Weapon_Spear" || txt == "Weapon_Sword")
            addonListModel->item(i)->setCheckState(Qt::Checked);
        else
            addonListModel->item(i)->setCheckState(Qt::Unchecked);
    }
}

