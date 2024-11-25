#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QSet>
#include <QRandomGenerator64>
#include <QDebug>


class DataMatrix;
class GameModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int score READ score NOTIFY scoreChanged FINAL)

public:
    explicit GameModel(QObject *parent = nullptr);
    ~GameModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override final;

    enum Roles {
        DisplayRole = Qt::DisplayRole,
        ColorRole
    };

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void move(QPoint from, QPoint to);

    Q_INVOKABLE void clearField();
    Q_INVOKABLE void computer();

    int score() const;
signals:
    void gameOver();
    void scoreChanged();

private:
    DataMatrix *matrix;
    int m_score;
};

class DataMatrix : public QObject
{
    Q_OBJECT
public:
    explicit DataMatrix(QObject *parent = nullptr);

    int rows()    const { return m_rows; }
    int columns() const { return m_columns; }

    QColor getColor(int,int) const;

    void computerTern();

    void move(QPoint from, QPoint to);

    void append(int, int, int);
    void remove(int, int);
    void clear();

signals:
    void cellChanged(int,int);
    void gameOver();
    void updateScore();

private:
    void init();
    enum Colors {
        Transparent  = 0,
        Red,
        Cyan,
        Blue,
        Green,
        ColorsCount
    };
    QVector<QVector<Colors>> m_matrix;
    const int m_rows    = 9;
    const int m_columns = 9;
    QList<QPair<int,int>> m_emptyCells;
    Colors color(int,int) const;
private slots:
    void checkLines(int, int);
    void removeLine(QPoint, QPoint);
};


#endif // GAMEMODEL_H
