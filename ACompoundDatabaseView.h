#ifndef ACOMPOUNDDATABASEVIEW_H
#define ACOMPOUNDDATABASEVIEW_H

#include <QWidget>
#include <QModelIndex>
#include <QJsonArray>
#include <QStyledItemDelegate>

#include "ACompoundDatabase.h"

class QGridLayout;
class QTableWidget;
class QMenu;
class QAction;
class QTableWidgetItem;
class AResearchDialog;

class ACompoundDatabaseView : public QWidget
{
    Q_OBJECT
public:
    explicit ACompoundDatabaseView(ACompoundDatabase *database, QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void customMenuRequested(QPoint const& point);
    void removeSlot();
    void save();
    void updateData(QTableWidgetItem *item);
    void closeEvent(QCloseEvent *event) override;
    void goToNext(const QString &word);
    void copyStruct() const;

signals:
    void saveRequest(const QVector<int> &index);

private:
    QTableWidget *m_table;
    QGridLayout *m_layout;
    QMenu *m_contextTableMenu;
    QModelIndex m_indexSelection;
    QAction *m_removeAc;
    QAction *m_copy;

    QJsonArray *m_compoundsArray;
    QJsonArray m_compoundsArrayTemp;
    QVector<int> m_filesIndexToRemove;
    ACompoundDatabase *m_database;
    AResearchDialog *m_finder;

};

class ACompoundDatabaseViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ACompoundDatabaseViewDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

public slots:
    void resizeNameEditor();
};

#endif // ACOMPOUNDDATABASEVIEW_H
