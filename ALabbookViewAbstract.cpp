#include <QDebug>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDate>
#include <QJsonArray>
#include <QMessageBox>

#include "ALabBookViewAbstract.h"
#include "ui_ALabBookViewAbstract.h"

ALabBookViewAbstract::ALabBookViewAbstract(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ALabBookViewAbstract)
{

}
ALabBookViewAbstract::~ALabBookViewAbstract()
{
    delete ui;
}
