#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QKeyEvent>

#include "AResearchDialog.h"

//******************************************************************************************************************************
//**********When instanciating, connect SIGNAL(searchRequest(QString)) to a slot looking for the QString in the set.
//m_finder = new AResearchDialog;
//connect(m_finder, SIGNAL(searchRequest(QString)), this, SLOT(goToNext(QString)));
//void AFoo::goToNext(const QString &word)
//{
//    const int beg = m_table->currentRow();
//    const int end = m_table->rowCount();
//    int row;
//    for(int i = 1; i <= end; i++){
//        i < beg ? row = i + beg : row = i - beg;
//        QTableWidgetItem *currentItem = m_table->item(row, 0);
//        if(currentItem->text().contains(word, Qt::CaseInsensitive)){
//            m_table->setCurrentItem(currentItem);
//            return;
//        }
//    }
//    QMessageBox::warning(nullptr, "warning","Cannot find \"" + word + "\"");
//}
//********set Ctrl+F to open  researchDialog
//void AFoo::keyPressEvent(QKeyEvent *event)
//{
//    if(event->modifiers() == Qt::CTRL && event->key() == Qt::Key_F){
//        m_finder->show();
//    }
//}
//************close researcDial when closing the widget it serches on.
//void AFoo::closeEvent(QCloseEvent *event)
//{
//    deleteLater();
//    m_finder->deleteLater();
//    QWidget::closeEvent(event);
//}
//******************************************************************************************************************************

AResearchDialog::AResearchDialog(QWidget *parent):
    QWidget(parent, Qt::WindowStaysOnTopHint |
            Qt::CustomizeWindowHint |
            Qt::WindowTitleHint |
            Qt::WindowCloseButtonHint)
{
    setWindowTitle("Find");
    label = new QLabel("find what: ");
    line = new QLineEdit;
    findButton = new QPushButton("Find");
    layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(line);
    layout->addWidget(findButton);
    connect(findButton, SIGNAL(clicked()), this, SLOT(sendsearchRequest()));
}

void AResearchDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() ==  Qt::Key_Return || event->key() ==  Qt::Key_Enter){
        sendsearchRequest();
    }
}

void AResearchDialog::sendsearchRequest()
{
    emit searchRequest(line->text());
}
