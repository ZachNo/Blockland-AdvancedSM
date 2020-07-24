#include "mainwindow.h"
#include "ui_mainwindow.h"

//MainWindow constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //Initialize server to NULL
    server = nullptr;

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

    //Link SQLite Database
    //db = nullptr;
    //db = new ABMDatabase(logText);
    //db->initDatabase();

    //Connect server buttons to functions
    connect(ui->searchExec, SIGNAL(clicked(bool)), this, SLOT(openBLExec()));
    connect(ui->saveFileBrowse, SIGNAL(clicked(bool)), this, SLOT(openSavefile()));
    connect(ui->startServer, SIGNAL(clicked(bool)), this, SLOT(startServer()));
    connect(ui->stopServer, SIGNAL(clicked(bool)), this, SLOT(stopServer()));
    connect(ui->killServer, SIGNAL(clicked(bool)), this, SLOT(killServer()));
    keyPress = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(keyPress, SIGNAL(activated()), this, SLOT(sendCommand()));
    connect(ui->sendCommand, SIGNAL(clicked(bool)), this, SLOT(sendCommand()));
    ui->chatBox->hide();
    connect(ui->onlySeeChat, SIGNAL(clicked(bool)), this, SLOT(changeOutput(bool)));
    connect(ui->useSteam, SIGNAL(clicked(bool)), this, SLOT(useSteamOrToken(bool)));

    //Initialize Addon List Model
    addonListModel = new QStandardItemModel;
    ui->addonsView->setModel(addonListModel);
    connect(ui->addonsSave, SIGNAL(clicked(bool)), this, SLOT(saveAddonList()));
    connect(ui->addonsAll, SIGNAL(clicked(bool)), this, SLOT(allAddons()));
    connect(ui->addonsNone, SIGNAL(clicked(bool)), this, SLOT(noAddons()));
    connect(ui->addonsDefault, SIGNAL(clicked(bool)), this, SLOT(defaultAddons()));
    connect(ui->addonsReload, SIGNAL(clicked(bool)), this, SLOT(loadAddonList()));

    //Initialize Music list model
    musicListModel = new QStandardItemModel;
    ui->musicView->setModel(musicListModel);
    connect(ui->musicSave, SIGNAL(clicked(bool)), this, SLOT(saveMusicList()));
    connect(ui->musicAll, SIGNAL(clicked(bool)), this, SLOT(allMusic()));
    connect(ui->musicNone, SIGNAL(clicked(bool)), this, SLOT(noMusic()));
    connect(ui->musicDefault, SIGNAL(clicked(bool)), this, SLOT(defaultMusic()));
    connect(ui->musicReload, SIGNAL(clicked(bool)), this, SLOT(loadMusicList()));

    //Colorset
    colorsetModel = new QStandardItemModel;
    ui->colorSetTable->setModel(colorsetModel);
    connect(ui->setColorset, SIGNAL(clicked(bool)), this, SLOT(saveColorset()));
    connect(ui->allColorsets, SIGNAL(currentIndexChanged(int)), this, SLOT(loadColorset()));

    //Banlist
    banlistModel = new QStandardItemModel;
    ui->banlistTable->setModel(banlistModel);
    banlistModel->setHorizontalHeaderLabels(
        QStringList() << tr("Admin")
                      << tr("Admin BLID")
                      << tr("Name")
                      << tr("BLID")
                      << tr("IP")
                      << tr("Reason")
                      << tr("Expiration Year")
                      << tr("Expiration Minute"));
    ui->banlistTable->verticalHeader()->hide();
    ui->banlistTable->horizontalHeader()->setStretchLastSection(true);
    ui->banlistTable->setColumnWidth(0,125);
    ui->banlistTable->setColumnWidth(1,75);
    ui->banlistTable->setColumnWidth(2,125);
    ui->banlistTable->setColumnWidth(3,75);
    ui->banlistTable->setColumnWidth(4,100);
    ui->banlistTable->setColumnWidth(5,300);
    ui->banlistTable->setColumnWidth(6,125);
    ui->banlistTable->setColumnWidth(7,125);
    connect(ui->refreshBanlist, SIGNAL(clicked(bool)), this, SLOT(loadBanlist()));
    connect(ui->saveBanlist, SIGNAL(clicked(bool)), this, SLOT(saveBanlist()));
    connect(ui->removeBanBtn, SIGNAL(clicked(bool)), this, SLOT(removeBan()));
    connect(ui->addBanBtn, SIGNAL(clicked(bool)), this, SLOT(addBan()));
    addBanW = new AddbanWindow(this);
    addBanW->setWindowFlags(Qt::Tool);
    addBanW->hide();

    //Flagged items category
    flaggedListModel = new QStandardItemModel;
    ui->flaggedItemsView->setModel(flaggedListModel);
    flaggedListModel->setHorizontalHeaderLabels(QStringList() << tr("Add-on Name") << tr("Reason"));
    ui->flaggedItemsView->verticalHeader()->hide();
    ui->flaggedItemsView->horizontalHeader()->setStretchLastSection(true);
    ui->flaggedItemsView->setColumnWidth(0,400);
    connect(ui->scanForIssues, SIGNAL(clicked(bool)), this, SLOT(scanForFlagged()));

    //Server prefs list
    prefListModel = new QStandardItemModel;
    ui->prefListView->setModel(prefListModel);
    prefListModel->setHorizontalHeaderLabels(QStringList() << tr("Variable Name") << tr("Value"));
    ui->prefListView->verticalHeader()->hide();
    ui->prefListView->horizontalHeader()->setStretchLastSection(true);
    ui->prefListView->setColumnWidth(0,300);
    connect(ui->savePrefsBtn, SIGNAL(clicked(bool)), this, SLOT(savePrefList()));
    connect(ui->advancedPrefs, SIGNAL(toggled(bool)), this, SLOT(toggleAdvancedPref(bool)));
    ui->prefListView->hide();

    //Setup admin list
    adminListModel = new QStandardItemModel;
    superAdminListModel = new QStandardItemModel;
    ui->adminList->setModel(adminListModel);
    ui->superAdminList->setModel(superAdminListModel);
    lastSelectedSAorA = false;
    adminListIndex = -1;
    superAdminListIndex = -1;
    connect(ui->removeAdminBtn, SIGNAL(clicked(bool)), this, SLOT(removeAdmin()));
    connect(ui->adminList, SIGNAL(clicked(QModelIndex)), this, SLOT(adminFocused()));
    connect(ui->superAdminList, SIGNAL(clicked(QModelIndex)), this, SLOT(sAdminFocused()));
    connect(ui->addAdminBtn, SIGNAL(clicked(bool)), this, SLOT(addAdmin()));
    connect(ui->addSABtn, SIGNAL(clicked(bool)), this, SLOT(addSuperAdmin()));
    connect(ui->demoteSABtn, SIGNAL(clicked(bool)), this, SLOT(demoteSA()));
    connect(ui->saveAdminsBtn, SIGNAL(clicked(bool)), this, SLOT(savePrefList()));

    //Load possible window styles
    QStringList styles = QStyleFactory::keys();
    for(int i = 0; i < styles.size(); ++i)
        ui->themePicker->addItem(styles[i]);
    connect(ui->themePicker, SIGNAL(currentIndexChanged(int)), this, SLOT(setTheme()));

    //Try to load config file
    loadSettings();
}

//MainWindow destructor
MainWindow::~MainWindow()
{
    saveSettings();

    if(server && server->state() != QProcess::NotRunning)
    {
        server->kill();
        server->waitForFinished();
    }

    delete model;
    delete addonListModel;
    delete banlistModel;
    delete colorsetModel;
    delete flaggedListModel;
    delete musicListModel;
    delete prefListModel;
    delete adminListModel;
    delete superAdminListModel;
    delete connection;
    delete logW;
    delete addBanW;
    delete aboutW;
    //delete db;
    delete keyPress;
    delete server;
    delete basePath;
    //delete logText; deleting causes crash on exit
    delete ui;
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

//Add message to log and display on status bar
void MainWindow::updateStatus(QString mes)
{
    ui->statusBar->showMessage(mes);
    logText->append("\n");
    logText->append(mes);
    logW->updateLogForce();
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
        exportTxt.append("1 ");
    else
        exportTxt.append("0 ");
    if(ui->useSteam->isChecked())
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

    //Export dtoken
    exportTxt = tr("dtoken ").append(ui->dtoken->text()).append("\r\n");
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

    QFile load;
    load.setFileName(path);
    if(!load.open(QIODevice::ReadOnly))
    {
        updateStatus("Unable to open config.ini for read!");
        return false;
    }

    //Start saving things
    QString importTxt;

    //Import bl.exe path
    importTxt = load.readLine();
    ui->blExec->setText(importTxt.remove("BLExecPath ").trimmed());
    basePathBuild();

    //import gamemode
    importTxt = load.readLine();
    int gamemode = ui->gamemodeBox->findText(importTxt.remove("ServerGame ").trimmed());
    if(gamemode == -1)
    {
        ui->gamemodeBox->setCurrentIndex(0);
        updateStatus("Failed to find saved gamemode!");
    }
    else
        ui->gamemodeBox->setCurrentIndex(gamemode);

    //import simple server settings
    importTxt = load.readLine();
    ui->maxPlayers->setValue(importTxt.section(' ',1,1).toInt());
    ui->portNumber->setValue(importTxt.section(' ',2,2).toInt());
    ui->lanDed->setChecked(importTxt.section(' ',3,3).toInt());
    ui->useSteam->setChecked(importTxt.section(' ',4,4).toInt());

    //import server name
    importTxt = load.readLine();
    ui->serverName->setText(importTxt.remove("ServerName ").trimmed());

    //import server save file
    importTxt = load.readLine();
    ui->saveFile->setText(importTxt.remove("ServerSave ").trimmed());

    //import dtoken
    importTxt = load.readLine();
    ui->dtoken->setText(importTxt.remove("dtoken ").trimmed());

    load.close();
    return true;
}

void MainWindow::setTheme()
{
    this->app->setStyle(QStyleFactory::create(ui->themePicker->currentText()));
    updateStatus("Window Theme set");
}
