#ifndef ACONDITIONSTABLEMODEL_H
#define ACONDITIONSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QPair>

class QJsonArray;
class QStandardItemModel;

class AConditionsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns { Temperature = 0, Duration, Count = Duration + 1 };

    struct Condition{
        double temperature;
        QString duration;
    };

    explicit AConditionsTableModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);
    QVariant getData(const int row, AConditionsTableModel::Columns col);
    void insertRowS(const int row);
    bool removeRowAt(const int row);
    void addElement(double temperature, QString duration, int count = -1);
    QJsonArray toJsonArray() const;
    void fromJSonArray(QJsonArray const& array);
    QStandardItemModel* getCompleterModel();

public slots:
    void updateCompleterModel();

signals:
    void completerModelChangedCond();

protected:
    QString makeCompleterStr(const int row);

private:
    QList<Condition> m_conditions;
    QStandardItemModel *m_completerModel;

};

#endif // ACONDITIONSTABLEMODEL_H
