#include "mainwindow.h"
#include "ui_mainwindow.h"

//Open file browser for Blockland.exe
void MainWindow::openBLExec()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"), "/", tr("*.exe"));
    if(filename == tr("")) //User hit cancel, ignore
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
    loadColorset();
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

    //Make file path of our file and where to put it
    QString repFile = QDir::currentPath().append("/replace.cs");
    QString repFileNew = (*basePath).trimmed().append("config/main.cs");
    //Start replacing the files
    if(!QFile::copy(repFile,repFileNew))
    {
        updateStatus(tr("Failed to copy ").append(repFile).append(" to ").append(repFileNew));
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
    //Make file path of our file and where to put it
    QString repFile = (*basePath).trimmed().append("config/main.cs");
    //Start replacing the files
    if(!QFile::remove(repFile))
        updateStatus(tr("Failed to remove ").append(repFile));
}

//Spits out error to statusbar is something went bad
void MainWindow::processError(QProcess::ProcessError err)
{
    if(err == QProcess::FailedToStart)
        updateStatus("Failed to start server");
    if(err == QProcess::Crashed)
        updateStatus("Server crashed");
    if(err == QProcess::Timedout)
        updateStatus("Server timed out");
    if(err == QProcess::WriteError)
        updateStatus("Write error");
    if(err == QProcess::ReadError)
        updateStatus("Read error");
    if(err == QProcess::UnknownError)
        updateStatus("Something went and borked (AKA unknown error)");
    return;
}

//Update the textbox with the console output
void MainWindow::updateOutput()
{
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

    //Blockland doesn't output anything special for warn() or error() to console, so we emulate it partially by using some default phrases
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