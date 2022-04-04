#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QMimeData>
#include <QCoreApplication>
#include <QMimeData>

#include "ACompleterTextEdit.h"

ACompleterTextEdit::ACompleterTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    m_superScript = new QAction(QIcon(":/General/Icons/superscript.png"), "Superscript");
    m_subScript = new QAction(QIcon(":/General/Icons/subscript.png"), "Subscript");
    m_bold = new QAction(QIcon(":/General/Icons/bold.png"), "Bold");
    m_italic = new QAction(QIcon(":/General/Icons/italic.png"), "Italic");
    m_underline = new QAction(QIcon(":/General/Icons/underline.png"), "Underline");

    connect(m_superScript, &QAction::triggered, this, &ACompleterTextEdit::makeSuperScript);
    connect(m_subScript, &QAction::triggered, this, &ACompleterTextEdit::makeSubScript);
    connect(m_bold, &QAction::triggered, this, &ACompleterTextEdit::makeBold);
    connect(m_italic, &QAction::triggered, this, &ACompleterTextEdit::makeItalic);
    connect(m_underline, &QAction::triggered, this, &ACompleterTextEdit::makeUnderLine);
}
ACompleterTextEdit::~ACompleterTextEdit()
{

}
void ACompleterTextEdit::setCompleter(QCompleter *completer)
{
    if (c)
        c->disconnect(this);
    c = completer;
    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setFilterMode(Qt::MatchContains);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, QOverload<const QString &>::of(&QCompleter::activated), this, &ACompleterTextEdit::insertCompletion);
}
QCompleter *ACompleterTextEdit::completer() const
{
    return c;
}
void ACompleterTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = QTextEdit::createStandardContextMenu(event->globalPos());
    menu->addAction(m_superScript);
    menu->addAction(m_subScript);
    menu->addAction(m_bold);
    menu->addAction(m_italic);
    menu->addAction(m_underline);
    menu->exec(event->globalPos());
    delete menu;
}
void ACompleterTextEdit::setIndex(QModelIndex index)
{
    m_index = index;
}
QModelIndex ACompleterTextEdit::getIndex() const
{
    return m_index;
}
void ACompleterTextEdit::insertFromMimeData(const QMimeData *source)
{
    insertPlainText ( source->text() );
}
void ACompleterTextEdit::makeSuperScript()
{
    changeFormat(SuperScript);
}
void ACompleterTextEdit::makeSubScript()
{
    changeFormat(SubScript);
}
void ACompleterTextEdit::makeBold()
{
    changeFormat(Bold);
}
void ACompleterTextEdit::makeItalic()
{
    changeFormat(Italic);
}
void ACompleterTextEdit::makeUnderLine()
{
    changeFormat(Underline);
}
void ACompleterTextEdit::changeFormat(int formatChange)
{
    auto cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();

    switch (formatChange) {
    case SubScript :
        if(!(format.verticalAlignment() == QTextCharFormat::AlignSubScript))
            format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
        else
            format.setVerticalAlignment(QTextCharFormat::AlignNormal);
        break;

    case SuperScript :
        if(!(format.verticalAlignment() == QTextCharFormat::AlignSuperScript))
            format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
        else
            format.setVerticalAlignment(QTextCharFormat::AlignNormal);
        break;

    case Bold :
        if(!(format.fontWeight() == 75))
            format.setFontWeight(75);
        else
            format.setFontWeight(50);

        break;

    case Italic :
        if(!(format.fontItalic()))
            format.setFontItalic(true);
        else
            format.setFontItalic(false);
        break;

    case Underline :
        if(!(format.fontUnderline()))
            format.setFontUnderline(true);
        else
            format.setFontUnderline(false);
        break;
    }
    cursor.setCharFormat(format);
    setFocus();
}

void ACompleterTextEdit::insertCompletion(const QString &completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    tc.removeSelectedText();
    tc.insertText(completion);
    setTextCursor(tc);
}
QString ACompleterTextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}
void ACompleterTextEdit::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QTextEdit::focusInEvent(e);
}
void ACompleterTextEdit::keyPressEvent(QKeyEvent *e)
{
    const auto key = e->key();

    if((!c || !c->popup()->isVisible()) ){
        if(e->modifiers() & Qt::ControlModifier){
            switch (key) {
            case Qt::Key_U:
                makeUnderLine();
                break;
            case Qt::Key_I:
                makeItalic();
                break;
            case Qt::Key_B:
                makeBold();
                break;
            }
        }
        else if(key == Qt::Key_F9)
            makeSubScript();
        else if(key == Qt::Key_F10)
            makeSuperScript();
    }
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (key) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    const bool isShortcut = (e->modifiers() == Qt::CTRL && key == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
            e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 1
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}



ASingleLineTextEdit::ASingleLineTextEdit(QWidget *parent):
    ACompleterTextEdit(parent)
{
    setContextMenuPolicy(Qt::NoContextMenu);
}
ASingleLineTextEdit::~ASingleLineTextEdit()
{

}
void ASingleLineTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QTextEdit::contextMenuEvent(event);
}
void ASingleLineTextEdit::keyPressEvent(QKeyEvent *e)
{
    const auto key = e->key();
    const auto modif = e->modifiers();


    if(!c || !c->popup()->isVisible()){
        if(key == Qt::Key_Up || (modif == Qt::SHIFT && (key == Qt::Key_Return || key == Qt::Key_Enter))){
            emit moveCell(Up);
            return;
        }
        else if(key == Qt::Key_Backtab){
            //          emit moveCell(Left);
            return;
        }
        else if(key == Qt::Key_Tab){
            emit moveCell(Right);
            return;
        }
        else if(key == Qt::Key_Down || key == Qt::Key_Return || key == Qt::Key_Enter){
            emit moveCell(Down);
            return;
        }
    }
    ACompleterTextEdit::keyPressEvent(e);
}
void ASingleLineTextEdit::insertFromMimeData(const QMimeData *source)
{
    QTextEdit::insertPlainText(source->text().replace(QRegExp("[\\n\\t\\r]")," "));
}
