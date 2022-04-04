#ifndef AANALYSISMANAGER_H
#define AANALYSISMANAGER_H

#include <QDialog>
#include <QListWidget>

class AListWidget;
class QLineEdit;
class QGridLayout;
class QCheckBox;

class AAnalysisManager : public QDialog
{
    Q_OBJECT
public:
    explicit AAnalysisManager(const QVector<QPair<QString,bool>> &analysis, QWidget *parent = nullptr);
    QVector<QPair<QString, bool>> execList();

    void keyPressEvent(QKeyEvent *event);

public slots:
    void addAnalysis();
    void removeAll();


private:
    AListWidget *m_list;
    QLineEdit *m_lineEdit;
    QGridLayout *m_layout;
    QPushButton *m_buttonRemoveAll;
};


class AListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AListWidget(const QVector<QPair<QString, bool> > &analysis, QWidget *parent = nullptr);
    void addAnalysis(const QString &analysis);
    void removeAnalysis();
    void keyPressEvent(QKeyEvent *event);
    QVector<QPair<QString, bool>> getList();

public slots:
    void updateAnalysisList();
    void highlightChecked(QListWidgetItem* item);
    void checkNHighlight(QListWidgetItem* item);

private:
    QVector<QPair<QString, bool>> m_retList;
};

#endif // AANALYSISMANAGER_H
