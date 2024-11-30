#ifndef DATAMATRIX_H
#define DATAMATRIX_H

#include <QObject>
#include <QColor>
#include <QRandomGenerator64>

#include "matrix.h"

enum Colors {
    Transparent  = 0,
    Red,
    Cyan,
    Blue,
    Green,
    ColorsCount
};

struct Cell
{
    Cell(QColor col = QColor(Qt::transparent)) : color(col) {}
    Cell( bool empty, QColor col = QColor(Qt::transparent)) :isEmpty(empty), color(col) {}

    bool isEmpty = true;
    QColor color;
};

typedef QList<QPoint> Points;

class DataMatrix : public QObject
{
    Q_OBJECT
public:
    explicit DataMatrix(QObject *parent = nullptr);

    inline int rows()    const { return m_rows; } 
    inline int columns() const { return m_columns; } 

    QColor getColor(int,int)  const;
    bool getFillness(int,int) const;

    void computerTern();

    void move(QPoint from, QPoint to);

    void append(int, int, QColor);
    inline void append(QPoint p, QColor col) {
        append(p.x(), p.y(), col);
    }
    void remove(int, int);
    inline void remove(QPoint p) {
        remove(p.x(), p.y());
    }
    void clear();

signals:
    void cellChanged(int,int);
    void gameOver();
    void updateScore();

private:
    void init();

    const int m_rows = 9;
    const int m_columns = 9;
    const int computerStepSize = 3;
    const int lineDoneTreshhold = 5;

    Matrix<Cell> *matrix; 
    Points m_emptyCells;  

    QColor toQColor(Colors) const;

private slots:
    void checkLines(int, int);
    void removeLine(QPoint, QPoint);
};

#endif // DATAMATRIX_H
