#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTableWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QAxObject>
#include <QDebug>

#include <windows.h>

#include "ACompoundDatabase.h"
#include "ACompoundDatabaseCreator.h"
#include "ACompoundDatabaseView.h"
#include "ALT.h"


ACompoundDatabase::ACompoundDatabase(QString fileName, QObject *parent) :
    QObject(parent)
{
    m_nameList = new QStringList;
    m_saveFile.setFileName(fileName);
    fileName = fileName.left(fileName.lastIndexOf("/")+1);
    m_structFolderNameCDXML = fileName + "structures/cdxml";
    m_structFolderNameGIF= fileName + "structures/gif";

    load();
    updateNameList();
}
ACompoundDatabase::~ACompoundDatabase()
{
}

QString ACompoundDatabase::createNewDatabase()
{
    ACompoundDatabaseCreator creator;
    QString filePath = creator.execFile();
    return filePath;
}
QVariantList ACompoundDatabase::query(const QString &name)
{
    QJsonObject obj;
    QVariantList listRet;

    QString trimName = name.trimmed();
    for(const auto &i : qAsConst(m_compoundsArray)){
        obj = i.toObject();
        if(ALT::toPlainText(obj["name"].toString()) == trimName){
            listRet << obj["name"].toString() << obj["mm"].toDouble() << obj["d"].toDouble() << cdxmlFile(obj["number"].toInt());
            return listRet;
        }
    }
    return QVariantList();
}
QString ACompoundDatabase::getFileName() const
{
    return m_saveFile.fileName();
}
int ACompoundDatabase::contains(QString const& name)
{
    int i = 0;
    for(i = 0; i < m_compoundsArray.size(); i++){
        if(m_compoundsArray[i].toObject()["name"].toString() == name){
            return i;
        }
    }
    return -1;
}
bool ACompoundDatabase::add(const QHash<QString, QVariant> &hash)
{
    const QString name = hash["name"].toString().trimmed();
    QVector<int> toDelete = {};
    int index = contains(name);
    if(index != -1){
        QMessageBox msgBox;
        QTextDocument doc;
        doc.setHtml(name);
        msgBox.setText("Compound \"" + doc.toPlainText() + "\" already exists in the database");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret){
        case QMessageBox::Save:
            toDelete << m_compoundsArray[index].toObject()["number"].toInt();
            m_compoundsArray.removeAt(index);
            break;
        case QMessageBox::Cancel:
            return false;
            break;
        default:
            return false;
            break;
        }
    }
    QFile file(hash["struct"].toString());
    const QString newPathCDXML = m_structFolderNameCDXML + "/" + QString::number(m_compoundNumberDB) + ".cdxml";
    const QString newPathGIF = m_structFolderNameGIF + "/" + QString::number(m_compoundNumberDB) + ".gif";
    file.copy(newPathCDXML);
    SetFileAttributesA(newPathCDXML.toStdString().c_str(), FILE_ATTRIBUTE_NORMAL);
    QAxObject* chemDraw(new QAxObject("{84328ED3-9299-409F-8FCC-6BB1BE585D08}"));
    chemDraw->dynamicCall(("Open("+ newPathCDXML +",True)").toStdString().c_str());
    chemDraw->dynamicCall(("SaveAs("+ newPathGIF+", kCDFormatGIF)").toStdString().c_str());

    QJsonObject compoundObject;
    compoundObject["name"] = name;
    compoundObject["mm"] = hash["mm"].toDouble();
    compoundObject["d"] = hash["d"].toDouble();
    compoundObject["number"] = m_compoundNumberDB;
    m_compoundsArray << compoundObject;
    m_compoundNumberDB++;
    save(toDelete);
    updateNameList();
    chemDraw->deleteLater();
    return true;
}
bool ACompoundDatabase::save(const QVector<int> &index)
{
    if (!m_saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open the compound database file.");
        return false;
    }
    QJsonObject databaseObject;
    databaseObject["databaseName"] = m_databaseName;
    databaseObject["compoundsArray"] = m_compoundsArray;
    databaseObject["compoundNumberDB"] = m_compoundNumberDB;
    QJsonDocument saveDoc(databaseObject);
    m_saveFile.write(saveDoc.toJson());
    m_saveFile.close();

    for(const auto &i : index){
        qDebug() << "Compound database removal at index" << i;
        qDebug() << "gif file removed  :" << QFile(gifFile(i)).remove();
        qDebug() << "cdxml file removed  :" <<QFile(cdxmlFile(i)).remove();
    }
    updateNameList();
    return true;
}
void ACompoundDatabase::load()
{
    if (!m_saveFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the compound database file.");
        return;
    }

    QByteArray saveData = m_saveFile.readAll();
    m_saveFile.close();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject databaseObject(loadDoc.object());

    if(databaseObject.contains("databaseName") && databaseObject["databaseName"].isString())
        m_databaseName = databaseObject["databaseName"].toString();
    else
        qWarning("error while loading labBookName");

    if(databaseObject.contains("compoundsArray") && databaseObject["compoundsArray"].isArray())
        m_compoundsArray = databaseObject["compoundsArray"].toArray();
    else
        qWarning("error while loading compoundsArray");

    if(databaseObject.contains("compoundNumberDB") && databaseObject["compoundNumberDB"].isDouble())
        m_compoundNumberDB = databaseObject["compoundNumberDB"].toDouble();
    else
        qWarning("error while loading compoundNumberDB");
}
void ACompoundDatabase::show()
{
    ACompoundDatabaseView *v = new ACompoundDatabaseView(this);
    Q_UNUSED(v);
}
void ACompoundDatabase::updateNameList()
{
    m_nameList->clear();
    for(auto i : qAsConst(m_compoundsArray))
        m_nameList->append(ALT::toPlainText(i.toObject()["name"].toString()));
}
QString ACompoundDatabase::gifFile(int number) const
{
    return m_structFolderNameGIF + "/" + QString::number(number) + ".gif";
}
QString ACompoundDatabase::cdxmlFile(int number) const
{
    return m_structFolderNameCDXML + "/" + QString::number(number) + ".cdxml";
}
QStringList* ACompoundDatabase::getNameList() const
{
    return m_nameList;
}
QJsonArray *ACompoundDatabase::getCompoundArray()
{
    return &m_compoundsArray;
}

