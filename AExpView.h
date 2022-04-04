#ifndef AEXPVIEW_H
#define AEXPVIEW_H

#include <QWidget>
#include <QModelIndex>
#include <QStringListModel>
#include <QTextCharFormat>

#include "ACompleterTextEdit.h"

class AExp;
class QMenu;
class QStandardItemModel;
class QStringListModel;
class ACompoundDatabase;
class AReactionScheme;
class AExpTableDelegate;
class QCompleter;
class QListWidgetItem;
class AAnalysisWidget;
class ACompleterTextEdit;

namespace Ui {
class AExpView;
}

class AExpView : public QWidget
{
    Q_OBJECT

public:
    explicit AExpView(AExp *exp, ACompoundDatabase *databaseArg = nullptr, QWidget *parent = nullptr);
    ~AExpView();

    QString getLabel() const;
    void insertRowExpTableModel(const int &row, const int &role = 1, const int &position = 2);
    void setIndexVec(int indexVec);
    QVector<QString> getAnalysisSimple() const;
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);
    void setDatabase(ACompoundDatabase *newDatabase);
    int getExpNumber() const;
    void focusOutEvent(QFocusEvent *event);

public slots:
    void updateWidgetsSize();

    void customMenuRequestedExp(QPoint const& pos);
    void insertRowExpTableModelUp();
    void insertRowExpTableModelDown();
    void removeRowExpTableModel();
    void moveRowUpSlot();
    void moveRowDownSlot();
    void headerChange(int row);
    void upadteSignificantNumber(int n);
    void updateHere();
    void moveCell(ASingleLineTextEdit::MoveCell move);

    void updateUserTitle();
    void updateProcedure();
    void updateNotes();
    void updateReference();
    void updateDate();
    void updateAnalysis(const QStringList &lst);
    void updateCompletionState();

    void saveExpDis();

    void openScheme();
    void updateScheme();

    void addToDatabaseSlot();
    void enableAddToDatabaseAc(bool const& enable);
    void query(QString const& name, int row);

    void updateCompleterModel();


    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);



signals:
    void closeTab(int);

private:
    Ui::AExpView *ui;

    AExp *m_expDis;
    int m_indexVec;
    AExpTableDelegate *m_expTableDelegate;
    ACompoundDatabase *m_database;
    AReactionScheme *m_reactionScheme;
    int m_significantsNumber;
    QModelIndex m_indexSelection;
    QPixmap m_schemePixmap;

    QStandardItemModel *m_completerModel;
    QCompleter *m_entryCompleter;

    QMenu *m_contextExpMenu;
    QAction *m_addRowUpAcExp;
    QAction *m_addRowDownAcExp;
    QAction *m_removeRowAcExp;
    QAction *m_moveRowUpAcExp;
    QAction *m_moveRowDownAcExp;
    QAction *m_addToDatabaseAcExp;
    QAction *m_updateHere;
};

#endif // AEXPVIEW_H
