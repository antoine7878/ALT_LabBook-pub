#include <windows.h>

#include "ASchemeDialog.h"
#include "ui_ASchemeDialog.h"

ASchemeDialog::ASchemeDialog(QString const& filePath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ASchemeDialog),
    m_saveFilePath(filePath)
{
    ui->setupUi(this);
    ui->chemDraw->setProperty("ShowToolsWhenVisible", false);
    ui->chemDraw->dynamicCall(("Open("+ m_saveFilePath +".cdxml ,false)").toStdString().c_str());
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
}
ASchemeDialog::~ASchemeDialog()
{
    delete ui;
}
void ASchemeDialog::save()
{
     ui->chemDraw->dynamicCall(("SaveAs(" + m_saveFilePath + ".cdxml , kCDFormatCDXML)").toStdString().c_str());
     ui->chemDraw->dynamicCall(("SaveAs(" + m_saveFilePath + ".gif , kCDFormatGIF)").toStdString().c_str());
     close();
}
