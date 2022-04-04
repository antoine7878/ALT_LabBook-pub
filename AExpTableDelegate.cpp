#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCompleter>
#include <QStandardItemModel>
#include <QPainter>
#include <QApplication>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QFocusEvent>
#include <QTableView>
#include <QDebug>

#include <cmath>

#include "AExpTableDelegate.h"
#include "AExpTableModel.h"
#include "ADoubleValidator.h"
#include "ALT.h"

AExpTableDelegate::AExpTableDelegate(int significantNumber, QStringList *databaseList, QObject *parent):
    QStyledItemDelegate(parent),
    m_significantNumber(significantNumber),
    m_databaseList(databaseList)
{

}

QWidget *AExpTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == AExpTableModel::Name)
    {
        auto *editor = new ASingleLineTextEdit(parent);
        connect(editor, &ASingleLineTextEdit::moveCell, this, &AExpTableDelegate::moveCell);
        editor->setIndex(index);
        connect(editor, SIGNAL(textChanged()), this, SLOT(resizeNameEditor()));
        editor->setAutoFillBackground(true);
        editor->setFocusPolicy(Qt::StrongFocus);
        editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QCompleter *completer = new QCompleter(*m_databaseList, parent);
        connect(completer, SIGNAL(activated(QString)), this, SLOT(queryDatabase(QString)));

        completer->setProperty("row", index.row());
        editor->setCompleter(completer);
        return editor;
    }
    else if (index.column() == AExpTableModel::Structure)
    {
        QPushButton *structButton = new QPushButton(parent);
        structButton->installEventFilter(const_cast<AExpTableDelegate *>(this));
        structButton->setAutoFillBackground(true);
        structButton->setIcon(QIcon(":/General/Icons/molecule"));
        structButton->setProperty("row", index.row());
        connect(structButton, SIGNAL(clicked()), this, SLOT(structButtonClicked()));
        return structButton;
    }
    else if (index.column() == AExpTableModel::Position)
    {
        QComboBox *positionEditor = new QComboBox(parent);

        positionEditor->installEventFilter(const_cast<AExpTableDelegate *>(this));
        connect(positionEditor, SIGNAL(activated(int)), this, SLOT(commitPosition()));

        positionEditor->setAutoFillBackground(true);
        positionEditor->addItem("None");
        positionEditor->addItem(QIcon(":/General/Icons/leftStruct"), "Left");
        positionEditor->addItem(QIcon(":/General/Icons/upStruct"), "Up");
        positionEditor->addItem(QIcon(":/General/Icons/rightStruct"), "Right");
        positionEditor->addItem(QIcon(":/General/Icons/downStruct"), "Down");
        positionEditor->addItem(QIcon(":/General/Icons/leftName"), "Left");
        positionEditor->addItem(QIcon(":/General/Icons/upName"), "Up");
        positionEditor->addItem(QIcon(":/General/Icons/rightName"), "Right");
        positionEditor->addItem(QIcon(":/General/Icons/downName"), "Down");

        return positionEditor;
    }

    else if (index.column() == AExpTableModel::Molarmass ||
             index.column() == AExpTableModel::Mmoles ||
             index.column() == AExpTableModel::Density ||
             index.column() == AExpTableModel::Purity ||
             index.column() == AExpTableModel::Equiv ||
             index.column() == AExpTableModel::Mmoles ||
             index.column() == AExpTableModel::Concentration ||
             index.column() == AExpTableModel::Mass ||
             index.column() == AExpTableModel::Volume)
    {
        QLineEdit *line = new QLineEdit(parent);
        line->setContextMenuPolicy(Qt::NoContextMenu);
        line->setAutoFillBackground(true);
        ADoubleValidator *val = new ADoubleValidator(-std::pow(10, 100), std::pow(10, 100), 20, parent);
        line->setValidator(val);
        return line;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}
void AExpTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == AExpTableModel::Name)
    {
        ACompleterTextEdit *editorCast = qobject_cast<ACompleterTextEdit*>(editor);
        if(editorCast)
        {
            const QString value = index.model()->data(index).toString();
            editorCast->setHtml(value);
        }

    }
    else if (index.column() == AExpTableModel::Structure)
    {
        return;
    }
    else if (index.column() == AExpTableModel::Position)
    {
        QComboBox *positionEditor = qobject_cast<QComboBox *>(editor);
        if (positionEditor)
        {
            const int value = index.model()->data(index).toInt();
            positionEditor->setCurrentIndex(value);
        }
    }
    else if(
            index.column() == AExpTableModel::Molarmass ||
            index.column() == AExpTableModel::Mmoles ||
            index.column() == AExpTableModel::Density ||
            index.column() == AExpTableModel::Purity ||
            index.column() == AExpTableModel::Equiv ||
            index.column() == AExpTableModel::Mmoles ||
            index.column() == AExpTableModel::Concentration ||
            index.column() == AExpTableModel::Mass ||
            index.column() == AExpTableModel::Volume)
    {
        QLineEdit *lineEditor = qobject_cast<QLineEdit *>(editor);
        if (lineEditor)
        {
            const QString str = QString::number(index.model()->data(index).toDouble());
            lineEditor->setText(str);
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}
void AExpTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == AExpTableModel::Name)
    {
        ACompleterTextEdit *editorCast = qobject_cast<ACompleterTextEdit*>(editor);
        if(editorCast)
            model->setData(index, editorCast->toHtml());
    }
    else if (index.column() == AExpTableModel::Structure)
    {
        return;
    }
    else if (index.column() == AExpTableModel::Position)
    {
        QComboBox *positionEditor = qobject_cast<QComboBox *>(editor);
        if (editor)
        {
            model->setData(index, positionEditor->currentIndex(), Qt::EditRole);
        }
    }
    else if(
            index.column() == AExpTableModel::Molarmass ||
            index.column() == AExpTableModel::Mmoles ||
            index.column() == AExpTableModel::Density ||
            index.column() == AExpTableModel::Purity ||
            index.column() == AExpTableModel::Equiv ||
            index.column() == AExpTableModel::Mmoles ||
            index.column() == AExpTableModel::Concentration ||
            index.column() == AExpTableModel::Mass ||
            index.column() == AExpTableModel::Volume)
    {
        QLineEdit *lineEditor = qobject_cast<QLineEdit *>(editor);
        if (lineEditor)
        {
            if(lineEditor->text() == " ")
                model->setData(index, 0);
            else
                model->setData(index, lineEditor->text().toDouble());
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
void AExpTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    Q_UNUSED(index);
    QRect rec(option.rect.x(), option.rect.y(), option.rect.width()-1, option.rect.height()-1);
    if(index.column() == AExpTableModel::Name)
        editor->setProperty("cellWidht", rec.width());
    editor->setGeometry(rec);
}
void AExpTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == AExpTableModel::Name)
    {
        auto options = option;
        initStyleOption(&options, index);
        painter->save();
        options.text = "";
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

        QTextDocument doc;
        doc.setHtml(index.model()->data(index).toString());
        int heightOffset = option.rect.height()/2 - QApplication::fontMetrics().boundingRect(doc.toPlainText()).height();
        painter->translate(option.rect.left(), option.rect.top() + heightOffset);
        QRect clip(0, 0, option.rect.width(), option.rect.height());
        doc.drawContents(painter, clip);
        painter->restore();

    }
    else if(index.column() == AExpTableModel::Position)
    {
        QStyleOptionComboBox comboOption;
        comboOption.rect = option.rect;
        comboOption.fontMetrics = QApplication::fontMetrics();
        int comboIndex = index.model()->data(index).toInt();

        switch(comboIndex){
        case 0:
            comboOption.currentText = "None";
            break;
        case 1 :
            comboOption.currentText = "Left";
            comboOption.currentIcon = QIcon(":/General/Icons/molecule");
            break;
        case 2 :
            comboOption.currentText = "Up";
            comboOption.currentIcon = QIcon(":/General/Icons/upStruct");
            break;
        case 3 :
            comboOption.currentText = "Right";
            comboOption.currentIcon = QIcon(":/General/Icons/rightStruct");
            break;
        case 4 :
            comboOption.currentText = "Down";
            comboOption.currentIcon = QIcon(":/General/Icons/downStruct");
            break;
        case 5 :
            comboOption.currentText = "Left";
            comboOption.currentIcon = QIcon(":/General/Icons/leftName");
            break;
        case 6 :
            comboOption.currentText = "Up";
            comboOption.currentIcon = QIcon(":/General/Icons/upName");
            break;
        case 7 :
            comboOption.currentText = "Right";
            comboOption.currentIcon = QIcon(":/General/Icons/rightName");
            break;
        case 8 :
            comboOption.currentText = "Down";
            comboOption.currentIcon = QIcon(":/General/Icons/downName");
            break;
        }

//        if(comboIndex == 1 || comboIndex == 5)
//            comboOption.currentText = "Left";
//        else if(comboIndex == 2 || comboIndex == 6)
//            comboOption.currentText = "Up";
//        else if(comboIndex == 3 || comboIndex == 7)
//            comboOption.currentText = "Right";
//        else if(comboIndex == 4 || comboIndex == 8)
//            comboOption.currentText = "Down";
//        else
//            comboOption.currentText = "None";

//        if(comboIndex >= 1 && comboIndex <= 4)
//            ico = QIcon(":/General/Icons/molecule");
//        else if(comboIndex >= 5 && comboIndex <= 8)
//            ico = QIcon(":/General/Icons/a");
//        comboOption.currentIcon = ico;

        comboOption.frame = false;
        comboOption.iconSize = QSize(option.rect.width()/4, option.rect.height()/1.8);

        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &comboOption, painter);
    }
    else if(index.column() == AExpTableModel::Molarmass ||
            index.column() == AExpTableModel::Equiv)
    {
        QStyleOptionComboBox line;
        line.rect = option.rect;
        line.fontMetrics = QApplication::fontMetrics();
        if(index.model()->data(index).toDouble() != 0)
            line.currentText = QString::number(index.model()->data(index).toDouble());
        line.frame = false;

        if(!index.model()->data(index, Qt::UserRole).toBool()){
            painter->fillRect(option.rect, QBrush(Qt::red));
        }

        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &line, painter);
    }
    else if(
            index.column() == AExpTableModel::Mmoles ||
            index.column() == AExpTableModel::Density ||
            index.column() == AExpTableModel::Purity ||
            index.column() == AExpTableModel::Mmoles ||
            index.column() == AExpTableModel::Concentration ||
            index.column() == AExpTableModel::Mass ||
            index.column() == AExpTableModel::Volume)
    {
        QStyleOptionComboBox line;
        line.rect = option.rect;
        line.fontMetrics = QApplication::fontMetrics();
        if(index.model()->data(index).toDouble() != 0)
            line.currentText = ALT::displayX(index.model()->data(index).toDouble(), m_significantNumber);
        line.frame = false;

        if(!index.model()->data(index, Qt::UserRole).toBool()){
            painter->fillRect(option.rect, QBrush(Qt::red));
        }

        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &line, painter);
    }
    else{
        QStyledItemDelegate::paint(painter, option, index);
    }
}
bool AExpTableDelegate::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::FocusIn){
        QComboBox *comboBox = qobject_cast<QComboBox*>(object);
        if(comboBox){
            comboBox->showPopup();
            comboBox->removeEventFilter(this);
        }
    }

//    ASingleLineTextEdit *editor = qobject_cast<ASingleLineTextEdit*>(object);
//    if(editor) {
//        // if leave Editor
//        if((event->type() == QEvent::FocusOut)){
//            // Set focus on editor to allow after, the emit of QEvent::Type(FocusOut) normally
//            emit commitData(editor);
//            editor->close();
//            return true;
//        }
//    }


    return QStyledItemDelegate::eventFilter(object, event);
}
QSize AExpTableDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    //    if(index.column() == AExpTableModel::Name)
    //    {
    //        auto options = option;
    //        initStyleOption(&options, index);

    //        QTextDocument doc;
    //        doc.setHtml(options.text);
    //        doc.setTextWidth(options.rect.width());
    //        return QSize(doc.idealWidth(), doc.size().height());
    //    }
    //    else{
    return QStyledItemDelegate::sizeHint(option, index);
    //    }
}

void AExpTableDelegate::commitPosition()
{
    QComboBox *combo = dynamic_cast<QComboBox *>(sender()) ;
    if(combo){
        emit commitData(combo);
        emit closeEditor(combo);
    }
}
void AExpTableDelegate::structButtonClicked()
{
    emit updateStructure(sender()->property("row").toInt());
}
void AExpTableDelegate::queryDatabase(const QString &name)
{
    emit query(name, (sender()->property("row")).toInt());
}
void AExpTableDelegate::updateSignificantNumber(int n)
{
    m_significantNumber = n;
}
void AExpTableDelegate::resizeNameEditor()
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

