#include <QLabel>
#include <QGridLayout>
#include <QJsonObject>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QToolBar>
#include <QTableWidget>
#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QAxObject>

#include "ACompoundDatabaseView.h"
#include "ACompleterTextEdit.h"
#include "AResearchDialog.h"

ACompoundDatabaseView::ACompoundDatabaseView(ACompoundDatabase *database, QWidget *parent) :
    QWidget(parent),
    m_compoundsArray(database->getCompoundArray()),
    m_database(database)
{
    setFocusPolicy(Qt::StrongFocus);
    grabKeyboard();
    m_compoundsArrayTemp = *m_compoundsArray;
    setWindowTitle("Database viewer");
    resize(500,600);
    //Setting the table widget
    m_table = new QTableWidget(0, 4, this);
    m_table->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_table);
    m_table->setFont(QFont("MS Shell Dlg 2", 11));
    m_table->setItemDelegate(new ACompoundDatabaseViewDelegate(m_table));
    m_table->setHorizontalHeaderLabels({"Name" , "M [g/mol]" , "d" , "Structure"});
    show();
    for(int i = 0; i < m_compoundsArrayTemp.size(); ++i){
        m_table->insertRow(i);
        QJsonObject line = m_compoundsArrayTemp[i].toObject();
        m_table->setItem(i, 0, new QTableWidgetItem(line["name"].toString()));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(line["mm"].toDouble())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(line["d"].toDouble())));

        QPixmap pixmap(m_database->gifFile(line["number"].toInt()), ".gif");
        pixmap = pixmap.scaledToWidth(pixmap.width());
        m_table->setRowHeight(i,std::max(pixmap.height(), m_table->rowHeight(i)));
        QLabel *label = new QLabel(this);
        label->setPixmap(pixmap);
        label->setAlignment(Qt::AlignCenter);

        m_table->setIndexWidget(m_table->model()->index(i,3), label);
    }
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //setting the layout

    //setting the context menu
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, SIGNAL(customContextMenuRequested(QPoint)), this,  SLOT(customMenuRequested(QPoint)));
    m_contextTableMenu =new QMenu(this);

    m_removeAc = new QAction("&Remove the row", this);
    m_contextTableMenu->addAction(m_removeAc);
    connect(m_removeAc, SIGNAL(triggered()), this, SLOT(removeSlot()));
    m_removeAc->setIcon(QIcon(":/General/Icons/removeRow.png"));

    m_copy = new QAction("&Copy");
    m_contextTableMenu->addAction(m_copy);
    connect(m_copy, &QAction::triggered, this, &ACompoundDatabaseView::copyStruct);

    connect(m_table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateData(QTableWidgetItem*)));

    //setting the finder
    m_finder = new AResearchDialog();
    connect(m_finder, SIGNAL(searchRequest(QString)), this, SLOT(goToNext(QString)));
}
void ACompoundDatabaseView::customMenuRequested(QPoint const& pos)
{
    m_indexSelection = m_table->indexAt(pos);
    m_contextTableMenu->popup(m_table->viewport()->mapToGlobal(pos));
}
void ACompoundDatabaseView::removeSlot()
{
    m_filesIndexToRemove << m_compoundsArrayTemp[m_indexSelection.row()].toObject()["number"].toInt();
    m_compoundsArrayTemp.removeAt(m_indexSelection.row());
    m_table->removeRow(m_indexSelection.row());
    save();
}
void ACompoundDatabaseView::save()
{
    *m_compoundsArray = m_compoundsArrayTemp;
    m_database->save(m_filesIndexToRemove);
    m_filesIndexToRemove.clear();
}
void ACompoundDatabaseView::updateData(QTableWidgetItem *item)
{
    int row = item->row();
    QJsonObject obj;
    obj["name"] = m_table->item(row, 0)->text();
    obj["mm"] = m_table->item(row, 1)->text().toDouble();
    obj["d"] = m_table->item(row, 2)->text().toDouble();
    obj["number"] = m_compoundsArray->at(row).toObject()["number"];
    m_compoundsArrayTemp.replace(row, obj);
    save();
}

void ACompoundDatabaseView::goToNext(const QString &word)
{
    const int beg = m_table->currentRow() + 1;
    const int end = m_table->rowCount();
    QTextDocument doc;
    QTableWidgetItem *currentItem;
    for(int i = beg; i < end; i++){
        currentItem = m_table->item(i, 0);
        doc.setHtml(currentItem->text());
        if(doc.toPlainText().contains(word, Qt::CaseInsensitive)){
            m_table->setCurrentItem(currentItem);
            return;
        }
    }
    for(int i = 0; i < beg; i++){
        currentItem = m_table->item(i, 0);
        doc.setHtml(currentItem->text());
        if(doc.toPlainText().contains(word, Qt::CaseInsensitive)){
            m_table->setCurrentItem(currentItem);
            return;
        }
    }
    QMessageBox::warning(nullptr, "warning","Cannot find \"" + word + "\"");
}

void ACompoundDatabaseView::copyStruct() const
{

    const int compoundNumber = m_compoundsArrayTemp[m_table->currentRow()].toObject()["number"].toInt();
    const QString chemdrawFile = m_database->cdxmlFile(compoundNumber);
    QAxObject chemdraw("{84328ED3-9299-409F-8FCC-6BB1BE585D08}");
    chemdraw.dynamicCall(("Open("+chemdrawFile+",False)").toStdString().c_str());
    chemdraw.querySubObject("Objects")->dynamicCall("Copy()");
    chemdraw.deleteLater();
}
void ACompoundDatabaseView::closeEvent(QCloseEvent *event)
{
    deleteLater();
    event->accept();
}
void ACompoundDatabaseView::keyPressEvent(QKeyEvent *event)
{
    auto const key = event->key();
    auto const modif = event->modifiers();
    if(modif == Qt::NoModifier && key == Qt::Key_F){
        m_finder->show();
        return;
    }
    else if(modif == Qt::NoModifier && (key == Qt::Key_Q || key == Qt::Key_C)){
        close();
        return;
    }
    else if(modif == Qt::CTRL && key == Qt::Key_C){
        copyStruct();
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


//*********************************************************
//               ACompoundDatabaseViewDelegate
//*********************************************************


ACompoundDatabaseViewDelegate::ACompoundDatabaseViewDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{

}
QWidget *ACompoundDatabaseViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0){
        auto *editor = new ASingleLineTextEdit(parent);
        editor->setIndex(index);
        connect(editor, SIGNAL(textChanged()), this, SLOT(resizeNameEditor()));
        editor->setAutoFillBackground(true);
        editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        return editor;
    }
    else
        return QStyledItemDelegate::createEditor(parent, option, index);
}
void ACompoundDatabaseViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == 0){
        ACompleterTextEdit *editorCast = qobject_cast<ACompleterTextEdit*>(editor);
        if(editorCast)
        {
            const QString value = index.model()->data(index).toString();
            editorCast->setHtml(value);
        }
    }
    else
        QStyledItemDelegate::setEditorData(editor, index);
}
void ACompoundDatabaseViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == 0){
        ACompleterTextEdit *editorCast = qobject_cast<ACompleterTextEdit*>(editor);
        if(editorCast)
            model->setData(index, editorCast->toHtml());
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}
void ACompoundDatabaseViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    QRect rec(option.rect.x(), option.rect.y(), option.rect.width()-1, option.rect.height()-1);
    if(index.column() == 0)
        editor->setProperty("cellWidht", rec.width());
    editor->setGeometry(rec);
}
void ACompoundDatabaseViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0){

        painter->save();
        auto options = option;
        initStyleOption(&options, index);
        options.text = "";
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

        QTextDocument doc;
        doc.setDefaultFont(QApplication::font());

        doc.setHtml(index.model()->data(index).toString());

        int heightOffset = option.rect.height()/2 - QApplication::fontMetrics().boundingRect(doc.toPlainText()).height();
        painter->translate(option.rect.left(), option.rect.top() + heightOffset);

        QRect clip = QRect(0, 0, option.rect.width(), option.rect.height());
        doc.drawContents(painter, clip);
        painter->restore();
    }
    else
        QStyledItemDelegate::paint(painter, option, index);
}
void ACompoundDatabaseViewDelegate::resizeNameEditor()
{
    auto editor = qobject_cast<ASingleLineTextEdit*>(sender());
    if(editor){
        auto rect = editor->geometry();
        int width = std::max(editor->fontMetrics().boundingRect(editor->toPlainText()).width()+editor->contentsMargins().left()*25, editor->property("cellWidht").toInt());
        rect.setWidth(width);
        editor->setGeometry(rect);
        editor->setAutoFillBackground(true);
    }
}
