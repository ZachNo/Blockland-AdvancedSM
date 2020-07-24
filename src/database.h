#ifndef DATABASE_H
#define DATABASE_H

#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QVariant>

class ABMDatabase
{
public:
    ABMDatabase(QString *lText);
    bool initDatabase();
    bool query(QString query);
private:
    QString *logText;
    QSqlDatabase db;
};

#endif // DATABASE_H
