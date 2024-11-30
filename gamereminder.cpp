#include "gamereminder.h"
#include "datamatrix.h"

GameReminder::GameReminder(QObject *parent) :
    QObject(parent)
{
    init();
}

GameReminder::~GameReminder()
{
    db.close();
}

bool GameReminder::init()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(gameDBName);

    if(!db.open()) {
        qDebug() << "Open error: " <<db.lastError().text();
        m_status = NotEnable;
        return false;
    }

    QSqlQuery query;

    // check table exists
    QString checkTableQuery = QString("SELECT name "
                                      "FROM sqlite_master "
                                      "WHERE type='table' "
                                      "AND name ='%0' ").arg(gameTableName);
    if(!query.exec(checkTableQuery)) {
        qDebug() << "Can't check table: " <<query.lastError().text();
        m_status = NotEnable;
        return false;
    }

    bool exists = false;
    while(query.next()) {
        if(gameTableName == query.value(0).toString()) {
            exists = true;
            break;
        }
    }

    if(!exists) {
        QString createTableQuery = QString("CREATE TABLE %0 ("
                                           "number integer PRIMARY KEY NOT NULL, "
                                           "color integer, "
                                           "isEmpty integer "
                                           ")"
                                           ).arg(gameTableName);
        if(!query.exec(createTableQuery)) {
            qDebug() << query.lastError().text();
            m_status = NotEnable;
            return false;
        } else {
            // new table just created, no saved data yet
            m_status = NewDB;
            return true;
        }
    }

    // ready to be loaded
    m_status = Ready;
    return true;
}

void GameReminder::save()
{

    QString insertQuery = QString("INSERT INTO %0(number, color, isEmpty) "
                                  "VALUES (%1, %2, %3);" );

    QSqlQuery query;

    for(int i = 0; i < 10; i ++)
        qDebug() << query.exec( insertQuery.arg(gameTableName).arg(i).arg(i+1).arg(i+2) );
}
