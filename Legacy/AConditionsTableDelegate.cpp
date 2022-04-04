#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QDebug>
#include <QApplication>

#include "AConditionsTableDelegate.h"
#include "AConditionsTableModel.h"


AconditionTableDelegate::AconditionTableDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{

}

QWidget *AconditionTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == AConditionsTableModel::Temperature)
    {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setRange(-273.5, __DBL_MAX__);
        editor->setAutoFillBackground(true);
        editor->setButtonSymbols(QAbstractSpinBox::NoButtons);
        return editor;
    }
    else if (index.column() == AConditionsTableModel::Duration)
    {
        QLineEdit *editor = new QLineEdit(parent);
        editor->setInputMask("99\\d99\\h99\\m\\i\\n");
        editor->setAutoFillBackground(true);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}
void AconditionTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == AConditionsTableModel::Temperature)
    {
        auto editorCast = qobject_cast<QDoubleSpinBox*>(editor);
        if (editorCast)
        {
            editorCast->setValue(index.model()->data(index).toDouble());
        }
    }
    else if (index.column() == AConditionsTableModel::Duration)
    {
        QLineEdit* editorCast = qobject_cast<QLineEdit*>(editor);
        if (editorCast)
        {
            editorCast->setText(index.model()->data(index).toString());
        }
    }
    else
        QStyledItemDelegate::setEditorData(editor, index);
}
void AconditionTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == AConditionsTableModel::Temperature)
    {
        auto editorCast = qobject_cast<QDoubleSpinBox*>(editor);
        if (editorCast)
        {
            model->setData(index, editorCast->value());
        }
    }
    else if (index.column() == AConditionsTableModel::Duration)
    {
        QLineEdit* editorCast = qobject_cast<QLineEdit*>(editor);
        if (editorCast)
        {
            model->setData(index, editorCast->text());
            qDebug() << editorCast->text();
        }
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}
void AconditionTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
void AconditionTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //    if(index.column() == AConditionsTableModel::Duration)
    //    {
    //        QStyleOptionViewItem itemOption(option);

    //        initStyleOption(&itemOption, index);
    //        itemOption.text = index.model()->data(index, AConditionsTableModel::Duration).toString();

    //        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
    //    }
    //    else
    QStyledItemDelegate::paint(painter, option, index);
}
