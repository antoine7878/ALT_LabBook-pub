#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QDir>
#include <QListWidget>
#include <QDate>
#include <QJsonArray>
#include <QProgressDialog>

#include <windows.h>
#include <Shellapi.h>
#include <algorithm>

#include "AExportAsODF.h"
#include "ui_AExportAsODF.h"
#include "ALT.h"

AExportAsODF::AExportAsODF(const QString &prefix, const int &expNumber, const QString &folderName, const QString &compagny, const QString &signature, const QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AExportAsODF),
    m_expNumber(expNumber),
    m_expPrefix(prefix),
    m_folderName(folderName),
    m_compagny(compagny),
    m_signature(signature),
    m_name(name)
{
    ui->setupUi(this);
    ui->radioExportAll->setChecked(true);
    ui->spinBoxFirst->setRange(1,expNumber);
    ui->spinBoxFirst->setEnabled(false);
    ui->spinBoxLast->setRange(1,expNumber);
    ui->spinBoxLast->setEnabled(false);
    setWindowTitle("Experiment exporter");

    connect(ui->radioExportRange, SIGNAL(toggled(bool)), this, SLOT(toggleRangeEditors(bool)));
    connect(ui->spinBoxFirst, SIGNAL(valueChanged(int)), this, SLOT(changeLastValue(int)));
    connect(ui->spinBoxLast, SIGNAL(valueChanged(int)), this, SLOT(changeFirstValue(int)));
}
AExportAsODF::~AExportAsODF()
{
    delete ui;
}
void AExportAsODF::keyPressEvent(QKeyEvent *event)
{
    const auto key = event->key();
    if(event->modifiers() == Qt::NoModifier && (key == Qt::Key_Q || key == Qt::Key_X)){
        close();
    }
    else
        QWidget::keyPressEvent(event);
}
void AExportAsODF::on_buttonExport_clicked()
{
    QStringList failed;
    QPair<int,int> range;
    if(ui->radioExportAll->isChecked()){
        range.first = 1;
        range.second = m_expNumber;
    }
    else{
        range.first = ui->spinBoxFirst->value();
        range.second = ui->spinBoxLast->value();
    }

    QString saveFolder = QFileDialog::getExistingDirectory(nullptr, "Choose save directory", "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(saveFolder == QString())
        return;

    const QString font = "MS Shell dlg 2";

    //Title A format
    QTextBlockFormat blockFormatTitleA;
    QTextCharFormat charFomatTitleA;
    blockFormatTitleA.setAlignment(Qt::AlignCenter);
    charFomatTitleA.setFont(QFont(font, 15, 75));

    //Title B Format
    QTextBlockFormat blockFormatTitleB;
    QTextCharFormat charFomatTitleB;
    blockFormatTitleB.setAlignment(Qt::AlignLeft);
    QFont fontTitleB(font, 13, 75);
    fontTitleB.setUnderline(true);
    charFomatTitleB.setFont(fontTitleB);

    //Normal format
    QTextBlockFormat blockFormatNormal;
    QTextCharFormat charFormatNormal;
    blockFormatNormal.setAlignment(Qt::AlignJustify);
    QFont fontNormal(font, 11);
    charFormatNormal.setFont(fontNormal);

    //Header format
    QTextBlockFormat blockFormatHeader;
    QTextCharFormat charFormatHeader;
    blockFormatHeader.setAlignment(Qt::AlignCenter);
    charFormatHeader.setFont(QFont(font, 11, 75));

    //NormalCentered format
    QTextBlockFormat blockFormatNormalCentred;
    QTextCharFormat charFormatNormalCentered;
    blockFormatNormalCentred.setAlignment(Qt::AlignHCenter);
    charFormatNormalCentered.setFont(QFont(font, 11));

    //Table format
    QTextTableFormat tableformat;
    tableformat.setCellPadding(5);
    tableformat.setHeaderRowCount(1);
    tableformat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableformat.clearColumnWidthConstraints();
    tableformat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
    tableformat.setAlignment(Qt::AlignHCenter);


    QProgressDialog progDial;
    int prog = 0;
    progDial.setRange(0, range.second - range.first + 1);
    QLabel *lab = new QLabel("Export in progress");
    lab->setFont(QFont(font,16));
    progDial.setLabel(lab);
    progDial.open();

    for(int i = range.first; i <= range.second; i++){
        const QString expName = m_expPrefix + "-" + QString::number(i);
        const QString expFolder = m_folderName + expName;
        const QString schemeFile = expFolder + "/" + expName + "_scheme.gif";
        QFile json(expFolder + "/" + expName + "_AExp.json");

        const QString exportExpFolder = saveFolder + "/" + expName;

        if(QDir(exportExpFolder).exists() || !json.open(QIODevice::ReadOnly) || !QDir().mkdir(exportExpFolder)){
            failed << m_expPrefix + "-" + QString::number(i);
            break;
        }

        QByteArray loadedData = json.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(loadedData));
        QJsonObject expObject(loadDoc.object());

        QTextDocument *doc = new QTextDocument;
        QTextCursor cur(doc);

        //Title
        const QString date = QDate::fromString(expObject["creationDate"].toString()).toString("dd/MM/yyyy");
        cur.setCharFormat(charFomatTitleA);
        cur.setBlockFormat(blockFormatTitleA);
        cur.insertText(m_compagny + " - " + m_name + "\n" + date + "\n" + expName);

        //User Title
        const QString userTitle = expObject["userTitle"].toString();
        if(!userTitle.isEmpty()){
            cur.insertBlock(blockFormatTitleB, charFomatTitleB);
            cur.insertText("Title:");
            cur.setCharFormat(charFormatNormal);
            cur.insertText(" " + userTitle);
        }

        //Reaction scheme
        cur.insertBlock(blockFormatTitleB, charFomatTitleB);
        cur.insertText("Reaction scheme:");
        cur.insertBlock(blockFormatNormalCentred, charFormatNormalCentered);
        cur.insertImage(QImage(schemeFile));

        //Compound table
        QJsonArray expTableArray = expObject["expTable"].toArray();
        if(expTableArray.size() > 0){
            cur.insertBlock(blockFormatTitleB, charFomatTitleB);
            cur.insertText("Compound table:");

            cur.insertTable(expTableArray.size() + 1, 9, tableformat);

            QStringList headerExpTable;
            headerExpTable << "Name" << "M [g/mol]" << "d" << "%" << "equiv" << "n [mmol]" << "C [mol/L]" << "m [mg]" << "V [ml]";
            for(auto &i : headerExpTable){
                cur.setCharFormat(charFormatHeader);
                cur.setBlockFormat(blockFormatHeader);
                cur.insertText(i);
                cur.movePosition(QTextCursor::NextCell);
            }

            const int significantNumber = expObject["significantNumbers"].toInt();
            for(const auto &i : qAsConst(expTableArray)){
                QJsonObject line = i.toObject();
                cur.insertHtml(line["name"].toString());
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(ALT::displayX(line["molarMass"].toDouble(),significantNumber));
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(ALT::displayX(line["density"].toDouble(),significantNumber));
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(ALT::displayX(line["purity"].toDouble(), 2));
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(ALT::displayX(line["equiv"].toDouble(), significantNumber));
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(ALT::displayX(line["mmoles"].toDouble(), significantNumber));
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(ALT::displayX(line["concentration"].toDouble(), significantNumber));
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(ALT::displayX(line["mass"].toDouble(), significantNumber));
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(ALT::displayX(line["volume"].toDouble(), significantNumber));
                cur.movePosition(QTextCursor::NextCell);
            }
            cur.movePosition(QTextCursor::NextBlock);
        }

        //Conditions
        QJsonArray conditionsTableArray = expObject["expConditions"].toArray();
        if(conditionsTableArray.size() > 0){
            cur.insertBlock(blockFormatTitleB, charFomatTitleB);
            cur.insertText("Conditions:");

            cur.insertTable(conditionsTableArray.size() + 1, 2, tableformat);
            QStringList headerConditionsTable;
            headerConditionsTable << "Temperature [°C]" << "Duration";
            for(auto &i : headerConditionsTable){
                cur.setCharFormat(charFormatHeader);
                cur.setBlockFormat(blockFormatHeader);
                cur.insertText(i);
                cur.movePosition(QTextCursor::NextCell);
            }

            for(const auto &i : qAsConst(conditionsTableArray)){
                QJsonObject line = i.toObject();
                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                cur.insertText(QString::number(line["temperature"].toDouble()));
                cur.movePosition(QTextCursor::NextCell);

                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                const QString duration = QString::number(line["hour"].toInt()) + " h " + QString::number(line["min"].toInt()) + " min";
                cur.insertText(duration);
                cur.movePosition(QTextCursor::NextCell);
            }
            cur.movePosition(QTextCursor::NextBlock);
        }

        //Procedure
        const QString procedure = expObject["procedure"].toString();
        if(!procedure.isEmpty()){
            cur.insertBlock(blockFormatTitleB, charFomatTitleB);
            cur.insertText("Procedure:");
            cur.insertBlock(blockFormatNormal, charFormatNormal);
            cur.insertHtml(procedure);
        }

        //Analysis
        const QJsonArray analysis = expObject["analysis"].toArray();
        if(!analysis.isEmpty()){
            cur.insertBlock(blockFormatTitleB, charFomatTitleB);
            cur.insertText("Analysis:\n");
            for(const auto &i: analysis){
                QJsonObject line = i.toObject();
                cur.setCharFormat(charFormatNormal);
                cur.setBlockFormat(blockFormatNormal);
                QString state(u8"\u2717");
                if(line["value"].toBool())
                    state = QString(u8"\u2713");
                cur.insertText(line["name"].toString() + ": " + state + "\n");

            }
        }


        //Notes
        const QString notes = expObject["notes"].toString();
        if(!notes.isEmpty()){
            cur.insertBlock(blockFormatTitleB, charFomatTitleB);
            cur.insertText("Notes:");
            cur.insertBlock(blockFormatNormal, charFormatNormal);
            cur.insertHtml(notes);
        }

        //Reference
        const QString references = expObject["reference"].toString();
        if(!references.isEmpty()){
            cur.insertBlock(blockFormatTitleB, charFomatTitleB);
            cur.insertText("Reference:");
            cur.insertBlock(blockFormatNormal, charFormatNormal);
            cur.insertText(references);
        }

        //Creating the file
        QTextDocumentWriter writer(exportExpFolder + "/" + expName + ".odt");
        writer.write(doc);
        doc ->deleteLater();

        //Copy of the attached files
        const QString attachedFilesFolder = m_folderName + expName + "/attachedFiles";
        const QStringList filesList = QDir(attachedFilesFolder).entryList();
        for(auto &i : filesList){
            QFile(attachedFilesFolder + "/" + i).copy(exportExpFolder + "/" + i);
        }
        prog++;
        progDial.setValue(prog);
    }

    if(failed.size() > 0){
        QDialog *dialFailed = new QDialog;
        QLabel *label = new QLabel("The following experiements could not be exported:",dialFailed);
        auto font = label->font();
        font.setPointSize(16);
        label->setFont(font);
        QListWidget *listWidget = new QListWidget(dialFailed);
        listWidget->addItems(failed);
        QVBoxLayout *layout = new QVBoxLayout(dialFailed);
        layout->addWidget(label);
        layout->addWidget(listWidget);
        dialFailed->exec();
        dialFailed->deleteLater();
    }
    close();
}
void AExportAsODF::changeLastValue(int i)
{
    if(i > ui->spinBoxLast->value())
        ui->spinBoxLast->setValue(i);
}
void AExportAsODF::changeFirstValue(int i)
{
    if(i < ui->spinBoxFirst->value())
        ui->spinBoxFirst->setValue(i);
}
void AExportAsODF::toggleRangeEditors(bool state)
{
    ui->spinBoxLast->setEnabled(state);
    ui->spinBoxFirst->setEnabled(state);
}

