#ifndef AEXPTABELMODEL_H
#define AEXPTABELMODEL_H


#include <QObject>
#include <QAbstractTableModel>
#include <QComboBox>
#include <QStandardItemModel>

class AExpTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns{
        Name = 0,
        Molarmass,
        Density,
        Purity,
        Equiv,
        Mmoles,
        Concentration,
        Mass,
        Volume,
        Structure,
        Position,
        Count = Position + 1
    };

    struct Entry{
        int role;
        QString name;
        QPair<double, bool> molarMass;
        QPair<double, bool> density;
        double purity;
        QPair<double, bool> equiv;
        QPair<double, bool> mmoles;
        QPair<double, bool> concentration;
        QPair<double, bool> mass;
        QPair<double, bool> volume;
        int structure;
        int position;
    };

    AExpTableModel(int *significantNumber, QObject *parent = nullptr);
    ~AExpTableModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);
    void addEntry(int role, QString name, double molarMass, double density, double purity, double equiv, double mmoles, double concentration, double masse, double volume, int structure, int position, int count = -1);
    QJsonArray toJsonArray() const;
    void fromJSonArray(QJsonArray const& array);
    QVariant getData(const int row, AExpTableModel::Columns col);
    QString getNamePlain(const int row) const;
    void loopVHeaderData(const int section);
    bool removeRowAt(int row);
    QHash<QString, QVariant> getHashDatabase(int row);
    QStandardItemModel* getCompleterModel();
    double getXlsxScale();
    double getXlsxYield();
    bool canUpdateHere(const QModelIndex &index);

    void insertRowS(int const& row, const int &role, const int &structure, const int &position);
    void moveRowUp(int const& row);
    void moveRowDown(int const& row);
    bool update(const QModelIndex &indexC);
    void receiveQuery(const int &row, const QVariantList &data);
    void updateCompleterList();

signals:
    void completerModelChangedExp();

protected:
    bool updateSMRow(const QModelIndex &indexC, double &return_nSM);
    void updateRRows(QVector<int> indexes, double nSM);
    void updateSRows(QVector<int> indexes, double nSM);
    void updatePRows(QVector<int> indexes, double nSM);
    void turnTrue();
    QString makeCompleterStr(const int row);

private:
    QList<Entry> m_entries;
    int *m_significantNumber;
    QStandardItemModel *m_completerModel;
};

#endif // AEXPTABELMODEL_H
