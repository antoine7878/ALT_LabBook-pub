#ifndef ALABBOOKCREATOR_H
#define ALABBOOKCREATOR_H

#include <QDialog>

#include "ALabBook.h"
#include "ui_ALabBookViewAbstract.h"

namespace Ui {
    class ALabBookViewAbstract;
}

class ALabBookViewAbstract : public QDialog
{
    Q_OBJECT

protected:
    explicit ALabBookViewAbstract(QWidget *parent = nullptr);
    ~ALabBookViewAbstract();


protected:
    Ui::ALabBookViewAbstract *ui;
    QString m_filePath;
};

#endif // ALABBOOKCREATOR_H
