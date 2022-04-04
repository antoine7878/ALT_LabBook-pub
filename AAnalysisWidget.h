#ifndef AANALYSISWIDGET_H
#define AANALYSISWIDGET_H

#include <QWidget>
#include <QMutex>

class QCheckBox;
class FlowLayout;
class QMenu;
class QAction;
class QWidgetAction;
class ALineAction;

class AAnalysisWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AAnalysisWidget(QWidget *parent = nullptr);
    bool addAnalysis(const QString &analysis, bool activated = false, bool send = true);
    void load(const QVector<QPair<QString, bool>> &lst);
    inline void removeAnalysis(int index);
    void updateAnalysis(QStringList lst);
    QVector<QString> getAnalysisListSimple();

public slots:
    void changeState();

signals:
    void changed(const QVector<QPair<QString, bool>> &saveList);

private:
    QMutex m_mutex;
    FlowLayout *m_layout;
    QVector<QCheckBox*> m_analysisList;
    QVector<QPair<QString, bool>> m_saveList;
};

#endif // AANALYSISWIDGET_H
