#include <QDragEnterEvent>
#include <QMimeData>
#include <QStringListModel>
#include <QMessageBox>
#include <QPushButton>
#include <QGridLayout>
#include <QDir>
#include <QFileIconProvider>
#include <QFileDialog>
#include <QListWidget>
#include <QToolButton>
#include <QAction>
#include <QStringListModel>
#include <QMenu>
#include <QLineEdit>
#include <QDebug>
#include <QLabel>
#include <QDirIterator>

#include <windows.h>
#include <Shellapi.h>

#include "AListWidgetAttachedFiles.h"
#include "ALT.h"


//******************************************************************************************************************
//                                              AListWidgetAttachedFiles
//******************************************************************************************************************

AListWidgetAttachedFiles::AListWidgetAttachedFiles(QWidget *parent):
    QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    //Setting the nagivation buttons
    m_pushButtonPrev = new QPushButton(QIcon(":/General/Icons/chevronLeft.png"), QString(), this);
    m_pushButtonPrev->setDisabled(true);
    connect(m_pushButtonPrev, &QPushButton::clicked, this, &AListWidgetAttachedFiles::goToPrev);

    //Setting the layouts
    m_buttonLayout = new QHBoxLayout;
    m_buttonLayout->addWidget(m_pushButtonPrev);
    m_buttonLayout->addItem(new QSpacerItem(1,1));
    m_buttonLayout->setStretch(1,1);

    m_listLayout = new QHBoxLayout;
    m_listLayout->setSpacing(0);

    m_layout = new QVBoxLayout;
    m_layout->addLayout(m_buttonLayout);
    m_layout->addLayout(m_listLayout);
    setLayout(m_layout);
}
void AListWidgetAttachedFiles::addList(const QString &directory)
{
    AListWidgetAttachedFilesWidget *list = new AListWidgetAttachedFilesWidget(directory, this, this);
    connect(list, &AListWidgetAttachedFilesWidget::delegateDone, this, &AListWidgetAttachedFiles::setFocus);
    m_listLayout->addWidget(list);
    m_listVector << list;
}
void AListWidgetAttachedFiles::closeOpenedDirs(AListWidgetAttachedFilesWidget *source, const QString path)
{
    int index = m_listVector.indexOf(source);
    while(m_listVector.size() > index+1 && m_listVector[index+1]->getDirectoy() == path){
        goToPrev();
    }
}

void AListWidgetAttachedFiles::openNewMNova(QDirIterator &it)
{
    const auto file = m_listVector.last()->addDefaultFile(AListWidgetAttachedFilesWidget::Mnova).replace("/", "\\")/*.toStdWString().c_str()*/;
    qDebug() << file;
    while(it.hasNext()){
//        ShellExecute(NULL, L"open", file, it.next().replace("/", "\\").toStdWString().c_str(), NULL, SW_SHOW);
        qDebug() << "Find a solution to open : \"" << it.next() << "\"  in the created Mnova file";
    }
}
void AListWidgetAttachedFiles::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left){
        goToPrev();
    }
    else
        m_listVector.last()->keyPressEvent(event);
}

void AListWidgetAttachedFiles::goToPrev()
{
    if(m_listVector.size() > 1){
        m_listVector.takeLast()->deleteLater();
        m_listVector.last()->giveFocus();
        setFocus();
    }
    if(m_listVector.size() == 1){
        m_pushButtonPrev->setDisabled(true);
    }
}

void AListWidgetAttachedFiles::setFocus()
{
    QWidget::setFocus();
}
void AListWidgetAttachedFiles::openDir(AListWidgetAttachedFilesWidget *source, const QString path)
{
    while(source != m_listVector.last()){
        m_listVector.takeLast()->deleteLater();
    }
    m_pushButtonPrev->setDisabled(false);
    addList(path);
}


//******************************************************************************************************************
//                                                  AListWidgetWidget
//*****************************************************************************************************************

QBrush AListWidgetAttachedFilesWidget::whiteBrush = QBrush(QColor("#FFFFFF"));
QBrush AListWidgetAttachedFilesWidget::selectedBrush = QBrush(QColor("#33A0FF"));

AListWidgetAttachedFilesWidget::AListWidgetAttachedFilesWidget(const QString &directory , AListWidgetAttachedFiles *attachedFileWidget, QWidget *parent):
    QListWidget(parent),
    m_directory(directory + "/"),
    m_iconProvider(new QFileIconProvider),
    m_attachedFilesWidget(attachedFileWidget)
{
    setFocusPolicy(Qt::NoFocus);
    setAcceptDrops(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::SelectedClicked);
    AListWidgetAttachedFilesDelegate *delegate = new AListWidgetAttachedFilesDelegate(m_directory, this);
    setItemDelegate(delegate);
    connect(delegate, &AListWidgetAttachedFilesDelegate::closeEditor, this, &AListWidgetAttachedFilesWidget::delegateDone);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QListWidget::itemDoubleClicked, this, &AListWidgetAttachedFilesWidget::openSelection);
    connect(this, &QListWidget::customContextMenuRequested, this,  &AListWidgetAttachedFilesWidget::customMenuRequestedList);
    loadDir();

    QList<QAction*> actionListAdd;
    actionListAdd.reserve(6);
    actionListAdd << new QAction(m_iconProvider->icon(QFileIconProvider::Folder), "Folder", this)
                  << new QAction(getIcon(".docx"), "World", this)
                  << new QAction(getIcon(".xlsx"), "Excel", this)
                  << new QAction(getIcon(".pptx"), "Power point", this)
                  << new QAction(getIcon(".cdxml"), "Chemdraw", this)
                  << new QAction(getIcon(".txt"), "Text", this)
                  << new QAction(getIcon(".mnova"), "MNova", this);
    for(int i = 0; i<actionListAdd.size(); i++){
        actionListAdd[i]->setProperty("index", i);
        connect(actionListAdd[i], &QAction::triggered, this, &AListWidgetAttachedFilesWidget::addDefaultFileSlt);
    }
    m_menuAdd= new QMenu("New");
    m_menuAdd->setIcon(QIcon(":/General/Icons/newFile.png"));
    m_menuAdd->addActions(actionListAdd);
    m_menuListWidget = new QMenu(this);
    m_removeAc = new QAction(QIcon(":/General/Icons/removeRow.png"), "Remove");
    m_renameAc = new QAction(QIcon(":/General/Icons/rename.png"), "Rename");
    connect(m_removeAc, SIGNAL(triggered()), this, SLOT(removeFile()));
    connect(m_renameAc, SIGNAL(triggered()), this, SLOT(editCurrentItem()));
    m_menuListWidget->addAction(m_removeAc);
    m_menuListWidget->addAction(m_renameAc);
    m_menuListWidget->addMenu(m_menuAdd);
    setCurrentRow(0);

}
AListWidgetAttachedFilesWidget::~AListWidgetAttachedFilesWidget()
{
    delete m_iconProvider;
}
QString AListWidgetAttachedFilesWidget::getDirectoy() const
{
    return m_directory;
}
void AListWidgetAttachedFilesWidget::loadDir()
{
    int blue = -1;
    QListWidgetItem *item;
    for(int i = 0; count() != 0; i++){
        item = takeItem(0);
        if(item->background() == AListWidgetAttachedFilesWidget::selectedBrush){
            blue = i;
        }
        delete item;
    }
    auto list = QDir(m_directory).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name | QDir::DirsFirst);
    for(const auto &file : qAsConst(list)){
        const QString name = file.fileName();
        item = new QListWidgetItem(getIcon(m_directory + name), name);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        addItem(item);
        if(blue == 0)
            item->setBackground(AListWidgetAttachedFilesWidget::selectedBrush);
        blue--;
    }
}
QIcon AListWidgetAttachedFilesWidget::getIcon(const QString &path)
{
    QFileInfo file(path);
    if(file.isDir())
        return m_iconProvider->icon(QFileIconProvider::Folder);
    else
        return m_iconProvider->icon(file.fileName());
}
void AListWidgetAttachedFilesWidget::resetAllBackground()
{
    for(int i = 0; i < count(); i++){
        item(i)->setBackground(AListWidgetAttachedFilesWidget::whiteBrush);
    }
}
void AListWidgetAttachedFilesWidget::addFile(const QString &path, const QString &newName)
{
    QFileInfo fileInfo(path);
    if(fileInfo.isDir()){
        if(!ALT::copyRecursively(path, m_directory + newName))
            QMessageBox::warning(nullptr, "Error", "An error curred while copying directory \"" + fileInfo.fileName() + "\"");
        loadDir();
    }
    else if(fileInfo.isFile()){
        if(!findItems(newName, Qt::MatchCaseSensitive).isEmpty())
            QMessageBox::information(this, "A problem occured while adding file(s)", "The experiment already contains a file named  \"" + newName + "\" , the file will be skipped");
        else{
            QString newPath = m_directory + newName;
            if(QFile(path).copy(newPath)){
                SetFileAttributesA(newPath.replace("/","\\").toStdString().c_str(), FILE_ATTRIBUTE_NORMAL);
                loadDir();
            }
            else
                QMessageBox::warning(0,"Error", "The file could not be copied");
        }
    }
}
void AListWidgetAttachedFilesWidget::giveFocus()
{
    for(int i = 0; i < count(); i++){
        if(item(i)->background() == AListWidgetAttachedFilesWidget::selectedBrush)
            setCurrentRow(i);
    }
    resetAllBackground();
}


void AListWidgetAttachedFilesWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()){
        event->accept();
    }
    else
        event->ignore();
}
void AListWidgetAttachedFilesWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasUrls()){
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
        event->ignore();
}
void AListWidgetAttachedFilesWidget::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls()){
        event->setDropAction(Qt::CopyAction);
        const auto lst = event->mimeData()->urls();
        for(const auto &i : lst){
            addFile(i.path().remove(0,1), i.fileName());
        }
    }
    else
        event->ignore();
}
Qt::DropActions AListWidgetAttachedFilesWidget::supportedDropActions() const
{
    return Qt::CopyAction;
}

void AListWidgetAttachedFilesWidget::keyPressEvent(QKeyEvent *event)
{
    const auto key =  event->key();
    const auto modif = event->modifiers();
    if(key == Qt::Key_Delete || key == Qt::Key_Backspace){
        removeFile();
    }
    else if(key == Qt::Key_Enter || key == Qt::Key_Return){
        editCurrentItem();
    }
    else if(key == Qt::Key_Right){
        openSelection(currentItem());
    }
    else if(modif == Qt::CTRL && key == Qt::Key_1){
                addDefaultFile(Folder);
    }
    else if(modif == Qt::CTRL && key == Qt::Key_2){
                addDefaultFile(Word);
    }
    else if(modif == Qt::CTRL && key == Qt::Key_3){
                addDefaultFile(Excel);
    }
    else if(modif == Qt::CTRL && key == Qt::Key_4){
                addDefaultFile(PowerPoint);
    }
    else if(modif == Qt::CTRL && key == Qt::Key_5){
                addDefaultFile(ChemDraw);
    }
    else if(modif == Qt::CTRL && key == Qt::Key_6){
                addDefaultFile(Txt);
    }
    else if(modif == Qt::CTRL && key == Qt::Key_7){
                addDefaultFile(Mnova);
    }
    QListWidget::keyPressEvent(event);
}

void AListWidgetAttachedFilesWidget::removeFile()
{
    const auto items = selectedItems();
    for(auto &item : items){
        const QString fileName = item->text();
        const QString filePath = m_directory + fileName;
        const QFileInfo fileInfo(filePath);
        QMessageBox msgBox;
        msgBox.setText("\"" + fileName + "\" is about to be permanently deleted.");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Save){
            QString saveLocation = QFileDialog::getSaveFileName(this, "Save As", "copy_" + fileName, "File (*." + fileInfo.suffix() + ")");
            if(!saveLocation.isEmpty()){
                if(fileInfo.isDir()){
                    if(!ALT::copyRecursively(filePath, saveLocation)){
                        QMessageBox::warning(nullptr, "Saving error", "The directory could not be saved at the selected location");
                        loadDir();
                        return;
                    }
                }
                else{
                    if(!QFile(filePath).copy(saveLocation)){
                        QMessageBox::warning(nullptr, "Saving error", "The file could not be saved at the selected location");
                        loadDir();
                        return;
                    }
                }
                SetFileAttributesA(saveLocation.toStdString().c_str(), FILE_ATTRIBUTE_NORMAL);
                ret = QMessageBox::Discard;
            }
        }
        if(ret == QMessageBox::Discard){
            bool rm;
            if(fileInfo.isDir()){
                rm = QDir(filePath).removeRecursively();
                m_attachedFilesWidget->closeOpenedDirs(this, filePath + "/");
            }
            else
                rm = QFile(filePath).remove();
            if(!rm){
                qDebug() << filePath;
                QMessageBox::warning(0, "Warning", "The file(s) could not be removed");
                loadDir();
                return;
            }
        }
        else if(ret == QMessageBox::Cancel)
            break;
    }
    loadDir();
}
void AListWidgetAttachedFilesWidget::customMenuRequestedList(const QPoint &pos)
{
    if(itemAt(pos)){
        m_removeAc->setDisabled(false);
        m_renameAc->setDisabled(false);
    }
    else{
        m_removeAc->setDisabled(true);
        m_renameAc->setDisabled(true);
    }
    m_menuListWidget->popup(viewport()->mapToGlobal(pos));
}
void AListWidgetAttachedFilesWidget::editCurrentItem()
{
    if(currentItem())
        editItem(currentItem());
}
void AListWidgetAttachedFilesWidget::openSelection(QListWidgetItem *item)
{
    if(!currentItem())
        return;
    QFileInfo file(m_directory + item->text());
    if(file.isFile())
        ShellExecute(NULL, L"open", file.absoluteFilePath().replace("/", "\\").toStdWString().c_str(), NULL, NULL, SW_SHOW);
    else if(file.isDir()){
        m_attachedFilesWidget->openDir(this, file.absoluteFilePath());
        resetAllBackground();
        item->setBackground(AListWidgetAttachedFilesWidget::selectedBrush);
        setCurrentItem(nullptr);
    }
}

void AListWidgetAttachedFilesWidget::addDefaultFileSlt()
{
    addDefaultFile(FileType (sender()->property("index").toInt()));
}


QString AListWidgetAttachedFilesWidget::addDefaultFile(FileType file)
{
    QString newFileName;
    QString defaultFileName = ":/General/Default/";
    int i = 0;
    bool isFile = true;


    switch(file){
    case Folder:
        newFileName = "New Folder(%1)";
        isFile = false;
        break;
    case Word:
        defaultFileName += "defaultWord.docx";
        newFileName = "Word(%1).docx";
        break;
    case Excel: //Excel
        defaultFileName += "defaultExcel.xlsx";
        newFileName = "Excel(%1).xlsx";
        break;
    case PowerPoint: //Power point
        defaultFileName += "defaultPowerPoint.pptx";
        newFileName = "PowerPoint(%1).pptx";
        break;
    case ChemDraw: //Chem Draw
        defaultFileName += "defaultChemDraw.cdxml";
        newFileName = "ChemDraw(%1).cdxml";
        break;
    case Txt: //txt
        defaultFileName += "defaultText.txt";
        newFileName = "Text(%1).txt";
        break;
    case Mnova://Mnova
        defaultFileName += "defaultMNova.mnova";
        newFileName = "MNova(%1).mnova";
        break;
    default:
        return QString();
        break;
    }
    while (findItems(newFileName.arg(i), Qt::MatchCaseSensitive).size() != 0)
        i++;
    newFileName = newFileName.arg(i);
    if(isFile)
        addFile(defaultFileName, newFileName);
    else{
        QDir(m_directory).mkdir(newFileName);
        loadDir();
    }
    return newFileName;
}


//******************************************************************************************************************
//                                          AListWidgetAttachedFilesDelegate
//******************************************************************************************************************

AListWidgetAttachedFilesDelegate::AListWidgetAttachedFilesDelegate(const QString &directory, QObject *parent) :
    QStyledItemDelegate(parent),
    m_directory(directory)
{
}

QWidget* AListWidgetAttachedFilesDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0){
        QLineEdit *editor = new QLineEdit(parent);
        editor->setAutoFillBackground(true);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}
void AListWidgetAttachedFilesDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == 0){
        QLineEdit *lineEditor = qobject_cast<QLineEdit *>(editor);
        if(lineEditor){
            const QString str = index.model()->data(index).toString();
            lineEditor->setText(str);
            QObject src;
            //the lambda function is executed using a queued connection
            connect(&src, &QObject::destroyed, lineEditor, [lineEditor](){
                int lastDotIndex= lineEditor->text().lastIndexOf(".");
                if(lastDotIndex > 0)
                    lineEditor->setSelection(0,lastDotIndex);
            }, Qt::QueuedConnection);

        }
    }
    QStyledItemDelegate::setEditorData(editor, index);
}
void AListWidgetAttachedFilesDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == 0){
        QLineEdit *lineEditor = qobject_cast<QLineEdit *>(editor);
        if(lineEditor){
            QFile(m_directory + model->data(index).toString()).rename(m_directory + lineEditor->text());
            model->setData(index, lineEditor->text());
        }
    }
    QStyledItemDelegate::setModelData(editor, model, index);
}
void AListWidgetAttachedFilesDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
