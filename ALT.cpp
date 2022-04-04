#include <math.h>
#include <QMessageBox>
#include <QTableView>
#include <QHBoxLayout>
#include <QDir>
#include <QFile>
#include <QInputMethodEvent>
#include <QCoreApplication>
#include <QTextLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QDebug>

#include <windows.h>

#include "ALT.h"

QString ALT::displayX(double x, int s)
{
// returns QString containing x with s significant numbers
    if(x == 0)
        return "0";
    bool a(false);
    int n(0);

    int lgs = std::ceil(std::log10(x))-s;
    x /= std::pow(10, lgs);
    x = std::round(x);
    x *= std::pow(10, lgs);
    QString ret(QString::number(x));

    for(auto &i : ret){
        if(i != '0' && i != '.')
            a = true;
        if(a && i != 0 && i != '.')
            n++;
    }

    if(n < s){
        if(!ret.contains('.'))
            ret += '.';
        for(int i = 0; i<s-n;i++)
            ret += '0';
    }
    return ret;
}
void ALT::displayModel(QAbstractItemModel *model)
{
    QTableView *view = new QTableView;
    view->setModel(model);
    QHBoxLayout *ly = new QHBoxLayout;
    ly->addWidget(view);
    QDialog *dial = new QDialog();
    dial->setLayout(ly);
    dial->show();

}
bool ALT::copyRecursively(const QString &sourceFolder, const QString &destFolder, bool skipAttachedFiles)
{
    bool success = false;
    QDir sourceDir(sourceFolder);

    if(!sourceDir.exists())
        return false;

    QDir destDir(destFolder);
    if(!destDir.exists())
        destDir.mkdir(destFolder);

    QStringList files = sourceDir.entryList(QDir::Files);

    for(int i = 0; i< files.count(); i++) {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = QFile::copy(srcName, destName);
        SetFileAttributesA(destName.toStdString().c_str(), FILE_ATTRIBUTE_NORMAL);
        if(!success)
            return false;
    }

    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);


    if(skipAttachedFiles){
        const int index = files.indexOf("attachedFiles");
        files.removeAt(index);
    }

    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = copyRecursively(srcName, destName);
        if(!success)
            return false;
    }

    return true;
}
QString ALT::toPlainText(const QString &html)
{
    QTextDocument doc;
    doc.setHtml(html);
    return doc.toPlainText();

}
