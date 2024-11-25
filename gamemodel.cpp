#include "gamemodel.h"
#include <QDebug>

GameModel::GameModel(QObject *parent)
    : QAbstractTableModel(parent),
    m_score(0)
{
    matrix = new DataMatrix(this);

    connect(matrix, &DataMatrix::gameOver, this, &GameModel::gameOver);
    connect(matrix, &DataMatrix::updateScore,
    [&]() {
        m_score +=10;
        emit scoreChanged();
    });

    connect(matrix, &DataMatrix::cellChanged, [&](int r, int c){
        QModelIndex index = this->createIndex(r,c);
        emit dataChanged(index, index, {ColorRole});
    });
}

GameModel::~GameModel()
{
}

int GameModel::score() const
{
    return m_score;
}

int GameModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return matrix->rows();
}

int GameModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return matrix->columns();
}

QVariant GameModel::data(const QModelIndex &index, int role) const
{
    if(role == ColorRole) {
        return QVariant(matrix->getColor(index.row(), index.column()));
    }

    return QVariant();
}

void GameModel::clearField()
{
    matrix->clear();
    matrix->computerTern();
    emit layoutChanged();
    return;
}

void GameModel::computer()
{
    matrix->computerTern();
}

QHash<int, QByteArray> GameModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DisplayRole] = "display";
    roles[ColorRole]   = "ballColor";

    return roles;
}

void GameModel::move(QPoint from, QPoint to)
{
    matrix->move(from, to);
    matrix->computerTern();
}

DataMatrix::DataMatrix(QObject *parent) : QObject{parent}
{
    init();
    computerTern();
}

void DataMatrix::move(QPoint from, QPoint to)
{
    auto color = m_matrix[from.x()][from.y()];
    remove(from.x(), from.y());
    append(to.x(), to.y(), color);
    checkLines(to.x(), to.y());
}

void DataMatrix::append(int r, int c, int color)
{
    m_matrix[r][c] = (Colors)color;
    emit cellChanged(r,c);
    m_emptyCells.removeOne({r,c});

    // checkLines(r,c);

    if(m_emptyCells.isEmpty()) {
        emit gameOver();
    }
}

void DataMatrix::remove(int r, int c)
{
    m_matrix[r][c] = Transparent;
    emit cellChanged(r,c);
    m_emptyCells.append({r,c});
}

DataMatrix::Colors DataMatrix::color(int r, int c) const
{
    return m_matrix[r][c];
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

    int treshhold = 5;

    int curRow = r;
    int curCol = c;
    auto baseColor = color(r,c);
    int ballsCounter = 0;

    // check right direction
    int upperBound = curCol;
    int lowerBound = curCol;

    auto successCheck = [&](bool isRow) -> bool {
        if(ballsCounter == treshhold) {
            isRow ? removeLine( {r, lowerBound} , {r, upperBound}) :
                    removeLine( {lowerBound, c} , {upperBound, c});
            emit updateScore();
            return true;
        }
        return false;
    };

    while( curCol != m_columns && ballsCounter != treshhold) {
        if( baseColor == color(curRow, curCol) ) {
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
    while( curCol != -1 && ballsCounter != treshhold) {
        if( baseColor == color(curRow, curCol) ) {
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
    while( curRow != m_rows && ballsCounter != treshhold) {
        if( baseColor == color(curRow, curCol)) {
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
    while( curRow != -1 && ballsCounter != treshhold) {
        if( baseColor == color(curRow, curCol) ) {
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
void DataMatrix::init()
{
    m_matrix.clear();
    m_emptyCells.clear();
    for(int  i = 0; i < m_rows; i++) {
        m_matrix.append( QVector<Colors>().fill(Transparent, m_columns));
        for(int j = 0; j < m_columns; j++) {
            m_emptyCells.append({i,j});
        }
    }
}

QColor DataMatrix::getColor(int r, int c) const
{
    switch ( color(r,c) ) {
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

void DataMatrix::computerTern()
{
    for(int i = 0; i < 3; i++) {
        if(m_emptyCells.isEmpty())
            break;
        auto randomCell = QRandomGenerator::global()->bounded(0, m_emptyCells.size());
                                                              // to skip transparent
        auto randomColor = QRandomGenerator::global()->bounded(1, Colors::ColorsCount);
        auto point = m_emptyCells.at(randomCell);
        this->append(point.first, point.second, (Colors)randomColor);
    }
}
