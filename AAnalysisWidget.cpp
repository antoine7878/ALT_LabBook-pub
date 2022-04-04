#include <QFormLayout>
#include <QCheckBox>
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QMenu>
#include <QWidgetAction>
#include <QMutexLocker>

#include "AAnalysisWidget.h"
#include "flowlayout.h"

AAnalysisWidget::AAnalysisWidget(QWidget *parent):
    QWidget(parent),
    m_layout(new FlowLayout(this))
{
}
bool AAnalysisWidget::addAnalysis(const QString &analysis, bool activated, bool send)
{
    for(auto &i : m_analysisList){
        if(i->text() == analysis){
            qDebug() << "Trying to add an analysis twice (" << analysis << ")";
            return false;
        }
    }
    auto cb = new QCheckBox(analysis);
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(changeState()));
    cb->setChecked(activated);
    m_layout->addWidget(cb);
    m_analysisList.append(cb);
    if(send)
        changeState();
    return true;
}
void AAnalysisWidget::load(const QVector<QPair<QString, bool>> &lst)
{
    for(int i = 0; i<lst.size(); i++)
        addAnalysis(lst.value(i).first, lst.value(i).second, false);
}
void AAnalysisWidget::removeAnalysis(int index)
{
        delete m_layout->takeAt(index);
        delete m_analysisList.takeAt(index);
}
void AAnalysisWidget::updateAnalysis(QStringList lst)
{
    QVector<QString> tempStrLst;
    tempStrLst.reserve(m_analysisList.size());
    QVector<bool> tempBoolLst;
    tempBoolLst.reserve(m_analysisList.size());

    while(!m_analysisList.isEmpty()){
        tempStrLst.append(m_analysisList.first()->text());
        tempBoolLst.append(m_analysisList.first()->isChecked());
        removeAnalysis(0);
    }

    for(auto &i : lst){
        int index = -1;
        if((index = tempStrLst.indexOf(i)) != -1)
            addAnalysis(i, tempBoolLst.value(index), false);
        else
            addAnalysis(i, false, false);
    }
    changeState();
}
QVector<QString> AAnalysisWidget::getAnalysisListSimple()
{
    QVector<QString> ret;
    ret.reserve(m_analysisList.size());
    for(auto i = m_analysisList.begin(); i != m_analysisList.end(); i++)
        ret.append((*i)->text());
    return ret;
}
void AAnalysisWidget::changeState()
{
    m_saveList.clear();
    for(auto &i : m_analysisList)
        m_saveList.append(QPair<QString,bool>(i->text(), i->isChecked()));
    emit changed(m_saveList);
}
