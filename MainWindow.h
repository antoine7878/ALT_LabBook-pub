#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

class QSettings;
class ALabBook;
class ACompoundDatabase;
class AExpView;
class AExp;
class QMenu;
class QAction;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void displayExp(AExp *exp);
    void updateExpViewIndexes();
    void closeAllTabs();

public slots:
    void addNewExp();
    void openSelectedExp(QModelIndex const& index);
    void closeTab(int tabIndex);
    void closeEvent(QCloseEvent *event);

    void newLabBook();
    void loadLabBook(QString fileName = QString());
    void setDefaultLabBook(QString str = QString());
    void resetDefaultLabBook();
    void viewLabBook();
    void openExportDialog();

    void newDatabase();
    void loadDatabase(QString fileName = QString());
    void setDefaultDatabase(QString str = QString());
    void resetDefaultDatabase();

    void manageAnalysis();
    void disableAcForLabBook(bool state);

    void customMenuRequestedExpList(const QPoint &point);
    void copyExp();
    void rmLastExp();

    void openExpFromViewer(int exp);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void keyPressEvent(QKeyEvent *event);

signals:
    void enableAddToDatabaseAcSignal(bool const& enable);

private:
    Ui::MainWindow *ui;
    QVector<AExpView*> m_expViewVector;
    ALabBook *m_labBook;
    QSettings *m_settings;
    ACompoundDatabase *m_database;

    QModelIndex m_currentIndex;
    QMenu *m_expSelectionMenu;
    QAction *m_copyAc;
    QAction *m_rmExpAc;
};

#endif // MAINWINDOW_H
