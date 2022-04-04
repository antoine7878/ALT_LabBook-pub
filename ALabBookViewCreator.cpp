#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>

#include "ALabBookViewCreator.h"

ALabBookViewCreator::ALabBookViewCreator(QWidget *parent):
    ALabBookViewAbstract(parent)
{
    ui->setupUi(this);
    setWindowTitle("Lab notebook creator");
    ui->dateEdit->setDate(QDate::currentDate());
    connect(ui->pushButtonCreate, SIGNAL(clicked()), this, SLOT(create()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->pushButtonBrowse, SIGNAL(clicked()), this, SLOT(browseSlot()));
    connectUpdates();
    updateCreateButton();
}

void ALabBookViewCreator::create()
{
    m_filePath = ui->lineEditSaveLocation->text() + "/" + ui->lineEditPrefix->text() + "_ALabBook.json";
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly)){
        qWarning("Could not create the lab notebook file.");
        close();
    }

    QJsonObject expObject;
    expObject["name"] = ui->lineEditName->text();
    expObject["prefix"] = ui->lineEditPrefix->text();
    expObject["compagny"] = ui->lineEditCompagny->text();
    expObject["signature"] = ui->textEditSignature->toPlainText();
    expObject["description"] = ui->textEditDecription->toPlainText();
    expObject["creationDate"] = QDate::currentDate().toString();
    expObject["expNumberBook"] = 0;

    QJsonArray array;
    QJsonObject obj;
    obj["default"] = false;
    QStringList names = {"H-NMR", "C-NMR",  "F-NMR", "HRMS", "EA", "IR", "m.p."};
    for(auto &i : names){
        obj["name"] = i;
        array << obj;
    }
    expObject["analysisList"] = array;

    QJsonDocument saveDoc(expObject);
    file.write(saveDoc.toJson());
    file.close();
    close();
}
void ALabBookViewCreator::browseSlot()
{
    const auto file = QFileDialog::getExistingDirectory(this, "Choose save directory", "/home", QFileDialog::ShowDirsOnly);
    ui->lineEditSaveLocation->setText(file);
}

QString ALabBookViewCreator::execFile()
{
    exec();
    return m_filePath;
}
void ALabBookViewCreator::updateCreateButton()
{
    if(ui->lineEditName->text().isEmpty() ||
            ui->lineEditPrefix->text().isEmpty() ||
            ui->lineEditSaveLocation->text().isEmpty())
        ui->pushButtonCreate->setDisabled(true);
    else
        ui->pushButtonCreate->setDisabled(false);
}
void ALabBookViewCreator::connectUpdates()
{
    connect(ui->lineEditName,         SIGNAL(textChanged(QString)), this, SLOT(updateCreateButton()));
    connect(ui->lineEditPrefix,       SIGNAL(textChanged(QString)), this, SLOT(updateCreateButton()));
    connect(ui->lineEditSaveLocation, SIGNAL(textChanged(QString)), this, SLOT(updateCreateButton()));
}
