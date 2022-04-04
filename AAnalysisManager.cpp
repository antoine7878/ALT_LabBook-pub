#include <QListWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QPushButton>

#include "AAnalysisManager.h"

//AAnalysis methodes
AAnalysisManager::AAnalysisManager(const QVector<QPair<QString, bool> > &analysis, QWidget *parent):
    QDialog(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Analysis manager");
    m_list = new AListWidget(analysis);

    m_lineEdit = new QLineEdit;
    connect(m_lineEdit, SIGNAL(returnPressed()), this, SLOT(addAnalysis()));

    m_buttonRemoveAll = new QPushButton("Clear all");
    connect(m_buttonRemoveAll, SIGNAL(clicked()), this, SLOT(removeAll()));

    m_layout = new QGridLayout;
    m_layout->addWidget(m_list, 0,0,1,2);
    m_layout->addWidget(m_lineEdit,1,0);
    m_layout->addWidget(m_buttonRemoveAll,1,1);
    setLayout(m_layout);
}
QVector<QPair<QString, bool>> AAnalysisManager::execList()
{
    exec();
    return m_list->getList();
}
void AAnalysisManager::addAnalysis()
{
    m_list->addAnalysis(m_lineEdit->text());
    m_lineEdit->clear();
}

void AAnalysisManager::removeAll()
{
    for(int i = 0; i < m_list->count();i++){
        const auto item = m_list->item(i);
        if(item->checkState())
            m_list->checkNHighlight(item);
    }
}
void AAnalysisManager::keyPressEvent(QKeyEvent *event){
    const auto key = event->key();
    const auto modif = event->modifiers();

    if(modif == Qt::NoModifier){
        if(key == Qt::Key_Q ||
                key == Qt::Key_V)
            close();
        else if(key == Qt::Key_E ||
                key == Qt::Key_Enter ||
                key == Qt::Key_Return)
            m_list->checkNHighlight(m_list->currentItem());
    }

    if(modif == Qt::CTRL)
        if(key == Qt::Key_A)
            removeAll();


    if((key == Qt::Key_S ||
        key == Qt::Key_W ||
        key == Qt::Key_Up ||
        key == Qt::Key_Down) &&
            (modif == Qt::NoModifier ||
             modif == Qt::ALT)){

        int row = m_list->currentRow();
        int jump = 1;
        if(modif == Qt::ALT)
            jump  = 10;

        if(key == Qt::Key_W ||
                key == Qt::Key_Up){
            row -= jump;
            if(row < 0)
                row = m_list->count() - 1;
        }
        else if(key == Qt::Key_S ||
                key == Qt::Key_Down){
            row += jump;
            if(row > m_list->count() - 1)
                row = 0;
        }
        m_list->setCurrentRow(row);
        m_list->setFocus();
    }


    else
        QWidget::keyPressEvent(event);
}

//AListWidget methodes
AListWidget::AListWidget(const QVector<QPair<QString, bool> > &analysis, QWidget *parent):
    QListWidget(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);

    for(auto &i : analysis){
        addItem(i.first);
        QListWidgetItem *itemC = item(count()-1);
        itemC->setFlags(itemC->flags() ^ Qt::ItemIsUserCheckable);
        if(i.second == true)
            itemC->setCheckState(Qt::Checked);
        else
            itemC->setCheckState(Qt::Unchecked);
        highlightChecked(itemC);
    }

    connect(this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(highlightChecked(QListWidgetItem*)));
    connect(this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(updateAnalysisList()));
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(checkNHighlight(QListWidgetItem*)));
    updateAnalysisList();
}
void AListWidget::highlightChecked(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Checked)
        item->setBackground(QColor(66, 245, 75));
    else
        item->setBackground(QColor(255, 255, 255));
}

void AListWidget::checkNHighlight(QListWidgetItem *item)
{
    item->checkState() ? item->setCheckState(Qt::Unchecked) : item->setCheckState(Qt::Checked);
}
void AListWidget::addAnalysis(const QString &analysis)
{
    if(findItems(analysis,Qt::MatchFixedString).size() < 0){
        qDebug() << "Trying to add an analysis twice (" << analysis << ")";
        return;
    }
    addItem(analysis);
    QListWidgetItem *curItem = item(count()-1);
    curItem->setFlags(curItem->flags() | Qt::ItemIsUserCheckable);
    curItem->setCheckState(Qt::Unchecked);
    setCurrentItem(curItem);
}
void AListWidget::removeAnalysis()
{
    delete takeItem(currentRow());
}
void AListWidget::keyPressEvent(QKeyEvent *event)
{
    const auto key = event->key();
    if(key == Qt::Key_Delete || key == Qt::Key_Backspace)
        removeAnalysis();
    else
        QWidget::keyPressEvent(event);
}
void AListWidget::updateAnalysisList()
{
    m_retList.clear();
    for(int i = 0; i<count();i++){
        auto curItem = item(i);
        m_retList << QPair<QString,bool>(curItem->text(), curItem->checkState());
    }
}
QVector<QPair<QString, bool> > AListWidget::getList()
{
    return m_retList;
}
