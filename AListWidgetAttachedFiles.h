#ifndef ALISTWIDGETATTACHEDFILES_H
#define ALISTWIDGETATTACHEDFILES_H

#include <QWidget>
#include <QStyledItemDelegate>
#include <QDir>
#include <QListWidget>


class QStringListModel;
class QPushButton;
class QGridLayout;
class QListView;
class QToolButton;
class QMenu;
class QFileIconProvider;
class QHBoxLayout;
class QVBoxLayout;
class QListWidgetItem;
class AListWidgetAttachedFilesWidget;

class AListWidgetAttachedFiles : public QWidget
{
    Q_OBJECT
public:
    AListWidgetAttachedFiles(QWidget *parent = nullptr);
    void addList(const QString &directory);
    void closeOpenedDirs(AListWidgetAttachedFilesWidget *source, const QString path);
    void openNewMNova(QDirIterator &it);
    void keyPressEvent(QKeyEvent *event);


public slots:
    void openDir(AListWidgetAttachedFilesWidget *source, const QString path);
    void goToPrev();
    void setFocus();

private:
    QVector<AListWidgetAttachedFilesWidget *> m_listVector;
    QPushButton *m_pushButtonPrev;

    QHBoxLayout *m_buttonLayout;
    QHBoxLayout *m_listLayout;
    QVBoxLayout *m_layout;
};

class AListWidgetAttachedFilesWidget : public QListWidget
{
    Q_OBJECT
public:

enum FileType{
    Folder,
    Word,
    Excel,
    PowerPoint,
    ChemDraw,
    Txt,
    Mnova
};
    AListWidgetAttachedFilesWidget(const QString &directory, AListWidgetAttachedFiles *attachedFileWidget, QWidget *parent = nullptr);
    ~AListWidgetAttachedFilesWidget();
    QString getDirectoy() const;
    void loadDir();
    QIcon getIcon(const QString &path);
    void  resetAllBackground();
    void addFile(const QString &path, const QString &newName);
    void giveFocus();
    QString addDefaultFile(FileType file);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);
    Qt::DropActions supportedDropActions() const;
    void keyPressEvent(QKeyEvent *event);

public slots:
    void removeFile();
    void customMenuRequestedList(QPoint const& pos);
    void editCurrentItem();
    void openSelection(QListWidgetItem *item);
    void addDefaultFileSlt();

signals:
    void delegateDone();

private:
    QString m_directory;
    QFileIconProvider *m_iconProvider;
    AListWidgetAttachedFiles *m_attachedFilesWidget;
    QMenu *m_menuListWidget;
    QMenu *m_menuAdd;

    QAction *m_removeAc;
    QAction *m_renameAc;

    static QBrush whiteBrush;
    static QBrush selectedBrush;

};

class AListWidgetAttachedFilesDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit AListWidgetAttachedFilesDelegate(const QString &directory, QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;   

private:
    QString m_directory;
};

#endif // ALISTWIDGETATTACHEDFILES_H
