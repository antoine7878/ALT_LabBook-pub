#ifndef AEXPORTASODF_H
#define AEXPORTASODF_H

#include <QDialog>

class QListWidgetItem;


namespace Ui {
class AExportAsODF;
}

class AExportAsODF : public QDialog
{
    Q_OBJECT

public:
    explicit AExportAsODF(const QString &prefix, const int &expNumber, const QString &folderName, const QString &compagny, const QString &signature, const QString name, QWidget *parent = nullptr);
    ~AExportAsODF();

    void keyPressEvent(QKeyEvent *event);

public slots:
    void toggleRangeEditors(bool state);
    void on_buttonExport_clicked();
    void changeLastValue(int i);
    void changeFirstValue(int i);

private:
    Ui::AExportAsODF *ui;
    int m_expNumber;
    QString m_expPrefix;
    QString m_folderName;
    QString m_compagny;
    QString m_signature;
    QString m_name;
};

#endif // AEXPORTASODF_H
