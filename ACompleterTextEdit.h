#ifndef ACOMPLETERTEXTEDIT_H
#define ACOMPLETERTEXTEDIT_H

#include <QTextEdit>
#include <QModelIndex>

class QCompleter;


class ACompleterTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    enum ForamtEdit {
        SubScript = 0,
        SuperScript,
        Bold,
        Italic,
        Underline
    };

    ACompleterTextEdit(QWidget *parent = nullptr);
    ~ACompleterTextEdit();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;
    void contextMenuEvent(QContextMenuEvent *event);
    void setIndex(QModelIndex index);
    QModelIndex getIndex() const;
    void insertFromMimeData ( const QMimeData * source );


protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

protected slots:
    void insertCompletion(const QString &completion);    
    void makeSuperScript();
    void makeSubScript();
    void makeBold();
    void makeItalic();
    void makeUnderLine();
    void changeFormat(int formatChange);

signals:
    void finishedEditing();

protected:
    QString textUnderCursor() const;


    QModelIndex m_index;
    QCompleter *c = nullptr;
    QAction *m_subScript;
    QAction *m_superScript;
    QAction *m_bold;
    QAction *m_italic;
    QAction *m_underline;
};


class ASingleLineTextEdit : public ACompleterTextEdit
{
    Q_OBJECT

public:

    enum MoveCell {
        Up = 0,
        Left,
        Down,
        Right
    };


    ASingleLineTextEdit(QWidget *parent = nullptr);
    ~ASingleLineTextEdit();
    void contextMenuEvent(QContextMenuEvent *event);

protected:
    void keyPressEvent(QKeyEvent *e);
    void insertFromMimeData(const QMimeData *source);

signals:
    void moveCell(ASingleLineTextEdit::MoveCell);
};

#endif // ACOMPLETERTEXTEDIT_H
