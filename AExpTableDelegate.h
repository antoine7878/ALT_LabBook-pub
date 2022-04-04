#ifndef AEXPTABLEDELEGATE_H
#define AEXPTABLEDELEGATE_H

#include <QStyledItemDelegate>
#include "ACompleterTextEdit.h"


class QTableView;
class QTextEdit;
class ACompleterTextEdit;

class AExpTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit AExpTableDelegate(int significantNumber, QStringList* databaseList, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool eventFilter(QObject *object, QEvent *event);
    QSize sizeHint (const QStyleOptionViewItem & option, const QModelIndex & index) const;

public slots:
    void commitPosition();
    void structButtonClicked();
    void queryDatabase(const QString &name);
    void updateSignificantNumber(int n);
    void resizeNameEditor();

signals:
    void updateStructure(int row);
    void query(const QString &name, int row);
    void nameEditorEntered(ACompleterTextEdit*);
    void moveCell(ASingleLineTextEdit::MoveCell);

private:
    int m_significantNumber;
    QStringList* m_databaseList;
};

#endif // AEXPTABLEDELEGATE_H
