#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtSql/QtSql>
#include <QShortcut>
#include <QProcess>
#include <QFile>
#include <QDirIterator>
#include <QColor>
#include <QByteArray>
#include <quazip.h>
#include <quazipfile.h>

#include "serverconnection.h"
#include "log.h"
#include "database.h"
#include "about.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateStatus(QString mes);
    void updatePlayers(QString input);
    Ui::MainWindow *getUI()
    {return ui;}
    void setApp(QApplication *a)
    {app = a;}
    void addMessage(QString message);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QStandardItemModel *addonListModel;
    QStandardItemModel *banlistModel;
    QStandardItemModel *colorsetModel;
    QStandardItemModel *flaggedListModel;
    QStandardItemModel *musicListModel;
    QStandardItemModel *prefListModel;
    QStandardItemModel *adminListModel;
    QStandardItemModel *superAdminListModel;
    ServerConnection *connection;
    LogWindow *logW;
    AboutWindow *aboutW;
    QString *logText;
    ABMDatabase *db;
    QShortcut *keyPress;
    QProcess *server;
    QString *basePath;
    QApplication *app;
    bool serverStarting;
    void clearPlayerList();
    void addPlayer(QString name, QString blid, QString ping);
    void cleanFiles();
    void buildGamemodeList();
    void basePathBuild();
    bool saveSettings();
    bool loadSettings();
    void updateSAList(QString val);
    void updateAList(QString val);
    void parseColorsetLine(QString line, int &row, int &col);

private slots:
    void openLog();
    void openAbout();
    void openBLExec();
    void openSavefile();

    void startServer();
    void stopServer();
    void killServer();
    void serverStopped();
    void processError(QProcess::ProcessError err);
    void updateOutput();
    void sendCommand();

    void saveAddonList();
    void allAddons();
    void noAddons();
    void defaultAddons();
    void loadAddonList();

    void saveMusicList();
    void allMusic();
    void noMusic();
    void defaultMusic();
    void loadMusicList();

    void loadColorset();
    void saveColorset();

    void loadBanlist();
    void saveBanlist();

    void changeOutput(bool);

    void scanForFlagged();

    void setTheme();

    void loadPrefList();
    void savePrefList();

    void applyZebloteFix(bool enabled);
};

#endif // MAINWINDOW_H
