#include <QInputDialog>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringListModel>
#include <windows.h>
#include <QMessageBox>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include "AExportAsODF.h"

#include "ALabBookViewer.h"
#include "ALabBookViewCreator.h"
#include "ALabBookInfoView.h"
#include "ALabBook.h"
#include "AExp.h"
#include "ALT.h"

//CONSTRUCTORS

ALabBook::ALabBook(QString fichier, QObject *parent):
    QObject(parent),
    m_expListModel(new QStringListModel(this))
{
    m_saveFile.setFileName(fichier);

    while(!fichier.endsWith('/'))
        fichier.chop(1);
    m_folderName = fichier;
    load(fichier);
}
ALabBook::~ALabBook()
{}

//METHODES

QString ALabBook::createNewLabBook()
{
    ALabBookViewCreator creator;
    return creator.execFile();
}
AExp* ALabBook::addNewExp()
{
    //Creates a new AExp, Adds it to expVector and returns it
    try{
        AExp *temp = new AExp(m_expPrefix+"-"+QString::number(m_expNumberBook+1), m_folderName, true, m_analysisList);
        m_expNumberBook++;
        m_loadedAExpVector << temp;
        save();
        return m_loadedAExpVector.last();
    }
    catch (char const* &e) {
        QMessageBox::warning(0,"Error",e);
        return nullptr;
    }
}
AExp* ALabBook::getExp(QString const& expName)
{
    m_loadedAExpVector << new AExp(expName, m_folderName, false);
    return m_loadedAExpVector.last();
}
void ALabBook::updateExpList()
{
    //Update expList and returns a model created from it
    //(called when an experiment is added)
    QStringList expList;
    expList.reserve(m_expNumberBook);
    for (int i = m_expNumberBook; i>0; i--)
    {
        expList.append(m_expPrefix + "-" + QString::number(i));
    }
    m_expListModel->setStringList(expList);
}
QStringListModel* ALabBook::getExpList()
{
    return m_expListModel;
}
int ALabBook::getExpNumberBook() const
{
    return m_expNumberBook;
}
QString ALabBook::getExpPrefix() const
{
    return m_expPrefix;
}
QString ALabBook::getFolderName() const
{
    return m_folderName;
}
QString ALabBook::getFileName() const
{
    return m_saveFile.fileName();
}
ALabBookInfoView* ALabBook::makeInfoWidget(QWidget *parent)
{
    QVector<QString*> list;
    list.reserve(8);
    QString file = m_saveFile.fileName();
    list << &m_name << &m_expPrefix << &m_compagny << &m_signature << &m_description << &m_creationDate << &file;
    ALabBookInfoView *view = new ALabBookInfoView(list, parent);
    connect(view, SIGNAL(dataChanged()), this, SLOT(save()));
    return view;
}
bool ALabBook::save()
{
    if (!m_saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open the lab notebook save file.");
        return false;
    }
    QJsonObject labBookObjectNew;
    labBookObjectNew["name"] = m_name;
    labBookObjectNew["prefix"] = m_expPrefix;
    labBookObjectNew["compagny"] = m_compagny;
    labBookObjectNew["signature"] = m_signature;
    labBookObjectNew["description"] = m_description;
    labBookObjectNew["creationDate"] = m_creationDate;
    labBookObjectNew["expNumberBook"] = m_expNumberBook;
    QJsonArray array;
    for(auto &i : m_analysisList){
        QJsonObject obj;
        obj["name"] = i.first;
        obj["default"] = i.second;
        array << obj;
    }
    labBookObjectNew["analysisList"] = array;

    QJsonDocument saveDoc(labBookObjectNew);
    m_saveFile.write(saveDoc.toJson());
    m_saveFile.close();
    qDebug() << "labBook saved";
    return true;
}
bool ALabBook::load(const QString &fileName)
{
    if(fileName == QString())
        return false;

    if (!m_saveFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the lab notebook save file.");
        return false;
    }
    QByteArray saveData = m_saveFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject labBookObject(loadDoc.object());

    bool failed = false;

    if(labBookObject.contains("name") && labBookObject.value("name").isString())
        m_name = labBookObject.value("name").toString();
    else{
        qWarning("error while loading name");
        failed = true;
    }
    if(labBookObject.contains("prefix") && labBookObject.value("prefix").isString())
        m_expPrefix = labBookObject.value("prefix").toString();
    else{
        qWarning("error while loading prefix");
        failed = true;
    }
    if(labBookObject.contains("compagny") && labBookObject.value("compagny").isString())
        m_compagny = labBookObject.value("compagny").toString();
    else{
        qWarning("error while loading compagny");
        failed = true;
    }
    if(labBookObject.contains("signature") && labBookObject.value("signature").isString())
        m_signature = labBookObject.value("signature").toString();
    else{
        qWarning("error while loading signature");
        failed = true;
    }
    if(labBookObject.contains("description") && labBookObject.value("description").isString())
        m_description = labBookObject.value("description").toString();
    else{
        qWarning("error while loading description");
        failed = true;
    }
    if(labBookObject.contains("creationDate") && labBookObject.value("creationDate").isString())
        m_creationDate = labBookObject.value("creationDate").toString();
    else{
        qWarning("error while loading creationDate");
        failed = true;
    }
    if(labBookObject.contains("expNumberBook") && labBookObject.value("expNumberBook").isDouble())
        m_expNumberBook = labBookObject.value("expNumberBook").toInt();
    else{
        qWarning("error while loading expNumberBook");
        failed = true;
    }

    if(labBookObject.contains("analysisList") && labBookObject.value("analysisList").isArray()){
        auto analysisArray = labBookObject.value("analysisList").toArray();
        for(const auto &i : qAsConst(analysisArray)){
            QJsonObject obj = i.toObject();
            m_analysisList << QPair<QString,bool>(obj.value("name").toString(),obj.value("default").toBool());
        }
    }
    else{
        qWarning("error while loading analysisList");
        failed = true;
    }
    m_saveFile.close();

    if(failed){
        QMessageBox::warning(nullptr, "Warning", "The labBook file data are corrumpted please check the integrity of the file");
        return false;
    }

    return true;
}
bool ALabBook::view()
{
    QStringList analysisList;
    for(const auto i : m_analysisList)
        analysisList << i.first;

    ALabBookViewer *v = new ALabBookViewer(analysisList,m_expNumberBook,m_expPrefix, m_folderName);
    connect(v, &ALabBookViewer::openExpSignal, this, &ALabBook::openExpSignal);
    return true;
}

QVector<QPair<QString,bool>> ALabBook::getAnalysisList()
{
    return m_analysisList;
}
void ALabBook::setAnalysisList(const QVector<QPair<QString,bool>> &analysisList)
{
    m_analysisList = analysisList;
    save();
}

QString ALabBook::copyExp(const QString &sourceName)
{
    const QString sourceFolder = m_folderName + sourceName;
    const QString newName = m_expPrefix + "-" + QString::number(m_expNumberBook+1);
    QString destinationFolder = m_folderName + newName;
    const QString schemeCDXML = destinationFolder + sourceName + "_scheme.cdxml";

    bool y = ALT::copyRecursively(sourceFolder, destinationFolder, true);
    if(!y){
        throw "Copy failed";
    }
    destinationFolder += "/";

    QDir(destinationFolder).mkdir(destinationFolder + "attachedFiles");

    QFile(schemeCDXML).rename(destinationFolder + newName +"_scheme.cdxml");

    const QString schemeGIF = destinationFolder + sourceName + "_scheme.gif";
    QFile(schemeGIF).
            rename(destinationFolder + newName + "_scheme.gif");

    QFile oldJsonFile(destinationFolder + sourceName + "_AExp.json");
    if (!oldJsonFile.open(QIODevice::ReadOnly))
        throw "Opening read copied file failed";
    QByteArray loadedData = oldJsonFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(loadedData));
    QJsonObject labBookObject(loadDoc.object());
    oldJsonFile.remove();

    labBookObject["creationDate"] = QDate::currentDate().toString();
    labBookObject["expName"] = newName;
    labBookObject["notes"] = "Copied from \"" + sourceName + "\"";
    labBookObject["state"] = AExp::Created;

    QJsonArray analysisArray = labBookObject.value("analysis").toArray();
    QJsonArray newAnalysisArray;
    for(const auto &i : qAsConst(analysisArray)){
        auto obj = i.toObject();
        obj["value"] = false;
        newAnalysisArray << obj;
    }
    labBookObject["analysis"] = newAnalysisArray;

    QFile newJsonFile(destinationFolder + newName + "_AExp.json");
    if (!newJsonFile.open(QIODevice::WriteOnly)) {
        throw "Opening write copied file failed";
    }
    QJsonDocument newDoc(labBookObject);
    newJsonFile.write(newDoc.toJson());
    newJsonFile.close();

    m_expNumberBook++;
    save();
    return newName;
}

int ALabBook::rmExp()
{
    const QString lstExpName = m_expPrefix + "-" + QString::number(m_expNumberBook);
    const int ret = m_expNumberBook;
    QMessageBox msgBox;
    msgBox.setText("The expriment \"" + lstExpName + "\" is about to be permanently deleted.");
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int rep = msgBox.exec();
    if(rep == QMessageBox::Ok){

        if(!QDir(m_folderName + lstExpName).removeRecursively()){
            QMessageBox::warning(nullptr, "Deletion error", "The last experiment could not be removed");
            return 0;
        }
        m_expNumberBook--;
        save();
        updateExpList();
        return ret;
    }
    return 0;
}
void ALabBook::exportAsODF()
{
    AExportAsODF exportDial(m_expPrefix, m_expNumberBook, m_folderName, m_compagny, m_signature, m_name);
    exportDial.exec();
}




