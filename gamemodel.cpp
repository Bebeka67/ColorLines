#include "gamemodel.h"

GameModel::GameModel() {}

int GameModel::rowCount(const QModelIndex &parent) const
{
    return 9;
}

int GameModel::columnCount(const QModelIndex &parent) const
{
    return 9;
}

QVariant GameModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}
