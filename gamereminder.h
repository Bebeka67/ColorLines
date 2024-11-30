#ifndef GAMEREMINDER_H
#define GAMEREMINDER_H

#include <QObject>
#include <QtSql>

class Cell;
class GameReminder : public QObject
{
    Q_OBJECT
public:
    explicit GameReminder(QObject *parent = nullptr);
    ~GameReminder();

    QVector<Cell> load();
    void save();

    enum Status {
        NotEnable,   // all operations banned
        NewDB,       // load operation banned
        Ready
    };

    inline Status satatus() const { return m_status; }

private:

    bool init();
    Status m_status;

    QSqlDatabase db;

    const QString gameDBName= "gamesave.db";
    const QString gameTableName = "dataMatrix";
};

#endif // GAMEREMINDER_H
