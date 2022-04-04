#ifndef ACOMPOUNDDATABASECREATOR_H
#define ACOMPOUNDDATABASECREATOR_H

#include <QDialog>

namespace Ui {
class ACompoundDatabaseCreator;
}

class ACompoundDatabaseCreator : public QDialog
{
    Q_OBJECT

public:
    explicit ACompoundDatabaseCreator(QWidget *parent = nullptr);
    ~ACompoundDatabaseCreator();

public slots:
        void on_pushButtonBrowse_clicked();
        void create();
        QString execFile();
        void updateCreateButton();

private:
    Ui::ACompoundDatabaseCreator *ui;
    QString m_filePath;
};

#endif // ACOMPOUNDDATABASECREATOR_H
