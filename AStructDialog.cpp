
#include <QAxWidget>
#include <QMessageBox>
#include <QTextStream>
#include <QStandardPaths>
#include <QMessageBox>
#include <QString>

#include "AStructDialog.h"
#include "ui_AStructDialog.h"

AStructDialog::AStructDialog(QString const& structureFile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AStructDialog),
    m_structureFile(structureFile)
{
    ui->setupUi(this);
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    if(!m_structureFile.isEmpty()){
        ui->chemDrawCtrl->setProperty("ShowToolsWhenVisible", true);
        ui->chemDrawCtrl->dynamicCall(("Open("+m_structureFile+",False)").toStdString().c_str());
    }
}
AStructDialog::~AStructDialog()
{

    delete ui;
}
void AStructDialog::save()
{
    ui->chemDrawCtrl->dynamicCall(("SaveAs("+m_structureFile+", kCDFormatCDXML)").toStdString().c_str());
    close();
}
QString AStructDialog::execStruct()
{
    exec();
    return m_structureFile;
}
