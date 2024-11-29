#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>

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
        ColorRole,
        EmptyRole
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

#endif // GAMEMODEL_H
