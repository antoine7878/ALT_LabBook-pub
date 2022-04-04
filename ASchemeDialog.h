#ifndef ASCHEMEDIALOG_H
#define ASCHEMEDIALOG_H

#include <QDialog>

namespace Ui {
class ASchemeDialog;
}

class ASchemeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ASchemeDialog(QString const& filePath, QWidget *parent = nullptr);
    ~ASchemeDialog();

public slots:
    void save();

private:
    Ui::ASchemeDialog *ui;
    QString m_saveFilePath;
};

#endif // ASCHEMEDIALOG_H
