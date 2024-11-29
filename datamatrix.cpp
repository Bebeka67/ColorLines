#include "datamatrix.h"

DataMatrix::DataMatrix(QObject *parent) : QObject{parent}
{
    matrix = new Matrix<Cell>(m_rows, m_columns);

    init();

    computerTern();
}

void DataMatrix::init()
{
    matrix->fill(Cell()); // fill matrix with default value
    m_emptyCells.clear(); // clear old matrix

    for(int  i = 0; i < matrix->rows(); i++)
        for(int j = 0; j < matrix->columns(); j++)
            m_emptyCells.append({i,j}); // refill coordinates of empty cells
}

void DataMatrix::computerTern()
{
    for(int i = 0; i < computerStepSize; i++) {
        if(m_emptyCells.isEmpty())
            break;
        auto randomCell = QRandomGenerator::global()->bounded(0, m_emptyCells.size());
            												// 1 to skip transparent
        auto randomColor = QRandomGenerator::global()->bounded(1, Colors::ColorsCount);
        auto point = m_emptyCells.at(randomCell);
        this->append(point, toQColor((Colors)randomColor));
    }
}

void DataMatrix::append(int r, int c, QColor color)
{
    matrix->at(r, c) = Cell(false, color);

    emit cellChanged(r, c);
    m_emptyCells.removeOne({r, c});

    if(m_emptyCells.isEmpty()) {
        emit gameOver();
    }
}

QColor DataMatrix::toQColor(Colors color) const
{
    switch ( color ) {
    case Transparent:
        return QColor("transparent");
        break;
    case Red:
        return QColor("red");
        break;
    case Blue:
        return QColor("blue");
        break;
    case Green:
        return QColor("green");
        break;
    case Cyan:
        return QColor("cyan");
        break;
    default:
        return QColor("transparent");
        break;
    }
}

void DataMatrix::move(QPoint from, QPoint to)
{
    QColor color = matrix->at(from.x(), from.y()).color;
    remove(from);
    append(to, color);
    checkLines(to.x(), to.y());

}

void DataMatrix::remove(int r, int c)
{
    matrix->at(r,c).isEmpty = true;

    emit cellChanged(r,c);
    m_emptyCells.append({r,c});
}

void DataMatrix::checkLines(int r, int c)
{
    /* реализация на скорую руку) МНОГО ПОВТОРОВ
     * Идея следующая.
     * 1) Берется опорное положение только что передвинутого шара.
     * 2) Вычисляется опорный цвет
     * 3) Обход начинается с опорного положения в правом горизонтальном направлении,
     *    т.е. в сторону увеличения индекса столбцов
     * 4) Вычисляется текущий цвет и сравнивается с опорным; проверям, достиг ли счетчик 5
     * 5) Если совпадает, то увеличиваем счетчик шаров
     * 6) Иначе идем в сторону уменьшения индекса столбцов, при этом, счетчик шаров сохраняется.
     * 7) Если счетчик шаров достиг 5, то удаляем шары и начисляем очки. Выход
     * 8) Иначе меняем направление на нижне вертикальное,
     *    т.е. в сторону увеличения индекса столбцов
     * 9) 4 - 7
     * 10) Иначе линия из 5 отсутствует
     *
     * при смене направлений на 180 градусов запоминаются крайние положения найденных шаров
     *
    */

    int curRow = r;
    int curCol = c;
    QColor baseColor = matrix->at(r,c).color;
    int ballsCounter = 0;

    // check right direction
    int upperBound = curCol;
    int lowerBound = curCol;

    auto successCheck = [&](bool isRow) -> bool {
        if(ballsCounter == lineDoneTreshhold) {
            isRow ? removeLine( {r, lowerBound} , {r, upperBound}) :
                removeLine( {lowerBound, c} , {upperBound, c});
            emit updateScore();
            return true;
        }
        return false;
    };

    auto checkBalls = [&](int r, int c) -> bool {
        return (baseColor == getColor(curRow, curCol)) && !matrix->at(r, c).isEmpty;
    };


    while( curCol != m_columns && ballsCounter != lineDoneTreshhold) {
        if( checkBalls(curRow, curCol) ) {
            ballsCounter++;
            upperBound = curCol;
            curCol++;
        } else {
            break;
        }
    }

    if(successCheck(true))
        return;

    // swap to opposite direction
    curCol = c - 1;
    while( curCol != -1 && ballsCounter != lineDoneTreshhold) {
        if( checkBalls(curRow, curCol) ) {
            ballsCounter++;
            lowerBound = curCol;
            curCol--;
        } else {
            break;
        }
    }

    if(successCheck(true))
        return;

    // swap to another axis
    // reset conditions
    curCol = c;
    ballsCounter = 0;

    upperBound = curRow;
    lowerBound = curRow;
    // check bottom direction
    while( curRow != m_rows && ballsCounter != lineDoneTreshhold) {
        if( checkBalls(curRow, curCol)) {
            ballsCounter++;
            upperBound = curRow;
            curRow++;
        } else {
            break;
        }
    }

    if(successCheck(false))
        return;

    // swap to opposite direction
    curRow = r - 1;
    while( curRow != -1 && ballsCounter != lineDoneTreshhold) {
        if( checkBalls(curRow, curCol) ) {
            ballsCounter++;
            lowerBound = curRow;
            curRow--;
        } else {
            break;
        }
    }

    if(successCheck(false))
        return;
}

void DataMatrix::removeLine(QPoint from, QPoint to)
{
    /* одна из осей всегда совпадает,
     * соответственно нужен просто перебор по оставшейся,
     * причем начальная точка всегда ближе к началу координат
    */
    if(from.x() == to.x()) {
        for(int i = from.y(); i <= to.y(); i++) {
            remove(from.x(), i);
        }
    } else {
        for(int i = from.x(); i <= to.x(); i++) {
            remove(i, from.y());
        }
    }
}

void DataMatrix::clear()
{
    init();
}

QColor DataMatrix::getColor(int r, int c) const
{
    return matrix->at(r,c).color;
}
bool DataMatrix::getFillness(int r, int c) const
{
    return matrix->at(r,c).isEmpty;
}

