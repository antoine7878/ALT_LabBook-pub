#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QUrl>

#include "ACompoundDatabaseCreator.h"
#include "ui_ACompoundDatabaseCreator.h"

ACompoundDatabaseCreator::ACompoundDatabaseCreator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ACompoundDatabaseCreator)
{
    setWindowTitle("Database creator");
    ui->setupUi(this);
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->pushButtonCreate, SIGNAL(clicked()), this, SLOT(create()));
    connect(ui->lineEditName,         SIGNAL(textChanged(QString)), this, SLOT(updateCreateButton()));
    connect(ui->lineEditSaveLocation, SIGNAL(textChanged(QString)), this, SLOT(updateCreateButton()));
    updateCreateButton();
}
ACompoundDatabaseCreator::~ACompoundDatabaseCreator()
{
    delete ui;
}

void ACompoundDatabaseCreator::create()
{
    QFile file(m_filePath);
    QString folderName = ui->lineEditSaveLocation->text();
    QString structFolderNameCDXML = folderName + "/structures/cdxml";
    QString structFolderNameGIF = folderName + "/structures/gif";
    QDir dir(folderName);

    m_filePath = ui->lineEditSaveLocation->text()+"/"+ui->lineEditName->text()+"_ACompoundDatabase.json";
    file.setFileName(m_filePath);
    if (!file.open(QIODevice::WriteOnly)){
        qWarning("Could not create the compound database file.");
        close();
    }

    if(!dir.mkpath(structFolderNameCDXML)){
        file.remove();
        QMessageBox::warning(nullptr, "Error", "cdxml folder directory could not be created");
        return;
    }

    if(!dir.mkpath(structFolderNameGIF)){
        file.remove();
        dir.rmpath(structFolderNameCDXML);
        QMessageBox::warning(nullptr, "Error", "gif folder directory could not be created");
        return;
    }

    QJsonObject expObject;
    expObject["databaseName"] = ui->lineEditName->text();
    expObject["compoundsArray"] = QJsonArray();
    expObject["compoundNumberDB"] = 0;
    expObject["counterFile"] = 0;
    QJsonDocument saveDoc(expObject);
    file.write(saveDoc.toJson());
    file.close();

    close();
}
void ACompoundDatabaseCreator::on_pushButtonBrowse_clicked()
{
    ui->lineEditSaveLocation->setText(QFileDialog::getExistingDirectory(this, "Choose save directory", "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}
QString ACompoundDatabaseCreator::execFile()
{
    exec();
    return m_filePath;
}
void ACompoundDatabaseCreator::updateCreateButton()
{
    if(ui->lineEditName->text().isEmpty() ||
       ui->lineEditSaveLocation->text().isEmpty())
        ui->pushButtonCreate->setDisabled(true);
    else
        ui->pushButtonCreate->setDisabled(false);
}
