#ifndef ALABBOOKVIEWER_H
#define ALABBOOKVIEWER_H

#include <QWidget>
#include <QModelIndex>

#include "ALabBook.h"

class QGridLayout;
class QTableWidget;
class QTableWidgetItem;
class AResearchDialog;
class QMenu;


class ALabBookViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ALabBookViewer(const QStringList &labBookanalysisList, const int &expNumber, const QString prefix, const QString &labBookDir, QWidget *parent = nullptr);
    ~ALabBookViewer();
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void copyScheme() const;
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void goToNext(const QString &word);
    void customMenuRequestedExp(const QPoint &pos);
    void openExpSlot();
    void setIndex();

signals:
    void openExpSignal(int exp);

private:
    QTableWidget *m_table;
    QGridLayout *m_layout;
    AResearchDialog *m_finder;
    QMenu *m_contextMenu;
    QModelIndex m_contextIndex;
    QAction *m_copy;
    QAction *m_openExp;
    QString m_dir;
};
#endif // ALABBOOKVIEWER_H
