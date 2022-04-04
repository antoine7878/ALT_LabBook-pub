#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QStandardItem>
#include <QAxWidget>
#include <QAxObject>
#include <QPair>
#include <QPointer>
#include <windows.h>
#include <QDebug>
#include <QPair>
#include <QMessageBox>

#include "AExp.h"
#include "ALT.h"
#include "AStructDialog.h"
#include "ole2.h"
double AExp::m_interCompoundDistance(15);

AExp::AExp(QString expName, const QString &folderName, bool isNew, const QVector<QPair<QString,bool>> &analysis):
    QObject(0),
    m_expName(expName),
    m_completionState(Created),
    m_completerModel(new QStandardItemModel(this)),
    m_previousComplterModelExpSize(0),
    m_previousComplterModelCondSize(0),
    m_inQuery(false)
{
    if(isNew){
        m_counterPath = 0;
        m_significantNumbers = 2;
        m_tableModel = new AExpTableModel(&m_significantNumbers, this);


        //Extraction of the saveFileName(first used for directory name) form the parent ALabBook
        QString saveFileName = folderName + m_expName;

        m_saveDir.setPath(saveFileName);
        //Creation of the AExp directory
        if(!m_saveDir.exists())
        {
            if(m_saveDir.mkpath(saveFileName + "/attachedFiles"))
            {
                m_creationDate = QDate::currentDate();

                //Creation of the reaction scheme file
                const QString schemePathCDXML = saveFileName + "/" + m_expName + "_scheme.cdxml";
                const QString schemePathGIF = saveFileName + "/" + m_expName + "_scheme.gif";

                QFile(":/General/Default/defaultScheme.cdxml").copy(schemePathCDXML);
                SetFileAttributesA(schemePathCDXML.toStdString().c_str(), FILE_ATTRIBUTE_NORMAL);
                QFile(":/General/Default/defaultScheme.gif").copy(schemePathGIF);
                SetFileAttributesA(schemePathGIF.toStdString().c_str(), FILE_ATTRIBUTE_NORMAL);

                //Setting the saveFileName
                saveFileName += '/' + m_expName + "_AExp.json";
                m_saveFile.setFileName(saveFileName);

                //Setting the default Analysis
                for(auto &i : analysis){
                    if(i.second)
                        m_analysis.append(QPair<QString,bool>(i.first,false));
                }
                save();
            }
            else
                throw "The AExp directory could not be created";
        }
        else
            throw "The AExp folder already exsitits, creation aborted, dataloss risk";
    }
    else{
        QString saveFileName = folderName;
        saveFileName += m_expName;
        m_saveDir.setPath(saveFileName);
        saveFileName += '/' + m_expName + "_AExp.json";
        m_saveFile.setFileName(saveFileName);
        m_tableModel = new AExpTableModel(&m_significantNumbers, this);
        if(!load()){
            return;
        }
    }
    while(!expName.startsWith("-"))
        expName.remove(0,1);
    expName.remove(0,1);
    m_expNumber = expName.toInt();
    constructChemDraws();
    m_completerModelCond = new QStandardItemModel();
    m_completerModelExp = m_tableModel->getCompleterModel();
    connect(m_tableModel, SIGNAL(completerModelChangedExp()), this, SLOT(updateCompleterModelExp()));
    connect(m_tableModel, SIGNAL(dataChanged(const QModelIndex, const QModelIndex, const QVector<int>)), this, SLOT(save()));
    updateCompleterModelExp();
    updateCompleterModelCond();
}
void AExp::setCompleterModelCond(QStandardItemModel *model)
{
    delete m_completerModelCond;
    m_completerModelCond = model;
}
AExp::~AExp()
{
}

int AExp::getExpNumber() const
{
    return m_expNumber;
}
QString AExp::getUserTitle() const
{
    return m_userTitle;
}
QString AExp::getExpName() const
{
    return m_expName;
}
QString AExp::getProcedure() const
{
    return m_procedure;
}
QString AExp::getNotes() const
{
    return m_notes;
}
QString AExp::getReference() const
{
    return m_reference;
}

QDate AExp::getDate() const
{
    return m_creationDate;
}

QString AExp::getDirectory() const
{
    return m_saveDir.path();
}
AExpTableModel* AExp::getModel()
{
    return  m_tableModel;
}
QString AExp::getSchemePathCDXML() const
{
    return m_saveDir.absolutePath() + "/" + m_expName + "_scheme.cdxml";
}
QString AExp::getSchemePathGIF() const
{
    return m_saveDir.absolutePath() + "/" + m_expName + "_scheme.gif";
}
QVector<QPair<QString, bool> > AExp::getAnalysis()
{
    return m_analysis;
}
QHash<QString, QVariant> AExp::getHashDatabase(int row)
{
    auto hash = m_tableModel->getHashDatabase(row);
    hash["struct"] = m_saveDir.absolutePath() + "/" + QString::number(hash["struct"].toInt()) + ".cdxml";
    return hash;
}
QModelIndex AExp::getQModelIndex(const int row, const int col)
{
    return m_tableModel->index(row, col, QModelIndex());
}
QString AExp::getSaveDirectoryPath() const
{
    return m_saveDir.path();
}
QStandardItemModel* AExp::getCompleterModel()
{
    return m_completerModel;
}

QJsonArray AExp::getExpConditions() const
{
    return m_expConditions;
}

AExp::CompletionState AExp::getCompletionState() const
{
    return m_completionState;
}
int AExp::getSignificantNumbers() const
{
    return m_significantNumbers;
}
void AExp::setUserTitle(QString const& str)
{
    m_userTitle = str;
}
void AExp::setProcedure(QString const& str)
{
    m_procedure = str;
}
void AExp::setNotes(QString const& str)
{
    m_notes = str;
}
void AExp::setReference(QString const& str)
{
    m_reference = str;
}
void AExp::setSignificantNumbers(const int n)
{
    m_significantNumbers = n;
    m_tableModel->updateCompleterList();
}

void AExp::setExpConditions(const QJsonArray expConditions)
{
    m_expConditions = expConditions;
}

void AExp::setDate(const QDate &date)
{
    m_creationDate = date;
}

void AExp::setCompletionState(AExp::CompletionState state)
{
    m_completionState = state;
}

void AExp::setAnalysis(const QVector<QPair<QString, bool>> &list)
{
    m_analysis = list;
}

void AExp::updateCompleterModelExp()
{
    m_completerModel->removeRows(0, m_previousComplterModelExpSize);
    m_previousComplterModelExpSize = m_completerModelExp->rowCount();
    while(m_completerModelExp->rowCount() > 0)
        m_completerModel->insertRow(0, m_completerModelExp->takeRow(0));
    emit modelChanged();
}
void AExp::updateCompleterModelCond()
{
    m_completerModel->removeRows(m_previousComplterModelExpSize, m_previousComplterModelCondSize);
    m_previousComplterModelCondSize = m_completerModelCond->rowCount();
    while(m_completerModelCond->rowCount() > 0)
        m_completerModel->appendRow(m_completerModelCond->takeRow(0));
    emit modelChanged();
}

bool AExp::save()
{
    if(m_inQuery)
        return true;
    if (!m_saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open the experiment save file.");
        return false;
    }
    QJsonObject expObject;
    expObject["userTitle"] = m_userTitle;
    expObject["creationDate"] = m_creationDate.toString();
    expObject["expName"] = m_expName;
    expObject["procedure"] = m_procedure;
    expObject["notes"] = m_notes;
    expObject["reference"] = m_reference;
    expObject["counterPath"] = m_counterPath;
    expObject["significantNumbers"] = m_significantNumbers;
    expObject["expTable"] = m_tableModel->toJsonArray();
    expObject["expConditions"] = m_expConditions;
    expObject["xlsx_scale"] = m_tableModel->getXlsxScale();
    expObject["xlsx_yield"] = m_tableModel->getXlsxYield();
    expObject["completionState"] = m_completionState;

    QJsonArray array;
    //    int j = 0;
    for(auto &i : m_analysis){
        QJsonObject obj;
        obj["name"] = i.first;
        obj["value"] = i.second;
        //        obj["pos"] = j++;
        array << obj;
    }
    expObject["analysis"] = array;

    QJsonDocument saveDoc(expObject);
    m_saveFile.write(saveDoc.toJson());
    m_saveFile.close();
    qDebug() << "exp saved";
    return true;
}
bool AExp::load()
{
    if (!m_saveFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open and load the experiment save file.");
        return false;
    }
    QByteArray saveData = m_saveFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject expObject(loadDoc.object());

    bool failed = false;

    if(expObject.contains("userTitle") && expObject.value("userTitle").isString())
        m_userTitle = expObject.value("userTitle").toString();
    else{
        qWarning("error while loading userTitle");
        failed = true;
    }

    if(expObject.contains("creationDate") && expObject.value("creationDate").isString())
        m_creationDate = QDate::fromString(expObject.value("creationDate").toString());
    else{
        qWarning("error while loading creationDate");
        failed = true;
    }

    if(expObject.contains("expName") && expObject.value("expName").isString())
        m_expName = expObject.value("expName").toString();
    else{
        qWarning("error while loading expName");
        failed = true;
    }

    if(expObject.contains("notes") && expObject.value("notes").isString())
        m_notes = expObject.value("notes").toString();
    else{
        qWarning("error while loading notes");
        failed = true;
    }

    if(expObject.contains("procedure") && expObject.value("procedure").isString())
        m_procedure = expObject.value("procedure").toString();
    else{
        qWarning("error while loading procedure");
        failed = true;
    }

    if(expObject.contains("expName") && expObject.value("expName").isString())
        m_expName = expObject.value("expName").toString();
    else{
        qWarning("error while loading expName");
        failed = true;
    }

    if(expObject.contains("reference") && expObject.value("reference").isString())
        m_reference = expObject.value("reference").toString();
    else{
        qWarning("error while loading reference");
        failed = true;
    }

    if(expObject.contains("counterPath") && expObject.value("counterPath").isDouble())
        m_counterPath = expObject.value("counterPath").toInt();
    else{
        qWarning("error while loading counterPath");
        failed = true;
    }

    if(expObject.contains("significantNumbers") && expObject.value("significantNumbers").isDouble())
        m_significantNumbers = expObject.value("significantNumbers").toInt();
    else{
        qWarning("error while loading significantNumbers");
        failed = true;
    }

    if(expObject.contains("expConditions") && expObject.value("expConditions").isArray())
        m_expConditions = expObject.value("expConditions").toArray();
    else{
        qWarning("error while loading the experiment's conditions");
        failed = true;
    }

    if(expObject.contains("expTable") && expObject.value("expTable").isArray())
        m_tableModel->fromJSonArray(expObject.value("expTable").toArray());
    else{
        qWarning("error while loading expTable");
        failed = true;
    }

    if(expObject.contains("analysis") && expObject.value("analysis").isArray()){
        QJsonArray array = expObject.value("analysis").toArray();
        for(const auto &i : qAsConst(array)){
            auto obj = i.toObject();
            m_analysis.append(QPair<QString, bool>(obj.value("name").toString(), obj.value("value").toBool()));
        }
    }
    else{
        qWarning("error while loading analysis");
        failed = true;
    }

    if(expObject.contains("completionState") && expObject.value("completionState").isDouble())
        m_completionState = (AExp::CompletionState) expObject.value("completionState").toInt();
    else{
        qWarning("error while loading completionState");

    }

    if(failed){
        int answer = QMessageBox::question(nullptr, "Error loading exp","An error accured while loading the experiment, do you you want to over right the save file with the patialy charged data ? (Please export the experiment first to prevent any data lose)");
        if(answer == QMessageBox::No)
            return false;
    }

    m_saveFile.close();
    return true;
}
void AExp::insertRow(const int &row, const int &role, const int &position = 2)
{
    //Inserts a row to the model at row
    const QString path = m_saveDir.absolutePath() + "/" + QString::number(m_counterPath) + ".cdxml";
    QFile(":/General/Default/defaultCompound.cdxml").copy(path);
    SetFileAttributesA(path.toStdString().c_str(), FILE_ATTRIBUTE_NORMAL);

    m_tableModel->insertRowS(row, role, m_counterPath, position);
    ++m_counterPath;
}
int AExp::getRowCount()
{
    return m_tableModel->rowCount();
}
void AExp::updateModel(const QModelIndex &index)
{
    m_tableModel->update(index);
}
void AExp::loopRole(const int row)
{
    m_tableModel->loopVHeaderData(row);
}
void AExp::removeRow(int const row)
{
    QFile(m_saveDir.absolutePath() + "/" + QString::number(m_tableModel->getData(row, AExpTableModel::Structure).toInt()) + ".cdxml").remove();
    m_tableModel->removeRowAt(row);
}
void AExp::moveRowUp(int const row)
{
    m_tableModel->moveRowUp(row);
}
void AExp::moveRowDown(int const row)
{
    m_tableModel->moveRowDown(row);
}
void AExp::receiveQuery(const QVariantList &list, int row)
{
    if(!list.isEmpty()){
        m_inQuery = true;
        m_tableModel->receiveQuery(row, list);
        QFile structDB(list[3].toString());
        QFile newFile(m_saveDir.absolutePath() + "/" + QString::number(m_tableModel->getData(row, AExpTableModel::Structure).toInt()) + ".cdxml");

        if(newFile.exists())
            newFile.remove();
        structDB.copy(newFile.fileName());
        SetFileAttributesA(newFile.fileName().toStdString().c_str(), FILE_ATTRIBUTE_NORMAL);
    }
    else
        qWarning("the compound was not found in the database");
    m_inQuery = false;
}

bool AExp::canUpdateHere(const QModelIndex &index)
{
    return m_tableModel->canUpdateHere(index);
}
void AExp::buttonStructClicked(int row)
{
    const QString path = m_saveDir.absolutePath() + "/" + QString::number(m_tableModel->getData(row, AExpTableModel::Structure).toInt()) + ".cdxml";

    QScopedPointer<AStructDialog> win(new AStructDialog(path));
    const QString structureFile = win->execStruct();

    if(!structureFile.isEmpty()){
        QScopedPointer<QAxObject> chemDraw(new QAxObject("{84328ED3-9299-409F-8FCC-6BB1BE585D08}"));
        chemDraw->dynamicCall(("Open("+structureFile+",False)").toStdString().c_str());
        QScopedPointer<QAxObject> objects(chemDraw->querySubObject("Objects()"));
        double mm = objects->property("MolecularWeight").toDouble();
        m_tableModel->setData(m_tableModel->index(row, 1), mm);
    }
}
void AExp::constructChemDraws()
{
    m_schemeChemDraw = new QAxObject("{84328ED3-9299-409F-8FCC-6BB1BE585D08}", this);
    m_chemDrawVbox = new QAxObject("{84328ED3-9299-409F-8FCC-6BB1BE585D08}", this);
    m_chemDrawHbox = new QAxObject("{84328ED3-9299-409F-8FCC-6BB1BE585D08}", this);
    m_tempChemDraw = new QAxObject("{84328ED3-9299-409F-8FCC-6BB1BE585D08}", this);
}

void AExp::updateScheme(const QStringList &conditions)
{
    m_expConditionsScheme = conditions;
    QVector<int> reagentList{};
    QVector<int> productList{};
    QVector<int> topLabelList{};
    QVector<int> bottomLabelList{};
    Position pos;

    for(int row = 0; row < m_tableModel->rowCount(); row++){
        const int pos = m_tableModel->getData(row, AExpTableModel::Position).toInt();
        switch (pos){
        case 1: //Structure reactant
            reagentList << row;
            break;
        case 2: //Structure top
            topLabelList << row;
            break;
        case 3: //Structure product
            productList << row;
            break;
        case 4: //Structure solvent
            bottomLabelList << row;
            break;
        case 5: //Name reactant
            reagentList << row;
            break;
        case 6: //Name top
            topLabelList << row;
            break;
        case 7: //Name product
            productList << row;
            break;
        case 8: //Name solvent
            bottomLabelList << row;
            break;
        }
    }
    double arrowY = makeVScheme(topLabelList, bottomLabelList);

    pos = pasteAtXY(m_schemeChemDraw, 0, 0, false, false);

    double centerY;
    centerY = makeHSchemeBox(reagentList);
    pasteAtXY(m_schemeChemDraw, pos.leftX - m_interCompoundDistance, pos.topY + arrowY - centerY, false, true, true);

    centerY = makeHSchemeBox(productList);
    pasteAtXY(m_schemeChemDraw, pos.rightX + m_interCompoundDistance, pos.topY + arrowY - centerY, true, true, false);

    m_schemeChemDraw->querySubObject("Objects")->dynamicCall("Move(double, double)", 1000, 1000);

    m_schemeChemDraw->dynamicCall(("SaveAs("+ getSchemePathCDXML() +", kCDFormatCDXML)").toStdString().c_str());
    m_schemeChemDraw->dynamicCall(("SaveAs("+ getSchemePathGIF() +", kCDFormatGIF)").toStdString().c_str());

    m_schemeChemDraw->querySubObject("Objects")->dynamicCall("Clear()");
    m_chemDrawVbox->querySubObject("Objects")->dynamicCall("Clear()");
    m_chemDrawHbox->querySubObject("Objects")->dynamicCall("Clear()");
}
double AExp::makeVScheme(const QVector<int> &rowsTop, const QVector<int> &rowsBottom)
{
    Position pos;
    double y (0), maxX(0), lineBreakCorrection(m_interCompoundDistance/5), bottomBox(0);

    //Making the top box
    for(auto i : rowsTop){
        if(m_tableModel->getData(i, AExpTableModel::Position).toInt() == 2)
            pos = addStructAtXY(i, m_chemDrawVbox, 0, y, true, true);
        else if(m_tableModel->getData(i, AExpTableModel::Position).toInt() == 6)
            pos = addLabelAtXY(m_tableModel->getNamePlain(i), m_chemDrawVbox, 0, y, true, true);

        y = pos.bottomY + lineBreakCorrection;

        bottomBox = pos.bottomY;
        pos = addLabelAtXY(makeLabel(i), m_chemDrawVbox, (pos.rightX + m_interCompoundDistance), pos.centerY, true, false);
        if(pos.rightX > maxX)
            maxX = pos.rightX;
    }

    double arrowY = bottomBox + lineBreakCorrection;
    y += 2*lineBreakCorrection;

    //Making the bottom box
    for(auto i : rowsBottom){
        if(m_tableModel->getData(i, AExpTableModel::Position).toInt() == 4)
            pos = addStructAtXY(i, m_chemDrawVbox, 0, y, true, true);
        else if(m_tableModel->getData(i, AExpTableModel::Position).toInt() == 8)
            pos = addLabelAtXY(m_tableModel->getNamePlain(i), m_chemDrawVbox, 0, y, true, true);
        y = pos.bottomY + lineBreakCorrection;

        pos = addLabelAtXY(makeLabel(i), m_chemDrawVbox, (pos.rightX + m_interCompoundDistance), pos.centerY, true, false);
        if(pos.rightX > maxX)
            maxX = pos.rightX;
    }

    //Adding the condition lablels
    for(const auto &i : qAsConst(m_expConditionsScheme)){
        pos = addLabelAtXY(i, m_chemDrawVbox, 0, y, true, true);
        y = pos.bottomY + lineBreakCorrection;
    }

    //Making the arrow
    QScopedPointer<QAxObject> arrow(m_chemDrawVbox->querySubObject("MakeArrow()"));
    QScopedPointer<QAxObject> point(arrow->querySubObject("Start"));

    point->setProperty("X", -20);
    point->setProperty("Y", arrowY);
    arrow->setProperty("End", point->asVariant());
    point->setProperty("X", maxX + 20);
    point->setProperty("Y", arrowY);
    arrow->setProperty("Start", point->asVariant());

    QScopedPointer<QAxObject> obj(m_chemDrawVbox-> querySubObject("Objects"));
    m_copyWidth = obj->property("Width").toDouble();
    m_copyHeight = obj->property("Height").toDouble();
    OleFlushClipboard();
    obj->dynamicCall("Copy()");
    return  arrowY + 0.3;
}
double AExp::makeHSchemeBox(const QVector<int> &rows)
{
    double x(0), lowest(0);
    Position pos;
    QVector<double> middles;
    middles.reserve(rows.size());

    for(int j = 0; j<rows.size(); j++){
        int i = rows[j];
        //Adds a structure a name label
        if(m_tableModel->getData(i, AExpTableModel::Position).toInt() == 1 || m_tableModel->getData(i, AExpTableModel::Position).toInt() == 3)
            pos = addStructAtXY(i, m_chemDrawHbox, x, 0, true, false);
        else if(m_tableModel->getData(i, AExpTableModel::Position).toInt() == 5 || m_tableModel->getData((i), AExpTableModel::Position).toInt() == 7)
            pos = addLabelAtXY(m_tableModel->getNamePlain(i), m_chemDrawHbox, x, 0, true, false);

        //stores the center point of the items added
        middles << pos.centerX;
        x = pos.rightX + m_interCompoundDistance;

        //Stores the lowest point
        if(pos.bottomY > lowest)
            lowest = pos.bottomY;

        //adds the "+"
        if(j < rows.size()-1){
            pos = addLabelAtXY("+", m_chemDrawHbox, x, 0, true, false);
            x = pos.rightX + m_interCompoundDistance;
        }
    }
    //Adds the info labels below compounds
    for(int i = 0; i < rows.size(); i++){
        QString str = makeLabel(rows[i]);
        addLabelAtXY(str, m_chemDrawHbox, middles[i], lowest + m_interCompoundDistance, false, false);
    }

    QScopedPointer<QAxObject> obj(m_chemDrawHbox-> querySubObject("Objects"));
    m_copyWidth = obj->property("Width").toDouble();
    m_copyHeight = obj->property("Height").toDouble();
    OleFlushClipboard();
    obj->dynamicCall("Copy()");
    obj->dynamicCall("Clear()");
    return lowest;
}
Position AExp::addStructAtXY(const int row, QAxObject* chemDraw, double x, double y, bool atRight, bool atBottom)
{
    QTextDocument doc;
    doc.setHtml(m_tableModel->getData(row, AExpTableModel::Name).toString());

    m_tempChemDraw->querySubObject("Objects")->dynamicCall("Clear()");

    m_tempChemDraw->dynamicCall("Open(QString, bool)", m_saveDir.absolutePath() + "/" + QString::number(m_tableModel->getData(row, AExpTableModel::Structure).toInt()) + ".cdxml", false);
    QScopedPointer<QAxObject> obj(m_tempChemDraw->querySubObject("Objects"));
    OleFlushClipboard();
    obj->dynamicCall("Copy()");
    m_copyWidth = obj->property("Width").toDouble();
    m_copyHeight = obj->property("Height").toDouble();

    Position pos = pasteAtXY(chemDraw, x, y, atRight, atBottom);
    return pos;
}
Position AExp::addLabelAtXY(const QString &text, QAxObject* chemDraw, double x, double y, bool atRight, bool atBottom)
{
    m_tempChemDraw->querySubObject("Objects")->dynamicCall("Clear()");
    QScopedPointer<QAxObject> caption(m_tempChemDraw->querySubObject("MakeCaption()"));
    caption->setProperty("Text" ,text);
    QScopedPointer<QAxObject> obj(m_tempChemDraw->querySubObject("Objects"));
    OleFlushClipboard();
    obj->dynamicCall("Copy()");
    m_copyWidth = obj->property("Width").toDouble();
    m_copyHeight = obj->property("Height").toDouble();

    Position pos = pasteAtXY(chemDraw, x, y, atRight, atBottom);
    return pos;
}
Position AExp::pasteAtXY(QAxObject* chemDraw, double x, double y, bool atRight, bool atBottom, bool atLeft)
{

    //Add the object in the clip board in the chemDraw centered at (x,y)
    chemDraw->querySubObject("ActiveDocument")->dynamicCall("Paste()");
    QScopedPointer<QAxObject> obj(chemDraw->querySubObject("Selection")->querySubObject("Objects"));

    double xZ = obj->property("Right").toDouble() - m_copyWidth/2;
    double yZ = obj->property("Bottom").toDouble() - m_copyHeight/2;
    obj->dynamicCall("Move(double, double)",-xZ, -yZ);

    //Correction of the position
    if(atRight)
        x += m_copyWidth/2;
    else if(atLeft)
        x -= m_copyWidth/2;
    if(atBottom)
        y += m_copyHeight/2;

    obj->dynamicCall("Move(double, double)", x, y);;

    Position ret;
    ret.rightX  = obj->property("Right").toDouble();
    ret.leftX = ret.rightX - m_copyWidth;
    ret.centerX = ret.rightX - m_copyWidth/2;
    ret.bottomY = obj->property("Bottom").toDouble();
    ret.topY = ret.bottomY - m_copyHeight;
    ret.centerY = ret.bottomY - m_copyHeight/2;
    OleFlushClipboard();
    return ret;
}
QString AExp::makeLabel(const int row)
{
    QString retStr = QString();
    const QString role = m_tableModel->headerData(row, Qt::Vertical).toString();
    if(role == "R"){
        const double eq = m_tableModel->getData(row, AExpTableModel::Equiv).toDouble();
        //($e equiv)tableModel->getData(row, AExpTableModel::Structure).toDouble()
        if(eq != 0)
            retStr = "(" + QString::number(eq) + " equiv)";
    }
    else if(role == "SM"){
        //$n mmol, $m mg
        retStr = ALT::displayX(m_tableModel->getData(row, AExpTableModel::Mmoles).toDouble(), m_significantNumbers) + " mmol, " + ALT::displayX(m_tableModel->getData(row, AExpTableModel::Mass).toDouble(), m_significantNumbers) + " mg";
    }
    else if(role == "S"){
        //$name ($C M)
        const double m = m_tableModel->getData(row, AExpTableModel::Concentration).toDouble();
        if(m != 0)
            retStr = "(" + ALT::displayX(m,2) + " M)";
    }
    else if (role == "P"){
        //TODO $yield%, &m mg
        retStr = ALT::displayX(m_tableModel->getData(row, AExpTableModel::Purity).toDouble(), 2) + "%";
        const double mass = m_tableModel->getData(row, AExpTableModel::Mass).toDouble();
        if(mass != 0)
            retStr += ", "+ ALT::displayX(mass, m_significantNumbers) + " mg";
    }
    return retStr;
}




