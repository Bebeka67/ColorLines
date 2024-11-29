#include "gamemodel.h"
#include <QDebug>

#include <datamatrix.h>

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
        emit dataChanged(index, index, {ColorRole, EmptyRole});
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
    if(role == EmptyRole) {
        return QVariant(matrix->getFillness(index.row(), index.column()));
    }

    return QVariant();
}

void GameModel::clearField()
{
    matrix->clear();
    matrix->computerTern();
    m_score = 0;
    emit scoreChanged();
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
    roles[EmptyRole]   = "isEmpty";

    return roles;
}

void GameModel::move(QPoint from, QPoint to)
{
    matrix->move(from, to);
    matrix->computerTern();
}
