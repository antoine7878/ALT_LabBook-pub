#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QDebug>

#include "AConditionsTableModel.h"

AConditionsTableModel::AConditionsTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_completerModel(new QStandardItemModel)
{
}

Qt::ItemFlags AConditionsTableModel::flags(const QModelIndex &index) const
{
    if (index.column() == Temperature)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    else if (index.column() == Duration)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    return QAbstractTableModel::flags(index);
}
int AConditionsTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_conditions.count();
}
int AConditionsTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : Count;
}
QVariant AConditionsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_conditions.count())
    {
        return QVariant();
    }

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (index.column() == Temperature)
        {
            return m_conditions[index.row()].temperature;
        }
        else if (index.column() == Duration)
        {
            return m_conditions[index.row()].duration;
        }
        break;
    }

    return QVariant();
}
QVariant AConditionsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case Temperature:
            return "Temperature (°C)";
            break;
        case Duration:
            return "Duration (h)";
            break;
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}
bool AConditionsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{    if (!index.isValid() || index.row() < 0 || index.row() >= m_conditions.count())
    {
        return false;
    }

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (index.column() == Temperature)
        {
            m_conditions[index.row()].temperature = value.toDouble();
        }
        else if (index.column() == Duration)
        {
            m_conditions[index.row()].duration = value.toString();
        }
        emit dataChanged(index, index);
        updateCompleterModel();
        return true;
        break;
    }

     return false;

}
QVariant AConditionsTableModel::getData(const int row, AConditionsTableModel::Columns col)
{
    if (col == Temperature)
    {
        return m_conditions[row].temperature;
    }
    else if (col == Duration)
    {
        return m_conditions[row].duration;
    }
    return QVariant();
}
void AConditionsTableModel::addElement(double temperature, QString duration, int count)
{
    if(count == -1)
        count = m_conditions.count();

    beginInsertRows(QModelIndex(), count, count);
    m_conditions.insert(count,Condition{temperature, duration});
    endInsertRows();
    updateCompleterModel();
}
void AConditionsTableModel::insertRowS(int const row)
{
    addElement(0, 0, row);
    updateCompleterModel();
}
bool AConditionsTableModel::removeRowAt(const int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_conditions.removeAt(row);
    endRemoveRows();
    updateCompleterModel();
    return true;
}
QJsonArray AConditionsTableModel::toJsonArray() const
{
    QJsonArray array;
    for(auto it = m_conditions.begin(); it != m_conditions.end(); it++){
        QJsonObject line;
        line["temperature"] = it->temperature;
        line["duration"] = it->duration;
        array << line;
    }

    return array;
}
void AConditionsTableModel::fromJSonArray(QJsonArray const& array)
{
    for(int i = 0; i < array.count(); i++){
        QJsonObject line(array[i].toObject());
        addElement(line["temperature"].toDouble(), line["duration"].toString());
    }
    updateCompleterModel();
}

QStandardItemModel* AConditionsTableModel::getCompleterModel()
{
    return m_completerModel;
}

void AConditionsTableModel::updateCompleterModel()
{
    m_completerModel->clear();
    for(int row = 0; row<rowCount(); row++)
        m_completerModel->setItem(row, 0, new QStandardItem(makeCompleterStr(row)));
    emit completerModelChangedCond();
}
QString AConditionsTableModel::makeCompleterStr(const int row)
{
    const Condition &condition = m_conditions[row];
    QString str = "At ";
        str += QString::number(condition.temperature) + " °C";
//    if(condition.duration > 0)
//        str += " for " + QString::number(condition.duration) + " h";
    return str;
}
