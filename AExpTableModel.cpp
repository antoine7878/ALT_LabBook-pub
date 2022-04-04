#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QAbstractItemModel>
#include <QDebug>
#include <cmath>

#include "AExpTableModel.h"
#include "ALT.h"

AExpTableModel::AExpTableModel(int *significantNumber, QObject *parent) :
    QAbstractTableModel(parent),
    m_significantNumber(significantNumber),
    m_completerModel(new QStandardItemModel(this))
{

}
AExpTableModel::~AExpTableModel()
{

}
//METHODES
Qt::ItemFlags AExpTableModel::flags(const QModelIndex &index) const
{
    int i = index.column();
    if (i == Name)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Molarmass)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Density)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Purity)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Equiv)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Mmoles)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Concentration)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Mass)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Volume)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Structure)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if (i == Position)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    return QAbstractTableModel::flags(index);
}
int AExpTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_entries.count();
}
int AExpTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : Count;
}
QVariant AExpTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.count())
        return QVariant();
    int i = index.column();
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (i == Name)
            return m_entries[index.row()].name;
        else if (i == Molarmass)
            return m_entries[index.row()].molarMass.first;
        else if (i == Density)
            return m_entries[index.row()].density.first;
        else if (i == Purity)
            return m_entries[index.row()].purity;
        else if (i == Equiv)
            return m_entries[index.row()].equiv.first;
        else if (i == Mmoles)
            return m_entries[index.row()].mmoles.first;
        else if (i == Concentration)
            return m_entries[index.row()].concentration.first;
        else if (i == Mass)
            return m_entries[index.row()].mass.first;
        else if (i == Volume)
            return m_entries[index.row()].volume.first;
        else if (i == Structure)
            return m_entries[index.row()].structure;
        else if (i == Position)
            return m_entries[index.row()].position;
        break;

    case Qt::UserRole:
        if (i == Molarmass)
            return m_entries[index.row()].molarMass.second;
        else if (i == Density)
            return m_entries[index.row()].density.second;
        else if (i == Equiv)
            return m_entries[index.row()].equiv.second;
        else if (i == Mmoles)
            return m_entries[index.row()].mmoles.second;
        else if (i == Concentration)
            return m_entries[index.row()].concentration.second;
        else if (i == Mass)
            return m_entries[index.row()].mass.second;
        else if (i == Volume)
            return m_entries[index.row()].volume.second;
        else
            return true;
        break;
    }
    return QVariant();
}
QVariant AExpTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case Name:
            return "Name";
            break;
        case Molarmass:
            return "M [g/mol]";
        case Density:
            return "d";
        case Purity:
            return "%";
        case Equiv:
            return "equiv";
        case Mmoles:
            return "n [mmol]";
        case Concentration:
            return "C [mol/L]";
        case Mass:
            return "m [mg]";
        case Volume:
            return "V [mL]";
        case Structure:
            return "Structure";
        case Position:
            return "Position";
            break;
        }
    }
    else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
        switch(m_entries[section].role)
        {
        case 0:
            return "SM";
            break;
        case 1:
            return "R";
            break;
        case 2:
            return "S";
            break;
        case 3:
            return "P";
            break;
        case 4:
            return u8"\u2205";
        }
    }
    else if (orientation == Qt::Vertical && role == Qt::UserRole)
    {
        return m_entries[section].role;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
bool AExpTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.count())
        return false;
    int i = index.column();
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (i == Name)
            m_entries[index.row()].name = value.toString();
        else if (i == Molarmass)
            m_entries[index.row()].molarMass.first = value.toDouble();
        else if (i == Density)
            m_entries[index.row()].density.first = value.toDouble();
        else if (i == Purity)
            m_entries[index.row()].purity = value.toDouble();
        else if (i == Equiv)
            m_entries[index.row()].equiv.first = value.toDouble();
        else if (i == Mmoles)
            m_entries[index.row()].mmoles.first = value.toDouble();
        else if (i == Concentration)
            m_entries[index.row()].concentration.first = value.toDouble();
        else if (i == Mass)
            m_entries[index.row()].mass.first = value.toDouble();
        else if (i == Volume)
            m_entries[index.row()].volume.first = value.toDouble();
        else if (i == Structure)
            m_entries[index.row()].structure = value.toInt();
        else if (i == Position)
            m_entries[index.row()].position = value.toInt();
        updateCompleterList();
        emit dataChanged(index, index);
        return true;
        break;
        break;
    }
    return false;
}
void AExpTableModel::addEntry(int role, QString name, double molarMass, double density, double purity, double equiv, double mmoles, double concentration, double mass, double volume, int structure, int position, int count)
{
    if(count == -1)
        count = m_entries.size();
    if(role < 0 || role > 4)
        role = 0;
    if (molarMass < 0)
        molarMass = 0;
    if (density < 0)
        density = 0;
    if(purity < 0)
        purity = 0;
    else if(purity > 100)
        purity = 100;
    if(equiv < 0)
        equiv  = 0;
    if(mmoles < 0)
        mmoles = 0;
    if(concentration < 0)
        concentration = 0;
    if(mass < 0)
        mass = 0;
    if(volume < 0)
        volume = 0;
    if(position < 0)
        position = 0;


    beginInsertRows(QModelIndex(), count, count);
    m_entries.insert(count, Entry{role,
                                  name,
                                  QPair<double,bool>(molarMass,true),
                                  QPair<double,bool>(density,true),
                                  purity,
                                  QPair<double,bool>(equiv,true),
                                  QPair<double,bool>(mmoles,true),
                                  QPair<double,bool>(concentration,true),
                                  QPair<double,bool>(mass,true),
                                  QPair<double,bool>(volume,true),
                                  structure,
                                  position});
    endInsertRows();
    updateCompleterList();
}
QJsonArray AExpTableModel::toJsonArray() const
{
    QJsonArray array;
    for(auto it = m_entries.cbegin(); it != m_entries.cend(); it++){
        QJsonObject line;

        line["role"] = it->role;
        line["name"] = it->name;
        line["molarMass"] = it->molarMass.first;
        line["density"] = it->density.first;
        line["purity"] = it->purity;
        line["equiv"] = it->equiv.first;
        line["mmoles"] = it->mmoles.first;
        line["concentration"] = it->concentration.first;
        line["mass"] = it->mass.first;
        line["volume"] = it->volume.first;
        line["struct"] = it->structure;

        line["position"] =it->position;

        array.append(line);
    }
    return array;
}
void AExpTableModel::fromJSonArray(QJsonArray const& array)
{
    for(int i = 0; i < array.count(); i++){
        QJsonObject line(array[i].toObject());
        addEntry(line.value("role").toInt(),
                 line.value("name").toString(),
                 line.value("molarMass").toDouble(),
                 line.value("density").toDouble(),
                 line.value("purity").toDouble(),
                 line.value("equiv").toDouble(),
                 line.value("mmoles").toDouble(),
                 line.value("concentration").toDouble(),
                 line.value("mass").toDouble(),
                 line.value("volume").toDouble(),
                 line.value("struct").toInt(),
                 line.value("position").toInt());
    }
    updateCompleterList();
}
QVariant AExpTableModel::getData(const int row, AExpTableModel::Columns col)
{
    if(col == Name)
        return m_entries[row].name;
    else if(col == Molarmass)
        return m_entries[row].mmoles.first;
    else if(col == Density)
        return m_entries[row].density.first;
    else if(col == Purity)
        return m_entries[row].purity;
    else if(col == Equiv)
        return m_entries[row].equiv.first;
    else if(col == Mmoles)
        return m_entries[row].mmoles.first;
    else if(col == Concentration)
        return m_entries[row].concentration.first;
    else if(col == Mass)
        return m_entries[row].mass.first;
    else if(col == Volume)
        return m_entries[row].volume.first;
    else if(col == Structure)
        return m_entries[row].structure;
    else if(col == Position)
        return m_entries[row].position;
    else
        return QVariant();
}

QString AExpTableModel::getNamePlain(const int row) const
{
    QTextDocument doc;
    doc.setHtml(m_entries[row].name);
    return doc.toPlainText();
}

void AExpTableModel::loopVHeaderData(const int section)
{
    m_entries[section].role < 4 ? m_entries[section].role++ : m_entries[section].role = 0;
    switch(m_entries[section].role){
    case 0 :
        setData(index(section, AExpTableModel::Position), 1);
        break;
    case 1:
        setData(index(section, AExpTableModel::Position), 2);
        break;
    case 2:
        setData(index(section, AExpTableModel::Position), 8);
        break;
    case 3:
        setData(index(section, AExpTableModel::Position), 3);
        break;
    case 4:
        setData(index(section, AExpTableModel::Position), 0);
        break;
    }
    updateCompleterList();
    emit headerDataChanged(Qt::Vertical, section, section);
}
QHash<QString,QVariant> AExpTableModel::getHashDatabase(int row)
{
    QHash<QString, QVariant> hash;
    hash["name"] = m_entries[row].name;
    hash["mm"] = m_entries[row].molarMass.first;
    hash["d"] = m_entries[row].density.first;
    hash["struct"] = m_entries[row].structure;
    return hash;
}
QStandardItemModel* AExpTableModel::getCompleterModel()
{
    return m_completerModel;
}
double AExpTableModel::getXlsxScale()
{
    for(int i = 0; i<rowCount(); i++){
        if(m_entries[i].role == 0)
            return m_entries[i].mmoles.first;
    }
    return 0;
}
double AExpTableModel::getXlsxYield()
{
    for(int i = 0; i<rowCount(); i++){
        if(m_entries[i].role == 3)
            return m_entries[i].purity;
    }
    return 0;
}

bool AExpTableModel::canUpdateHere(const QModelIndex &index)
{
    if(m_entries[index.row()].role == 0){
        if(index.column() == 5 || index.column() == 7 || index.column() == 8)
            return true;
    }
    return false;
}

void AExpTableModel::insertRowS(int const& row, const int &role , int const& structure, const int &position)
{
    addEntry(role, QString(), 0, 0, 0, 0, 0, 0, 0, 0, structure, position, row);
}
bool AExpTableModel::removeRowAt(int row)
{
    if(row < 0 || row>rowCount())
        return false;
    beginRemoveRows(QModelIndex(), row, row);
    m_entries.removeAt(row);
    endRemoveRows();
    updateCompleterList();
    return true;
}
void AExpTableModel::moveRowUp(int const& row)
{
    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row - 1);
    m_entries.swapItemsAt(row, row-1);
    endMoveRows();
    updateCompleterList();
}
void AExpTableModel::moveRowDown(int const& row)
{
    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row + 2);
    m_entries.swapItemsAt(row, row+1);
    endMoveRows();
    updateCompleterList();
}

bool AExpTableModel::update(const QModelIndex &indexC)
{
    turnTrue();
    QVector<int> smRows(0);
    QVector<int> rRows(0);
    QVector<int> sRows(0);
    QVector<int> pRows(0);

    for(int i = 0; i < rowCount(); i++){
        switch (m_entries[i].role) {
        case 0:
            smRows << i;
            break;
        case 1:
            rRows << i;
            break;
        case 2:
            sRows << i;
            break;
        case 3:
            pRows << i;
            break;
        }
    }
    if(smRows.size() > 1)
    {
        QMessageBox::warning(nullptr, "Warning", "Please choose only one starting material");
        return false;
    }
    double nSM = -1;
    if(!updateSMRow(indexC, nSM)){
        emit dataChanged(index(smRows[0],0), index(smRows[0], columnCount()));
        return false;
    }
    updateRRows(rRows, nSM);
    updateSRows(sRows, nSM);
    updatePRows(pRows, nSM);

    updateCompleterList();
    return true;
}

void AExpTableModel::receiveQuery(const int &row, const QVariantList &data)
{
    setData(index(row, Name), data[0]);
    setData(index(row, Molarmass), data[1]);
    setData(index(row, Density), data[2]);
}
bool AExpTableModel::updateSMRow(const QModelIndex &indexC, double &return_nSM)
{
    Entry &currentEntry = m_entries[indexC.row()];
    int column = indexC.column();

    if((column == 5 && currentEntry.mmoles.first == 0) ||
            (column == 7 && currentEntry.mass.first == 0) ||
            (column == 8 && currentEntry.volume.first == 0)){
        QMessageBox::warning(0,"warning", "The selected cell is empty");
        return false;
    }

    if(currentEntry.molarMass.first != 0){
        double mm = currentEntry.molarMass.first;
        double d = currentEntry.density.first;
        double p = currentEntry.purity;
        (p == 0) ? (p = 1) : (p/=100);

        if(column == 5){//n is filled
            return_nSM = currentEntry.mmoles.first;
            double m = return_nSM * mm / p;
            currentEntry.mass.first = m;
            if(d != 0){
                currentEntry.volume.first = m / d /1000;
            }
        }
        else if(column == 7){//mass is filled
            return_nSM = currentEntry.mass.first * p / mm;
            currentEntry.mmoles.first = return_nSM;
            if(d != 0){
                currentEntry.volume.first = currentEntry.mass.first * d / 1000;
            }
        }
        else if(column == 8){// volume is filled
            currentEntry.mass.first = currentEntry.volume.first * d;
            return_nSM = currentEntry.mass.first * p / mm;
            currentEntry.mmoles.first = return_nSM;
        }
        return true;
    }
    if(currentEntry.molarMass.first == 0)
        currentEntry.molarMass.second = false;
    return false;
}
void AExpTableModel::updateRRows(QVector<int> indexes, double nSM)
{
    for(auto i : indexes ){
        Entry &currentEntry = m_entries[i];
        if(currentEntry.equiv.first == 0){
            currentEntry.equiv.second = false;
        }
        else if(currentEntry.molarMass.first != 0){
            double equiv = currentEntry.equiv.first;
            double mm = currentEntry.molarMass.first;
            double d = currentEntry.density.first;
            double p = currentEntry.purity;
            (p == 0) ? (p = 1) : (p/=100);
            currentEntry.mmoles.first = p*equiv*nSM;
            currentEntry.mass.first = currentEntry.mmoles.first*mm;
            if(d != 0)
                currentEntry.volume.first = currentEntry.mass.first/d/1000;
        }

        if(currentEntry.molarMass.first == 0){
            currentEntry.molarMass.second = false;
        }
    }
}
void AExpTableModel::updateSRows(QVector<int> indexes, double nSM)
{
    for(auto i : indexes){
        Entry &currentEntry = m_entries[i];

        if(currentEntry.concentration.first == 0 && currentEntry.volume.first == 0){
            currentEntry.concentration.second = false;
            currentEntry.volume.second = false;

        }
        else{
            if(currentEntry.concentration.first != 0){
                currentEntry.volume.first = nSM / currentEntry.concentration.first;
            }
            else if (currentEntry.volume.first != 0){
                currentEntry.concentration.first = nSM/currentEntry.volume.first;
            }
        }
    }
}
void AExpTableModel::updatePRows(QVector<int> indexes, double nSM)
{
    for(auto i : indexes){
        Entry &currentEntry = m_entries[i];

        if(currentEntry.mass.first == 0 && currentEntry.volume.first == 0){
            currentEntry.mass.second = false;
            currentEntry.volume.second = false;
            if(currentEntry.density.first == 0)
                currentEntry.density.second = false;
        }
        else if(currentEntry.mass.first != 0 && currentEntry.volume.first != 0){
            currentEntry.mass.second = false;
            currentEntry.volume.second = false;
        }
        else if(currentEntry.volume.first != 0 && currentEntry.density.first == 0){
            currentEntry.density.second = false;
        }
        else if(currentEntry.molarMass.first != 0){
            if(currentEntry.mass.first != 0){
                currentEntry.mmoles.first = currentEntry.mass.first / currentEntry.molarMass.first;
                if(currentEntry.density.first != 0){
                    currentEntry.volume.first = currentEntry.mass.first / currentEntry.density.first / 1000;
                }
            }
            else if((currentEntry.volume.first != 0) && (currentEntry.density.first != 0)){
                currentEntry.mass.first = currentEntry.volume.first * currentEntry.density.first;
                currentEntry.mmoles.first = currentEntry.mass.first / currentEntry.molarMass.first;
            }
            currentEntry.purity = currentEntry.mmoles.first / nSM / currentEntry.equiv.first * 100;
        }
        if(currentEntry.molarMass.first == 0)
            currentEntry.molarMass.second = false;
    }
}
void AExpTableModel::updateCompleterList()
{
    m_completerModel->clear();
    for(int row = 0; row<rowCount(); row++)
        m_completerModel->setItem(row, 0, new QStandardItem(makeCompleterStr(row)));
    emit completerModelChangedExp();
}
void AExpTableModel::turnTrue()
{
    for(int i = 0; i < rowCount(); i++){
        m_entries[i].molarMass.second = true;
        m_entries[i].density.second = true;
        m_entries[i].equiv.second = true;
        m_entries[i].mmoles.second = true;
        m_entries[i].concentration.second = true;
        m_entries[i].mass.second = true;
        m_entries[i].volume.second = true;
    }
}
QString AExpTableModel::makeCompleterStr(const int row)
{
    Entry entry = m_entries[row];
    QTextDocument doc;
    doc.setHtml(entry.name);
    QString str = doc.toPlainText() + " ";
    //Entry is Starting material or Reagent
    if(entry.role == 0 || entry.role == 1){
        str += "(";
        if(entry.mass.first != 0)
            str += ALT::displayX(entry.mass.first, *m_significantNumber) + " mg, ";
        if(entry.volume.first != 0){
            str += ALT::displayX(entry.volume.first, *m_significantNumber) + " mL, ";
            if(entry.density.first != 0)
                str += "d = " + ALT::displayX(entry.density.first, *m_significantNumber) +", ";
        }
        str += ALT::displayX(entry.mmoles.first, *m_significantNumber) + " mmol, ";
        str += ALT::displayX(entry.equiv.first, *m_significantNumber) + " equiv)";
    }

    //Entry is Solvent
    if(entry.role == 2){
        str += "(";
        str += ALT::displayX(entry.volume.first, *m_significantNumber) + " mL, ";
        str += ALT::displayX(entry.concentration.first, *m_significantNumber) + " M)";
    }

    //Entry is Product
    if(entry.role == 3){
        str += ALT::displayX(entry.purity, *m_significantNumber) + "% (";
        if(entry.mass.first != 0)
            str += ALT::displayX(entry.mass.first, *m_significantNumber) + " mg, ";
        if(entry.volume.first != 0){
            str += ALT::displayX(entry.volume.first, *m_significantNumber) + " mL, ";
            if(entry.density.first != 0)
                str += "d = " + ALT::displayX(entry.density.first, *m_significantNumber) +", ";
        }
        str += ALT::displayX(entry.mmoles.first, *m_significantNumber) + " mmol)";
    }
    return str;
}
