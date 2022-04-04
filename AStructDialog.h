#ifndef ASTRUCTDIALOG_H
#define ASTRUCTDIALOG_H

#include <QDialog>

namespace Ui {
class AStructDialog;
}

class AStructDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AStructDialog(QString const& structureFile = QString(),QWidget *parent = nullptr);
    ~AStructDialog();
    QString execStruct();

public slots:
    void save();

private:
    Ui::AStructDialog *ui;
    QString m_structureFile;
};

#endif // ASTRUCTDIALOG_H
