#include <QFileDialog>
#include <QCloseEvent>
#include <QObject>
#include <QMessageBox>
#include <QToolBar>
#include <QSettings>
#include <QStringListModel>
#include <QMimeData>
#include <QListWidget>
#include <QDebug>
#include <QMenu>
#include <QAction>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AExpView.h"
#include "ALabBook.h"
#include "ACompoundDatabase.h"
#include "AExp.h"
#include "ALabBookInfoView.h"
#include "AAnalysisManager.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_labBook(0),
    m_database(0)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    connect(ui->addNewExpButton, &QAbstractButton::clicked, this, &MainWindow::addNewExp);

    //Notebook menu actions
    connect(ui->newLabBookAc, &QAction::triggered, this, &MainWindow::newLabBook);
    connect(ui->loadLabBookAc, SIGNAL(triggered()), this, SLOT(loadLabBook()));
    connect(ui->resetDefaultLabBookAc, &QAction::triggered, this, &MainWindow::resetDefaultLabBook);
    connect(ui->actionExport, &QAction::triggered, this, &MainWindow::openExportDialog);

    ui->newLabBookAc->setIcon(QIcon(":/General/Icons/addNewLabBook.png"));
    ui->loadLabBookAc->setIcon(QIcon(":/General/Icons/openLabBook.png"));
    ui->resetDefaultLabBookAc->setIcon(QIcon(":/General/Icons/resetDefaultLabBook.png"));
    ui->viewLabBookAc->setIcon(QIcon(":/General/Icons/viewLabBook.png"));
    ui->actionExport->setIcon(QIcon(":/General/Icons/export.png"));

    //Databse menu actions
    connect(ui->addNewDatabaseAc, &QAction::triggered, this, &MainWindow::newDatabase);
    connect(ui->loadDatabaseAc, SIGNAL(triggered()), this, SLOT(loadDatabase()));
    connect(ui->resetDatabaseAc, &QAction::triggered, this, &MainWindow::resetDefaultDatabase);
    connect(ui->viewLabBookAc, &QAction::triggered, this, &MainWindow::viewLabBook);

    ui->addNewDatabaseAc->setIcon(QIcon(":/General/Icons/addNewDataBase.png"));
    ui->loadDatabaseAc->setIcon(QIcon(":/General/Icons/loadDataBase.png"));
    ui->viewDatabaseAc->setIcon(QIcon(":/General/Icons/viewDataBase.png"));
    ui->resetDatabaseAc->setIcon(QIcon(":/General/Icons/resetDefaultDatabase.png"));

    //Analysis menu action
    connect(ui->manageAnalysisAc, SIGNAL(triggered()), this, SLOT(manageAnalysis()));
    ui->manageAnalysisAc->setIcon(QIcon(":/General/Icons/manageAnalysis.png"));

    QToolBar *toolBar = new QToolBar(this);
    toolBar->addAction(ui->newLabBookAc);
    toolBar->addAction(ui->loadLabBookAc);
    toolBar->addAction(ui->viewLabBookAc);
    toolBar->addAction(ui->actionExport);
    toolBar->addSeparator();
    toolBar->addAction(ui->addNewDatabaseAc);
    toolBar->addAction(ui->loadDatabaseAc);
    toolBar->addAction(ui->viewDatabaseAc);
    toolBar->addSeparator();
    toolBar->addAction(ui->manageAnalysisAc);

    addToolBar(toolBar);

    //Setting drag and drop operation
    setAcceptDrops(true);

    //Setting expList
    m_expSelectionMenu = new QMenu(this);
    ui->expList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->expList, &QWidget::customContextMenuRequested, this, &MainWindow::customMenuRequestedExpList);
    connect(ui->expList, &QAbstractItemView::activated, this, &MainWindow::openSelectedExp);
    ui->expList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->expList->setFocusPolicy(Qt::NoFocus);

    m_copyAc = new QAction("Create a copy" ,this);
    m_expSelectionMenu->addAction(m_copyAc);
    connect(m_copyAc, &QAction::triggered, this, &MainWindow::copyExp);

    m_rmExpAc = new QAction("Remove last experiment" ,this);
    connect(m_rmExpAc, &QAction::triggered, this, &MainWindow::rmLastExp);
    m_expSelectionMenu->addAction(m_rmExpAc);

    //loading the default ALabBook
    disableAcForLabBook(true);
    m_settings = new QSettings("ALTSoft", "ALabBook", this);

    if(!m_settings->value("defaultLabBookPath").toString().isEmpty()){
        QFile tempFile(m_settings->value("defaultLabBookPath").toString());
        if(tempFile.exists()){
            loadLabBook(m_settings->value("defaultLabBookPath").toString());
            ui->resetDefaultLabBookAc->setEnabled(true);
        }
        else{
            QMessageBox::warning(this, "warning", "The default Lab notebook save file could not be found or does not exist");
            resetDefaultLabBook();
        }
    }
    else{
        ui->resetDefaultLabBookAc->setDisabled(true);
    }

    //Open default ACompoundDatabase
    if(!m_settings->value("defaultCompoundDatabasePath").toString().isEmpty()){
        QFile tempFile(m_settings->value("defaultCompoundDatabasePath").toString());
        if(tempFile.exists()){
            loadDatabase(m_settings->value("defaultCompoundDatabasePath").toString());
            ui->resetDatabaseAc->setEnabled(true);
            ui->viewDatabaseAc->setEnabled(true);
            emit enableAddToDatabaseAcSignal(true);
        }
        else{
            QMessageBox::warning(this, "warning", "The default compound database file could not be found or does not exist");
            resetDefaultDatabase();
        }
    }
    else{
        ui->resetDatabaseAc->setDisabled(true);
        ui->viewDatabaseAc->setDisabled(true);
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}

//METHODES
void MainWindow::displayExp(AExp *exp)
{
    //Display the given AExp
    AExpView *expV = new AExpView(exp, m_database);
    m_expViewVector << expV;
    connect(this, &MainWindow::enableAddToDatabaseAcSignal, expV, &AExpView::enableAddToDatabaseAc);
    connect(expV, &AExpView::closeTab, this, &MainWindow::closeTab);

    ui->expTab->addTab(m_expViewVector.last(), exp->getExpName());
    ui->expTab->setCurrentIndex(m_expViewVector.size());
    updateExpViewIndexes();
    expV->updateWidgetsSize();;
}
void MainWindow::updateExpViewIndexes()
{
    for(int i = 0; i<m_expViewVector.size(); i++)
        m_expViewVector[i]->setIndexVec(i);
}

//SLOTS
void MainWindow::addNewExp()
{
    //Creates and opens a new experiment
    if(m_database){
        AExp *temp = m_labBook->addNewExp();
        if(temp){
            displayExp(temp);
            m_labBook->updateExpList();
        }
    }
    else{
        QMessageBox::warning(nullptr, "Warning", "Please open a compound database before creating an experiement");
    }

}
void MainWindow::openSelectedExp(QModelIndex const& index)
{
    //Opens existing experiment from expListModel
    if(m_database){
        const QVariant selectedExp = m_labBook->getExpList()->data(index, Qt::DisplayRole);
        const QString strIndex = selectedExp.toString();
        for(int i = 0; i < m_expViewVector.size(); i++){
            if(m_expViewVector[i]->getLabel() == strIndex){
                ui->expTab->setCurrentIndex(i+1);
                return;
            }
        }
        displayExp(m_labBook->getExp(strIndex));
    }
    else{
        QMessageBox::warning(nullptr, "Warning", "Please open a compound database before opening an experiement");
    }
}
void MainWindow::copyExp()
{
    try{
        const QString selectedExp = m_labBook->getExpList()->data(m_currentIndex, Qt::DisplayRole).toString();
        const QString newName = m_labBook->copyExp(selectedExp);
        displayExp(m_labBook->getExp(newName));
        m_labBook->updateExpList();
    }
    catch (char const* &e) {
        QMessageBox::warning(nullptr, "Copy fail", e);
    }
}

void MainWindow::rmLastExp()
{
    if(m_labBook){
        int closedExp = m_labBook->rmExp();
        for(int i = 0; i < m_expViewVector.size(); i++){
            if(m_expViewVector[i]->getExpNumber() == closedExp){
                closeTab(i);
                return;
            }
        }
    }
}

void MainWindow::openExpFromViewer(int exp)
{
    openSelectedExp(ui->expList->model()->index(exp,0));
}
void MainWindow::closeTab(int tabIndex)
{
    ui->expTab->removeTab(tabIndex+1);
    delete m_expViewVector.takeAt(tabIndex);
    updateExpViewIndexes();
}
void MainWindow::closeAllTabs()
{
    while(!m_expViewVector.isEmpty())
        closeTab(0);
    ui->expTab->widget(0)->deleteLater();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    closeAllTabs();
    event->accept();
}

void MainWindow::newLabBook()
{
    QString labBookPath = ALabBook::createNewLabBook();
    if(!labBookPath.isEmpty())
        loadLabBook(labBookPath);
}
void MainWindow::loadLabBook(QString fileName)
{
    if(fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(this, "Open a lab notebook file", QString(), "Lab notebook save file (*_ALabBook.json)");

    if(fileName.endsWith("_ALabBook.json") ){
        if(m_labBook != nullptr){
            delete m_labBook;
            closeAllTabs();
        }
        m_labBook = new ALabBook(fileName, this);
        if(m_settings->value("defaultLabBookPath").toString().isEmpty()){
            int answer = QMessageBox::question(this, "set default lab notebook","Do you want to set the selected lab notebook as default ?");
            if(answer == QMessageBox::Yes)
                setDefaultLabBook(fileName);
        }
        connect(m_labBook, &ALabBook::openExpSignal, this, &MainWindow::openExpFromViewer);
        ui->expTab->addTab(m_labBook->makeInfoWidget(this), "Lab notebook info");
        m_labBook->updateExpList();
        ui->expList->setModel(m_labBook->getExpList());

        disableAcForLabBook(false);
    }
    else{
        QMessageBox::warning(nullptr, "Warning", "The lab notebook could not be opened");
    }
}
void MainWindow::setDefaultLabBook(QString str)
{
    if(str.isEmpty())
        str = QFileDialog::getOpenFileName(this, "Choose a default lab notebook", QString(), "Lab notebook save file (*.json)");
    if(!str.isEmpty()){
        m_settings->setValue("defaultLabBookPath", str);
        if(m_labBook == nullptr)
            loadLabBook(str);
        ui->resetDefaultLabBookAc->setEnabled(true);
    }
}
void MainWindow::resetDefaultLabBook()
{
    m_settings->remove("defaultLabBookPath");
    ui->resetDefaultLabBookAc->setDisabled(true);
}
void MainWindow::viewLabBook()
{
    m_labBook->view();
}

void MainWindow::openExportDialog()
{
    m_labBook->exportAsODF();
}
void MainWindow::newDatabase()
{
    QString database = ACompoundDatabase::createNewDatabase();
    if(!database.isEmpty())
        loadDatabase(database);
}
void MainWindow::loadDatabase(QString fileName)
{
    if(fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(this, "Open a compound databse file", QString(), "Compound databse file (*_ACompoundDatabase.json)");

    if(fileName.endsWith("_ACompoundDatabase.json") ){
        if(m_database != nullptr)
            delete  m_database;

        m_database = new ACompoundDatabase(fileName, this);

        if(m_settings->value("defaultCompoundDatabasePath").toString().isEmpty()){
            int answer = QMessageBox::question(this, "set default compound database","Do you want to set the selected compound database as default ?");
            if(answer == QMessageBox::Yes)
                setDefaultDatabase(fileName);
        }

        m_database->updateNameList();
        ui->viewDatabaseAc->setEnabled(true);
        connect(ui->viewDatabaseAc, &QAction::triggered, m_database, &ACompoundDatabase::show);
        emit enableAddToDatabaseAcSignal(true);
    }
    else{
        QMessageBox::warning(nullptr, "Warning", "The compound database could not be opened");
    }
}
void MainWindow::setDefaultDatabase(QString str)
{
    if(str.isEmpty())
        str = QFileDialog::getOpenFileName(this, "Choose a default compound database", QString(), "Compound database file (*.json)");
    if(!str.isEmpty()){
        m_settings->setValue("defaultCompoundDatabasePath", str);
        if(m_database == nullptr)
            loadDatabase(str);
        ui->resetDatabaseAc->setEnabled(true);
        ui->viewDatabaseAc->setEnabled(true);
    }
}
void MainWindow::resetDefaultDatabase()
{
    m_settings->remove("defaultCompoundDatabasePath");
    ui->resetDatabaseAc->setDisabled(true);
    ui->viewDatabaseAc->setDisabled(true);
}
void MainWindow::manageAnalysis()
{
    int index = ui->expTab->currentIndex();
    QVector<QPair<QString, bool>> analysisList = m_labBook->getAnalysisList();
    if(index != 0){
        //        QStringList lst;
        QVector<QString> analysisFormExp = m_expViewVector[index-1]->getAnalysisSimple();
        for(auto &i : analysisList)
            i.second = analysisFormExp.contains(i.first);
    }
    AAnalysisManager *manager = new AAnalysisManager(analysisList);
    auto lst = manager->execList();
    manager->deleteLater();
    QStringList strLst;
    QStringList strLstActive;
    for (auto &i : lst){
        strLst << i.first;
        if(i.second)
            strLstActive << i.first;
    }
    if(index == 0)
        m_labBook->setAnalysisList(lst);
    else
        m_expViewVector[index-1]->updateAnalysis(strLstActive);
}
void MainWindow::disableAcForLabBook(bool state)
{
    ui->viewLabBookAc->setDisabled(state);
    ui->addNewExpButton->setDisabled(state);
    ui->manageAnalysisAc->setDisabled(state);
    ui->actionExport->setDisabled(state);
}
void MainWindow::customMenuRequestedExpList(const QPoint &point)
{
    m_currentIndex = ui->expList->indexAt(point);
    if(m_currentIndex.isValid())
        m_expSelectionMenu->popup(ui->expList->viewport()->mapToGlobal(point));
}
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1 && (event->mimeData()->urls()[0].url().endsWith("_ALabBook.json")
                                                                                 || event->mimeData()->urls()[0].url().endsWith("_ACompounDatabase.json"))){
        event->accept();
    }
    else
        event->ignore();
}
void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1 && (event->mimeData()->urls()[0].url().endsWith("_ALabBook.json")
                                                                                 || event->mimeData()->urls()[0].url().endsWith("_ACompounDatabase.json"))){
        event->accept();
    }
    else
        event->ignore();
}
void MainWindow::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1 && event->mimeData()->urls()[0].url().endsWith("_ALabBook.json")){
        event->accept();
        loadLabBook(event->mimeData()->urls()[0].url().remove(0, 8));
    }
    else if(event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1 && event->mimeData()->urls()[0].url().endsWith("_ACompounDatabase.json")){
        event->accept();
        loadDatabase(event->mimeData()->urls()[0].url().remove(0, 8));
    }
    else
        event->ignore();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{

    const auto key = event->key();
    const auto modif = event->modifiers();

    if(modif == Qt::NoModifier || modif == Qt::ALT){
        if(key == Qt::Key_A ||
                key == Qt::Key_Left){
            ui->expTab->setCurrentIndex(ui->expTab->currentIndex()-1);
            ui->expTab->setFocus();
        }
        else if(key == Qt::Key_D ||
                key == Qt::Key_Right){
            ui->expTab->setCurrentIndex(ui->expTab->currentIndex()+1);
            ui->expTab->setFocus();
        }
        else if(key == Qt::Key_W ||
                key == Qt::Key_Up){
            int jump = 1;
            if(modif == Qt::ALT)
                jump = 10;
            int index = ui->expList->currentIndex().row() - jump;
            if(index < 0)
                index = ui->expList->model()->rowCount() -1;
            ui->expList->setCurrentIndex(ui->expList->model()->index(index,0));
            return;
        }
        else if(key == Qt::Key_S ||
                key == Qt::Key_Down){
            int jump = 1;
            if(modif == Qt::ALT)
                jump = 10;
            ui->expList->setCurrentIndex(ui->expList->model()->index(ui->expList->currentIndex().row()+jump,0));
            return;
        }
    }
    if(modif == Qt::ALT && (key == Qt::Key_Q || key == Qt::Key_Backspace)){
        qDebug() << "enter";
        while(!m_expViewVector.isEmpty()){
            m_expViewVector[0]->close();
        }
    }
    else if(modif == Qt::NoModifier){
        if(key == Qt::Key_Return ||
                key == Qt::Key_Enter ||
                key == Qt::Key_E){
            openSelectedExp(ui->expList->currentIndex());
            return;
        }
        else if(key == Qt::Key_Backspace ||
                key == Qt::Key_Q){
            const  int index = ui->expTab->currentIndex()-1;
            if(index >= 0 && index < m_expViewVector.size())
                m_expViewVector[index]->close();
            return;
        }

        else if(key == Qt::Key_Y){
            if(ui->viewLabBookAc->isEnabled())
                viewLabBook();
            return;
        }
        else if(key == Qt::Key_X){
            openExportDialog();
            return;
        }
        else if(key == Qt::Key_C){
            if(ui->viewDatabaseAc->isEnabled())
                m_database->show();
            return;
        }
        else if(key == Qt::Key_V){
            manageAnalysis();
            return;
        }
        else if(key == Qt::Key_Delete){
            m_labBook->rmExp();
            return;
        }
    }


    else
        QWidget::keyPressEvent(event);
}
