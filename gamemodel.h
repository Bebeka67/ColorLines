#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>

class GameModel : public QAbstractTableModel
{
public:
    GameModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override final;
};

#endif // GAMEMODEL_H
