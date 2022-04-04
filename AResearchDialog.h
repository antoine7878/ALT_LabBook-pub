#ifndef ARESEARCHDIALOG_H
#define ARESEARCHDIALOG_H

#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>

class QLabel;
class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QWheelEvent;


class AResearchDialog : public QWidget
{
    Q_OBJECT
public:
    AResearchDialog(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void sendsearchRequest();

signals:
    void searchRequest(QString);

private:
    QLabel *label;
    QLineEdit *line;
    QPushButton *findButton;
    QHBoxLayout *layout;
};




#endif // ARESEARCHDIALOG_H
