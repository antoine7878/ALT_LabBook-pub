#include <QLabel>
#include <QGridLayout>
#include <QHeaderView>
#include <QTableWidget>
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QAxObject>
#include <QAxWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>

#include "ALabBookViewer.h"
#include "ALT.h"
#include "AResearchDialog.h"

ALabBookViewer::ALabBookViewer(const QStringList &labBookanalysisList, const int &expNumber, const QString prefix, const QString &labBookDir, QWidget *parent):
    QWidget(parent),
    m_dir(labBookDir)
{
    setFocusPolicy(Qt::StrongFocus);
    grabKeyboard();
    setWindowTitle("Lab notebook viewer");
    resize(1600,800);

    m_contextMenu = new QMenu;
    m_copy = new QAction("Copy scheme");
    connect(m_copy, &QAction::triggered, this, &ALabBookViewer::copyScheme);
    m_contextMenu->addAction(m_copy);

    m_openExp = new QAction("Open exp");
    connect(m_openExp, &QAction::triggered, this, &ALabBookViewer::openExpSlot);
    m_contextMenu->addAction(m_openExp);


    //Setting the table widget
    m_table = new QTableWidget(expNumber, 6+labBookanalysisList.size(), this);
    connect(m_table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequestedExp(QPoint)));
    connect(m_table, &QTableWidget::currentCellChanged, this, &ALabBookViewer::setIndex);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList headerList({"Exp name", "Creation date", "Title", "Scale (mmol)", "Yield (%)", "Scheme"});
    const int preAnalysis = headerList.size();
    headerList.reserve(labBookanalysisList.size());
    for(auto &i : labBookanalysisList)
        headerList << i;

    m_table->setHorizontalHeaderLabels(headerList);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);

    //setting the layout
    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_table);

    show();
    for(int i = 0; i < expNumber; i++){
        QString expFile = labBookDir + prefix + "-" + QString::number(i+1) + "/" + prefix + "-" + QString::number(i+1);
        const QString schemeFile = expFile + "_scheme.gif";
        expFile += "_AExp.json";
        QFile saveFile(expFile);

        if (saveFile.open(QIODevice::ReadOnly)) {
            QByteArray saveData = saveFile.readAll();
            QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
            QJsonObject expObject(loadDoc.object());

            QTableWidgetItem *nameItem = new QTableWidgetItem(expObject.value("expName").toString());
            nameItem->setBackground(QColor((Qt::GlobalColor) expObject.value("completionState").toInt()));
            m_table->setItem(i, 0, nameItem);
            m_table->setItem(i, 1, new QTableWidgetItem(QDate::fromString(expObject.value("creationDate").toString()).toString("dd'.'MM'.'yyyy")));
            m_table->setItem(i, 2, new QTableWidgetItem(expObject.value("userTitle").toString()));
            m_table->setItem(i, 3, new QTableWidgetItem(ALT::displayX(expObject.value("xlsx_scale").toDouble(), 2)));
            m_table->setItem(i, 4, new QTableWidgetItem(ALT::displayX(expObject.value("xlsx_yield").toDouble(), 2)));

            QPixmap pixmap(schemeFile,".gif");
            m_table->setRowHeight(i, pixmap.height());
            QLabel *label = new QLabel(this);
            label->setPixmap(pixmap);
            label->setAlignment(Qt::AlignCenter);
            m_table->setIndexWidget(m_table->model()->index(i,5), label);

            const QJsonArray expAnalysis = expObject.value("analysis").toArray();
            QString color;
            for(int j = 0; j < labBookanalysisList.size(); j++){
                color = "#DCDCDC";
                for(const auto &expa: expAnalysis){
                    const QJsonObject obj = expa.toObject();
                    if(obj["name"] == labBookanalysisList[j]){
                        if(obj["value"] == true)
                            color ="#42f54b";
                        else
                            color ="#FF0000";
                        break;
                    }
                }
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setBackground(QColor(color));
                m_table->setItem(i,j+preAnalysis, item);
            }

        }
        else
            qWarning() << "Couldn't open exp" << i << "file for index loading.";
    }
    m_table->resizeColumnsToContents();

    //Setting the find window
    m_finder = new AResearchDialog;
    connect(m_finder, SIGNAL(searchRequest(QString)), this, SLOT(goToNext(QString)));
}

ALabBookViewer::~ALabBookViewer()
{
    if(m_finder)
        m_finder->deleteLater();
}
void ALabBookViewer::closeEvent(QCloseEvent *event)
{
    deleteLater();
    QWidget::closeEvent(event);
}

void ALabBookViewer::goToNext(const QString &word)
{
    const int beg = m_table->currentRow() + 1;
    const int end = m_table->rowCount();
    for(int i = beg; i < end; i++){
        QTableWidgetItem *currentItem = m_table->item(i, 2);
        if(currentItem->text().contains(word, Qt::CaseInsensitive)){
            m_table->setCurrentItem(currentItem);
            return;
        }
    }
    for(int i = 0; i < beg; i++){
        QTableWidgetItem *currentItem = m_table->item(i, 2);
        if(currentItem->text().contains(word, Qt::CaseInsensitive)){
            m_table->setCurrentItem(currentItem);
            return;
        }
    }
    QMessageBox::warning(nullptr, "warning","Cannot find \"" + word + "\"");
}
void ALabBookViewer::keyPressEvent(QKeyEvent *event)
{
    const auto key = event->key();
    const auto modif = event->modifiers();

    if(modif == Qt::CTRL && key == Qt::Key_F){
        m_finder->show();
    }
    else if(modif == Qt::NoModifier && (key == Qt::Key_Q || key == Qt::Key_Y)){
        close();
        return;
    }
    else if(modif == Qt::NoModifier && key == Qt::Key_E){
        openExpSlot();
    }
    else if(modif == Qt::CTRL && key == Qt::Key_C){
        copyScheme();
        return;
    }

    if((key == Qt::Key_A ||
        key == Qt::Key_S ||
        key == Qt::Key_D ||
        key == Qt::Key_W ||
        key == Qt::Key_Left ||
        key == Qt::Key_Up ||
        key == Qt::Key_Down ||
        key == Qt::Key_Right) &&
            (modif == Qt::NoModifier ||
             modif == Qt::ALT)){

        int row = m_table->currentIndex().row();
        int col = m_table->currentIndex().column();
        int jump = 1;
        if(modif == Qt::ALT)
            jump  = 10;

        if(key == Qt::Key_W ||
                key == Qt::Key_Up){
            row -= jump;
            if(row < 0)
                row = m_table->rowCount() - 1;
            m_table->setCurrentIndex(m_table->model()->index(row, col));
        }
        else if(key == Qt::Key_S ||
                key == Qt::Key_Down){
            row += jump;
            if(row > m_table->rowCount() - 1)
                row = 0;
            m_table->setCurrentIndex(m_table->model()->index(row, col));
        }
        else if(key == Qt::Key_D ||
                key == Qt::Key_Right){
            col += 1;
            if(col > m_table->columnCount() - 1)
                col = 0;
            m_table->setCurrentIndex(m_table->model()->index(row, col));
            if(modif == Qt::ALT){
                m_table->setCurrentIndex(m_table->model()->index(row, m_table->columnCount() - 1));
            }
        }
        else if(key == Qt::Key_A ||
                key == Qt::Key_Left){
            col -= 1;
            if(col < 0)
                col = m_table->columnCount() - 1;
            m_table->setCurrentIndex(m_table->model()->index(row, col));
            if(modif == Qt::ALT){
                m_table->setCurrentIndex(m_table->model()->index(row, 0));
            }
        }
        m_table->setFocus();
        return;
    }
    else
        QWidget::keyPressEvent(event);
}
void ALabBookViewer::copyScheme() const
{
    const QString expName = m_table->item(m_contextIndex.row(), 0)->text();
    const QString chemdrawFile = m_dir + expName + "/" + expName + "_scheme.cdxml";
    QAxObject chemdraw("{84328ED3-9299-409F-8FCC-6BB1BE585D08}");
    chemdraw.dynamicCall(("Open("+chemdrawFile+",False)").toStdString().c_str());
    chemdraw.querySubObject("Objects")->dynamicCall("Copy()");
    chemdraw.deleteLater();

}
void ALabBookViewer::resizeEvent(QResizeEvent *event)
{
    m_table->resizeColumnsToContents();
    QWidget::resizeEvent(event);
}
void ALabBookViewer::customMenuRequestedExp(QPoint const& pos)
{
    m_contextIndex = m_table->indexAt(pos);
    m_contextMenu->popup(m_table->viewport()->mapToGlobal(pos));
}

void ALabBookViewer::openExpSlot()
{
    emit openExpSignal(m_table->rowCount() - m_contextIndex.row() - 1);
    close();
}

void ALabBookViewer::setIndex()
{
    m_contextIndex = m_table->currentIndex();
}
