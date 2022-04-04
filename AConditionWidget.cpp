#include <QJsonArray>
#include <QJsonObject>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QWheelEvent>

#include <float.h>
#include <math.h>

#include "AResearchDialog.h"
#include "AConditionWidget.h"

AConditionWidget::AConditionWidget(QWidget *parent) :
    QWidget(parent)
{
    m_completerModel = new QStandardItemModel(this);
    m_layout = new QVBoxLayout(this);
    m_expLayout = new QVBoxLayout();
    m_addLineButton = new QPushButton(QIcon(":/General/Icons/plus.png"), QString(), this);
    connect(m_addLineButton, SIGNAL(clicked()), this, SLOT(addLine()));
    m_layout->addLayout(m_expLayout);
    m_layout->addWidget(m_addLineButton);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}
void AConditionWidget::addLine(const double &temperature, const int &hour, const int &min)
{
    m_conditions << new ConditionLine(m_conditions.size(), temperature, hour, min, this);
    m_expLayout->addWidget(m_conditions.last());
    updateCompleterModel();
}
void AConditionWidget::removeLine()
{
    int index = sender()->property("line").toInt();
    m_conditions.takeAt(index)->deleteLater();
    for(int i = index; i<m_conditions.size(); i++){
        m_conditions[i]->setRmButtonLine(i);
    }
    updateCompleterModel();
}
QJsonArray AConditionWidget::toJsonArray() const
{
    QJsonArray array;
    for(const auto &i : m_conditions){
        array << i->getJsonLine();
    }
    return array;
}
void AConditionWidget::fromJsonArray(const QJsonArray &array)
{
    if(array.size() == 0){
        addLine();
    }
    else{
        for(const auto &i : array){
            QJsonObject line(i.toObject());
            addLine(line["temperature"].toDouble(), line["hour"].toInt(), line["min"].toInt());
        }
    }
}
QStandardItemModel *AConditionWidget::getCompleterModel()
{
    return m_completerModel;
}
void AConditionWidget::updateCompleterModel()
{
    m_completerModel->clear();
    for(auto &condition : m_conditions){
        m_completerModel->appendRow(new QStandardItem(condition->getCompleterLine()));
    }
    emit completerModelChangedCond();
}

QStringList AConditionWidget::getSchemeLines() const
{
    QStringList ret;
    for(const auto &i : m_conditions){
        ret << i->getSchemeLine();
    }
    return ret;
}


//**********************************
// class ConditionLine
//**********************************



ConditionLine::ConditionLine(const int &line, const double &temperature, const int &hour, const int &min, AConditionWidget *parent) :
    QWidget(parent)
{

    m_tempEditor = new ASpinBoxNoWheel(this);
    m_tempEditor->setRange(-273, INT_MAX);
    m_tempEditor->setValue(temperature);
    m_tempEditor->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(m_tempEditor, SIGNAL(valueChanged(int)), parent, SLOT(updateCompleterModel()));

    m_hourEditor = new ASpinBoxNoWheel(this);
    m_hourEditor->setRange(0, INT_MAX);
    m_hourEditor->setValue(hour);
    m_hourEditor->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(m_hourEditor, SIGNAL(valueChanged(int)), parent, SLOT(updateCompleterModel()));

    m_minEditor = new ASpinBoxNoWheel(this);
    m_minEditor->setRange(0, INT_MAX);
    m_minEditor->setValue(min);
    m_minEditor->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(m_minEditor, SIGNAL(valueChanged(int)), parent, SLOT(updateCompleterModel()));

    m_tempLabel = new QLabel("°C", this);
    m_tempLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_hourLabel = new QLabel("h", this);
    m_hourLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_minLabel = new QLabel("min", this);
    m_minLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    m_rmButton = new QPushButton(QIcon(":/General/Icons/minus.png"),QString(), this);
    m_rmButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_rmButton->setProperty("line", line);
    connect(m_rmButton, SIGNAL(clicked()), parent, SLOT(removeLine()));

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_tempEditor);
    m_layout->addWidget(m_tempLabel);
    m_layout->addWidget(m_hourEditor);
    m_layout->addWidget(m_hourLabel);
    m_layout->addWidget(m_minEditor);
    m_layout->addWidget(m_minLabel);
    m_layout->addWidget(m_rmButton);
}
void ConditionLine::setRmButtonLine(const int &i)
{
    m_rmButton->setProperty("line", i);
}
QJsonObject ConditionLine::getJsonLine() const
{
    QJsonObject line;
    line["temperature"] = m_tempEditor->value();
    line["hour"] = m_hourEditor->value();
    line["min"] = m_minEditor->value();
    return line;
}
QString ConditionLine::getCompleterLine() const
{
    QString ret = "At ";
    ret += m_tempEditor->cleanText() + " °C";
    if(m_hourEditor->value() != 0){
        ret += " for " + m_hourEditor->cleanText() + " h";
        if(m_minEditor != 0){
            ret += " " + m_minEditor->cleanText() + " min";
        }
    }
    else if(m_minEditor->value() != 0){
        ret += " for" + m_minEditor->cleanText() + " min";
    }
    return ret;
}
QString ConditionLine::getSchemeLine() const
{
    QString ret = m_tempEditor->cleanText() + " °C";
    if(m_hourEditor->value() != 0){
        ret += ", " + m_hourEditor->cleanText() + " h";
        if(m_minEditor->value() != 0){
            ret += " " + m_minEditor->cleanText() + " min";
        }
    }
    else if(m_minEditor != 0){
        ret += ", " + m_minEditor->cleanText() + " min";
    }
    return ret;
}


//**********************************************************************************
//                                  ASpinBoxNoWheel
//**********************************************************************************

ASpinBoxNoWheel::ASpinBoxNoWheel(QWidget *parent):
    QSpinBox(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}
void ASpinBoxNoWheel::wheelEvent(QWheelEvent *event) {
    if (!hasFocus()) {
        event->ignore();
    }
    else {
        QSpinBox::wheelEvent(event);
    }
}
