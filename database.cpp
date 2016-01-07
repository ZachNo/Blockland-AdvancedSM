#include "database.h"

ABMDatabase::ABMDatabase(QString *lText)
{
    logText = lText;
}

bool ABMDatabase::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ABM.db");
    if (!db.open()) {
        QMessageBox::critical(0, "Cannot open database",
            "Unable to establish a database connection.\n\n"
                     "Click Cancel to exit.", QMessageBox::Cancel);
        logText->append("\nUnable to establish connection to database!");
        return false;
    }
    logText->append("\nConnection to database successful!");

    if(!query("CREATE TABLE IF NOT EXISTS IP ("
                   "BLID INTEGER UNIQUE,"
                   "IP1 TEXT,"
                   "PRIMARY KEY(BLID))")
    )
    return false;

    return true;
}

bool ABMDatabase::query(QString query)
{
    QSqlQuery q;
    if(!q.exec(query))
    {
        logText->append("\n");
        logText->append(q.lastError().text());
        return false;
    }

    logText->append("\nQuery Result:");
    while(q.next())
    {
        logText->append("\n | ");
        logText->append(q.value(0).toString());
    }

    return true;
}
