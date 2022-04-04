#include <QMessageBox>
#include <QTextStream>
#include <QCompleter>
#include <QListView>
#include <QAxWidget>
#include <QMenu>
#include <QStringListModel>
#include <QComboBox>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QCheckBox>
#include <QDebug>
#include <QTextCharFormat>
#include <QMimeData>
#include <QDirIterator>
#include <QDir>

#include <windows.h>

#include "ui_AExpView.h"
#include "AExp.h"
#include "AExpView.h"
#include "AExpTableDelegate.h"
#include "ACompoundDatabase.h"
#include "ASchemeDialog.h"
#include "AStructDialog.h"
#include "ACompleterTextEdit.h"
#include "ALT.h"

AExpView::AExpView(AExp *exp, ACompoundDatabase *databaseArg, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AExpView),
    m_expDis(exp),
    m_database(databaseArg),
    m_completerModel(exp->getCompleterModel())
{
    ui->setupUi(this);
    setAcceptDrops(true);
    //setFocusPolicy(Qt::StrongFocus);

    //Setting significative numbers
    m_significantsNumber = m_expDis->getSignificantNumbers();
    ui->significantNumberSpinBox->setRange(1, 6);
    ui->significantNumberSpinBox->setValue(m_significantsNumber);

    //Setting expTableView
    ui->expTableView->setModel(m_expDis->getModel());
    ui->expTableView->setUpdatesEnabled(true);
    m_expTableDelegate = new AExpTableDelegate(m_significantsNumber, m_database->getNameList() , ui->expTableView);
    ui->expTableView->setItemDelegate(m_expTableDelegate);
    connect(m_expTableDelegate, &AExpTableDelegate::updateStructure, m_expDis, &AExp::buttonStructClicked);
    connect(m_expTableDelegate, &AExpTableDelegate::query, this, &AExpView::query);
    connect(m_expTableDelegate, &AExpTableDelegate::moveCell, this, &AExpView::moveCell);
    connect(ui->significantNumberSpinBox, SIGNAL(valueChanged(int)), this, SLOT(upadteSignificantNumber(int)));
    ui->expTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->expTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->expTableView->horizontalHeader()->setVisible(true);
    ui->expTableView->verticalHeader()->setVisible(true);
    ui->expTableView->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->expTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->expTableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    ui->expTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->expTableView->setSelectionBehavior(QAbstractItemView::SelectItems);

    connect(ui->expTableView->verticalHeader(), &QHeaderView::sectionClicked, this, &AExpView::headerChange);

    for(int row = 0; row < m_expDis->getRowCount(); row++)
        ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(row, 9));

    //Connection of the QPushButton bellow expTableView
    connect(ui->updateSchemeButton, SIGNAL(clicked()), this, SLOT(updateScheme()));

    //Setting the contexte menu in expTableView
    m_contextExpMenu = new QMenu(this);
    connect(ui->expTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequestedExp(QPoint)));

    m_addRowUpAcExp = new QAction("Add &up", this);
    m_contextExpMenu->addAction(m_addRowUpAcExp);
    connect(m_addRowUpAcExp, SIGNAL(triggered()), this, SLOT(insertRowExpTableModelUp()));
    m_addRowUpAcExp->setIcon(QIcon(":/General/Icons/addUp.png"));

    m_addRowDownAcExp = new QAction("&Add down", this);
    m_contextExpMenu->addAction(m_addRowDownAcExp);
    connect(m_addRowDownAcExp, SIGNAL(triggered()), this, SLOT(insertRowExpTableModelDown()));
    m_addRowDownAcExp->setIcon(QIcon(":/General/Icons/addDown.png"));

    m_removeRowAcExp = new QAction("&Delete row", this);
    m_contextExpMenu->addAction(m_removeRowAcExp);
    connect(m_removeRowAcExp, SIGNAL(triggered()), this, SLOT(removeRowExpTableModel()));
    m_removeRowAcExp->setIcon(QIcon(":/General/Icons/removeRow.png"));

    m_contextExpMenu->addSeparator();

    m_moveRowUpAcExp = new QAction("Move up", this);
    m_contextExpMenu->addAction(m_moveRowUpAcExp);
    connect(m_moveRowUpAcExp, SIGNAL(triggered()), this, SLOT(moveRowUpSlot()));
    m_moveRowUpAcExp->setIcon(QIcon(":/General/Icons/moveUp.png"));

    m_moveRowDownAcExp = new QAction("Move down", this);
    m_contextExpMenu->addAction(m_moveRowDownAcExp);
    connect(m_moveRowDownAcExp, SIGNAL(triggered()), this, SLOT(moveRowDownSlot()));
    m_moveRowDownAcExp->setIcon(QIcon(":/General/Icons/moveDown.png"));

    m_contextExpMenu->addSeparator();

    m_addToDatabaseAcExp = new QAction("Add to database", this);
    m_contextExpMenu->addAction(m_addToDatabaseAcExp);
    connect(m_addToDatabaseAcExp, SIGNAL(triggered()), this, SLOT(addToDatabaseSlot()));
    m_addToDatabaseAcExp->setEnabled(m_database != nullptr);
    m_addToDatabaseAcExp->setIcon(QIcon(":/General/Icons/addToDatabase.png"));

    m_contextExpMenu->addSeparator();

    m_updateHere = new QAction("Update from here", this);
    m_contextExpMenu->addAction(m_updateHere);
    connect(m_updateHere, SIGNAL(triggered()), this, SLOT(updateHere()));
    m_updateHere->setIcon(QIcon(":/General/Icons/updateHere.png"));

    if(m_expDis->getRowCount() == 0)
        insertRowExpTableModel(0, 0, 1);

    (m_expDis->getRowCount() <= 1) ? m_removeRowAcExp->setEnabled(false) : m_removeRowAcExp->setEnabled(true);
    (m_expDis->getRowCount() <= 1) ? m_moveRowUpAcExp->setEnabled(false) : m_moveRowUpAcExp->setEnabled(true);
    (m_expDis->getRowCount() <= 1) ? m_moveRowDownAcExp->setEnabled(false) : m_moveRowDownAcExp->setEnabled(true);

    //Setting the condition widget
    ui->conditionWidget->fromJsonArray(m_expDis->getExpConditions());
    m_expDis->setCompleterModelCond(ui->conditionWidget->getCompleterModel());
    connect(ui->conditionWidget, SIGNAL(completerModelChangedCond()), m_expDis, SLOT(updateCompleterModelCond()));
    connect(ui->conditionWidget, SIGNAL(completerModelChangedCond()), this, SLOT(saveExpDis()));

    //Setting general buttons
    ui->titleLabel->setText(m_expDis->getExpName());
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

    //Setting of the reaction scheme
    m_schemePixmap.load(m_expDis->getSchemePathGIF(), ".gif");
    ui->schemeButton->setFlat(true);
    ui->schemeButton->setFocusPolicy(Qt::NoFocus);
    ui->schemeButton->setIcon(m_schemePixmap);
    ui->schemeButton->setIconSize(m_schemePixmap.size());
    connect(ui->schemeButton, SIGNAL(clicked()), this, SLOT(openScheme()));

    //Setting  the redimensioning of the widgets
    connect(m_addRowUpAcExp, SIGNAL(triggered()), this, SLOT(updateWidgetsSize()));
    connect(m_addRowDownAcExp, SIGNAL(triggered()), this, SLOT(updateWidgetsSize()));
    connect(m_removeRowAcExp, SIGNAL(triggered()), this, SLOT(updateWidgetsSize()));
    connect(ui->procedureTextEdit, SIGNAL(textChanged()), this, SLOT(updateWidgetsSize()));
    connect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(updateWidgetsSize()));

    //Setting of the text edits and line edits
    m_entryCompleter = new QCompleter(m_expDis->getCompleterModel(), this);
    ui->lineEditUserTitle->setText(m_expDis->getUserTitle());
    ui->procedureTextEdit->setCompleter(m_entryCompleter);
    ui->procedureTextEdit->setHtml(m_expDis->getProcedure());
    ui->notesTextEdit->setCompleter(m_entryCompleter);
    ui->notesTextEdit->setHtml(m_expDis->getNotes());
    ui->referenceLineEdit->setText(m_expDis->getReference());
    ui->dateEdit->setDate(m_expDis->getDate());

    auto state = m_expDis->getCompletionState();
    switch(state){
    case 6:
        ui->radioButtonCreated->setChecked(true);
        break;
    case 16:
        ui->radioButtonOnGoing->setChecked(true);
        break;
    case 12:
        ui->radioButtonFinished->setChecked(true);
        break;
    case 3:
        ui->radioButtonClosed->setChecked(true);
        break;
    }


    //Setting  update of the AExp
    connect(ui->lineEditUserTitle, SIGNAL(textChanged(QString)), this, SLOT(updateUserTitle()));
    connect(ui->procedureTextEdit, SIGNAL(textChanged()), this, SLOT(updateProcedure()));
    connect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(updateNotes()));
    connect(ui->referenceLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateReference()));
    connect(ui->dateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(updateDate()));
    connect(ui->radioButtonClosed, SIGNAL(clicked()), this, SLOT(updateCompletionState()));
    connect(ui->radioButtonFinished, SIGNAL(clicked()), this, SLOT(updateCompletionState()));
    connect(ui->radioButtonCreated, SIGNAL(clicked()), this, SLOT(updateCompletionState()));
    connect(ui->radioButtonOnGoing, SIGNAL(clicked()), this, SLOT(updateCompletionState()));


    //Setting the attached file widget
    ui->attachedFileWidget->addList( m_expDis->getSaveDirectoryPath() + "/attachedFiles");

    //Setting the AAnalysisWidget
    ui->analysisWidget->load(m_expDis->getAnalysis());
    connect(ui->analysisWidget, SIGNAL(changed(QVector<QPair<QString, bool>>)), m_expDis, SLOT(setAnalysis(QVector<QPair<QString, bool>>)));
    connect(ui->analysisWidget, SIGNAL(changed(QVector<QPair<QString, bool>>)), this, SLOT(saveExpDis()));

    //Setting rich text
    ui->procedureTextEdit->setAcceptRichText(true);
    ui->notesTextEdit->setAcceptRichText(true);

    update();
    saveExpDis();
}
AExpView::~AExpView()
{
    delete ui;
}
void AExpView::insertRowExpTableModel(const int &row, const int &role, const int &position)
{
    m_expDis->insertRow(row, role, position);
    ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(row, 9));

    if(!m_removeRowAcExp->isEnabled() && m_expDis->getRowCount() != 1){
        m_removeRowAcExp->setEnabled(true);
        m_moveRowUpAcExp->setEnabled(true);
        m_moveRowDownAcExp->setEnabled(true);
    }
    saveExpDis();
}
QString AExpView::getLabel() const
{
    return ui->titleLabel->text();
}
void AExpView::setIndexVec(int indexVec)
{
    m_indexVec = indexVec;
}

QVector<QString> AExpView::getAnalysisSimple() const
{
    const QVector<QString> ret = ui->analysisWidget->getAnalysisListSimple();
    return ret;
}


//SLOTS

void AExpView::customMenuRequestedExp(QPoint const& pos)
{
    m_indexSelection = ui->expTableView->indexAt(pos);
    m_expDis->canUpdateHere(m_indexSelection) ? m_updateHere->setEnabled(true) : m_updateHere->setEnabled(false);
    m_indexSelection.row() == 0 ? m_moveRowUpAcExp->setEnabled(false) : m_moveRowUpAcExp->setEnabled(true);
    m_indexSelection.row() == m_expDis->getRowCount()-1 ? m_moveRowDownAcExp->setEnabled(false) : m_moveRowDownAcExp->setEnabled(true);
    m_contextExpMenu->popup(ui->expTableView->viewport()->mapToGlobal(pos));
}
void AExpView::updateWidgetsSize()
{
    double heightExp = 0;
    heightExp += ui->expTableView->horizontalHeader()->height()+2;
    for(int i = 0; i < m_expDis->getRowCount(); i++){
        heightExp += ui->expTableView->rowHeight(i);
        ui->expTableView->setFixedHeight(heightExp);
    }

    ui->procedureTextEdit->setMinimumHeight(ui->procedureTextEdit->document()->size().height()+5);
    ui->notesTextEdit->setMinimumHeight(ui->notesTextEdit->document()->size().height()+5);
}
void AExpView::insertRowExpTableModelUp()
{
    insertRowExpTableModel(m_indexSelection.row());
    for(int i = m_indexSelection.row() + 1; i < m_expDis->getRowCount(); i++){
        ui->expTableView->closePersistentEditor(m_expDis->getQModelIndex(i, 9));
        ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(i, 9));
    }
    saveExpDis();
}
void AExpView::insertRowExpTableModelDown()
{
    insertRowExpTableModel(m_indexSelection.row()+1);
    for(int i = m_indexSelection.row() + 2; i < m_expDis->getRowCount(); i++){
        ui->expTableView->closePersistentEditor(m_expDis->getQModelIndex(i, 9));
        ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(i, 9));
    }
    saveExpDis();
}
void AExpView::removeRowExpTableModel()
{
    m_expDis->removeRow(m_indexSelection.row());
    for(int i = m_indexSelection.row(); i < m_expDis->getRowCount(); i++){
        ui->expTableView->closePersistentEditor(m_expDis->getQModelIndex(i, 9));
        ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(i, 9));
    }
    if(m_expDis->getRowCount() == 1)
    {
        m_removeRowAcExp->setEnabled(false);
        m_moveRowUpAcExp->setEnabled(false);
        m_moveRowDownAcExp->setEnabled(false);
    }
    saveExpDis();
}
void AExpView::moveRowUpSlot()
{
    ui->expTableView->closePersistentEditor(m_expDis->getQModelIndex(m_indexSelection.row(), 9));
    ui->expTableView->closePersistentEditor(m_expDis->getQModelIndex(m_indexSelection.row() - 1, 9));
    m_expDis->moveRowUp(m_indexSelection.row());
    ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(m_indexSelection.row(), 9));
    ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(m_indexSelection.row() - 1, 9));
    saveExpDis();
}
void AExpView::moveRowDownSlot()
{
    ui->expTableView->closePersistentEditor(m_expDis->getQModelIndex(m_indexSelection.row(), 9));
    ui->expTableView->closePersistentEditor(m_expDis->getQModelIndex(m_indexSelection.row() + 1, 9));
    m_expDis->moveRowDown(m_indexSelection.row());
    ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(m_indexSelection.row(), 9));
    ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(m_indexSelection.row() + 1, 9));
    ui->expTableView->setCurrentIndex(QModelIndex());
    saveExpDis();
}
void AExpView::headerChange(int row)
{
    m_expDis->loopRole(row);
    saveExpDis();
}
void AExpView::upadteSignificantNumber(int n)
{
    m_expTableDelegate->updateSignificantNumber(n);
    m_expDis->setSignificantNumbers(n);
    ui->expTableView->update();
    ui->expTableView->setFocus();
    this->setFocus();
    saveExpDis();
}
void AExpView::updateAnalysis(const QStringList &lst)
{
    ui->analysisWidget->updateAnalysis(lst);
    saveExpDis();
}

void AExpView::updateCompletionState()
{
    AExp::CompletionState state;
    if(ui->radioButtonCreated->isChecked())
        state = AExp::Created;
    else if(ui->radioButtonOnGoing->isChecked())
        state = AExp::OnGoing;
    else if(ui->radioButtonFinished->isChecked())
        state = AExp::Finished;
    else
        state = AExp::Closed;

    m_expDis->setCompletionState(state);
    saveExpDis();
}
void AExpView::updateHere()
{
    m_expDis->updateModel(m_indexSelection);
    ui->expTableView->setFocus();
    saveExpDis();
}

void AExpView::moveCell(ASingleLineTextEdit::MoveCell move)
{
    qDebug() << "test";
    QModelIndex index = ui->expTableView->currentIndex();
    const int row = index.row();
    const int col = index.column();
    switch(move){
    case ASingleLineTextEdit::Up:
        index = ui->expTableView->model()->index(row - 1, col);
        break;
    case ASingleLineTextEdit::Right:
        index = ui->expTableView->model()->index(row, col + 1);
        break;
    case ASingleLineTextEdit::Down:
        index = ui->expTableView->model()->index(row + 1, col);
        break;
    case ASingleLineTextEdit::Left:
        index = ui->expTableView->model()->index(row, col - 1);
        break;
    }
    ui->expTableView->setCurrentIndex(index);
}

void AExpView::updateUserTitle()
{
    m_expDis->setUserTitle(ui->lineEditUserTitle->text());
    saveExpDis();

}
void AExpView::updateProcedure()
{
    m_expDis->setProcedure(ui->procedureTextEdit->toHtml());
    saveExpDis();
}
void AExpView::updateNotes()
{
    m_expDis->setNotes(ui->notesTextEdit->toHtml());
    saveExpDis();
}
void AExpView::updateReference()
{
    m_expDis->setReference(ui->referenceLineEdit->text());
    saveExpDis();
}

void AExpView::updateDate()
{
    m_expDis->setDate(ui->dateEdit->date());
}
void AExpView::openScheme()
{
    std::unique_ptr<ASchemeDialog> schemDial(new ASchemeDialog(m_expDis->getSchemePathCDXML().chopped(6)));
    schemDial->exec();
    m_schemePixmap.load(m_expDis->getSchemePathGIF(), ".gif");
    ui->schemeButton->setIcon(m_schemePixmap);
    ui->schemeButton->setIconSize(m_schemePixmap.size());
}

void AExpView::saveExpDis()
{
    m_expDis->setExpConditions(ui->conditionWidget->toJsonArray());
    if(!m_expDis->save())
        QMessageBox::warning(this, "Warning", "The experiment was not saved");
}
void AExpView::addToDatabaseSlot()
{
    m_database->add(m_expDis->getHashDatabase(m_indexSelection.row()));
}
void AExpView::enableAddToDatabaseAc(bool const& enable)
{
    m_addToDatabaseAcExp->setEnabled(enable);
}
void AExpView::setDatabase(ACompoundDatabase *newDatabase)
{
    m_database = newDatabase;
}

int AExpView::getExpNumber() const
{
    return m_expDis->getExpNumber();
}
void AExpView::query(QString const& name, int row)
{
    QVariantList list = m_database->query(name);
    m_expDis->receiveQuery(list, row);
    ui->expTableView->closePersistentEditor(m_expDis->getQModelIndex(row, 9));
    ui->expTableView->openPersistentEditor(m_expDis->getQModelIndex(row, 9));
    saveExpDis();
    ui->expTableView->setCurrentIndex(QModelIndex());
    saveExpDis();
}
void AExpView::updateScheme()
{
    m_expDis->updateScheme(ui->conditionWidget->getSchemeLines());
    m_schemePixmap.load(m_expDis->getSchemePathGIF(), ".gif");
    ui->schemeButton->setIcon(m_schemePixmap);
    ui->schemeButton->setIconSize(m_schemePixmap.size());
}
void AExpView::updateCompleterModel()
{
    m_entryCompleter->setModel(m_completerModel);
}
void AExpView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls() && event->mimeData()->urls().first().url().endsWith("")){
        event->accept();
    }
    else
        QWidget::dragEnterEvent(event);
}
void AExpView::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1 && event->mimeData()->urls().first().url().endsWith("")){
        event->accept();
    }
    else
        QWidget::dragMoveEvent(event);
}
void AExpView::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    if(mime->hasUrls()){
        const QList<QUrl> urls = mime->urls();
        if (urls.size() == 1){
            event->accept();
            QDirIterator dirIt(urls.first().path().remove(0,1), QStringList() << "fid", QDir::NoFilter, QDirIterator::Subdirectories );
            if(dirIt.hasNext()){
                ui->attachedFileWidget->openNewMNova(dirIt);
            }
        }
    }
    else
        QWidget::dropEvent(event);
}
void AExpView::keyPressEvent(QKeyEvent *event)
{
    if((event->key() == Qt::Key_1)){
        qDebug() << "AExpView";
    }
    if((event->key() == Qt::Key_2)){
        qDebug() << "2";
    }
    if((event->key() == Qt::Key_3)){
        qDebug() << "3";
    }
    if((event->key() == Qt::Key_4)){
        qDebug() << "4";
    }
    if((event->key() == Qt::Key_5)){
        qDebug() << "5";
    }
    if((event->key() == Qt::Key_6)){
        qDebug() << "6";
    }
    else{
        QWidget::keyPressEvent(event);
    }
}
void AExpView::closeEvent(QCloseEvent *event)
{
    emit closeTab(m_indexVec);
    event->accept();
}
void AExpView::focusOutEvent(QFocusEvent *event)
{
    releaseKeyboard();
    QWidget::focusOutEvent(event);
}
