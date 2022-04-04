#include "ALabBookInfoView.h"

ALabBookInfoView::ALabBookInfoView(QVector<QString*> &vec, QWidget *parent):
    ALabBookViewAbstract(parent),
    m_labBookInfos(vec)
{
    //used to view the lab book
    ui->setupUi(this);

    ui->lineEditName->setText(*(m_labBookInfos[0]));
    ui->lineEditPrefix->setText(*(m_labBookInfos[1]));
    ui->lineEditCompagny->setText(*(m_labBookInfos[2]));
    ui->textEditSignature->setText(*(m_labBookInfos[3]));
    ui->textEditDecription->setText(*(m_labBookInfos[4]));
    ui->dateEdit->setDate(QDate::fromString(*(m_labBookInfos[5])));
    ui->lineEditSaveLocation->setText(*(m_labBookInfos[6]));

    //Chgange UI to make a viewer
    ui->labelName->setText("Name");
    ui->labelPrefix->setText("Prefix");
    ui->labelSaveLocation->setText("Save location");

    ui->lineEditPrefix->setReadOnly(true);
    ui->lineEditSaveLocation->setReadOnly(true);
    delete ui->pushButtonBrowse;
    delete ui->pushButtonCancel;
    delete ui->pushButtonCreate;

    connect(ui->lineEditName, SIGNAL(textChanged(QString)), this, SLOT(commit()));
    connect(ui->lineEditPrefix, SIGNAL(textChanged(QString)), this, SLOT(commit()));
    connect(ui->lineEditCompagny, SIGNAL(textChanged(QString)), this, SLOT(commit()));
    connect(ui->textEditSignature, SIGNAL(textChanged()), this, SLOT(commit()));
    connect(ui->textEditDecription, SIGNAL(textChanged()), this, SLOT(commit()));
}
void ALabBookInfoView::commit()
{
    *(m_labBookInfos[0]) = ui->lineEditName->text();
    *(m_labBookInfos[1]) = ui->lineEditPrefix->text();
    *(m_labBookInfos[2]) = ui->lineEditCompagny->text();
    *(m_labBookInfos[3]) = ui->textEditSignature->toPlainText();
    *(m_labBookInfos[4]) = ui->textEditDecription->toPlainText();
    emit dataChanged();
}
