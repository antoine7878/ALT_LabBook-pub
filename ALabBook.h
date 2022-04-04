#ifndef ALABBOOK_H
#define ALABBOOK_H

#include <QFile>
#include <QPixmap>

class AExp;
class QStringListModel;
class ALabBookInfoView;

struct ViewerEntry {
    QString expName;
    QString creationDate;
    QString userTitle;
    double scale;
    double yield;
    QPixmap scheme;
    QVector<QString> analysis;
};

class ALabBook : public QObject
{
    Q_OBJECT


public:
    ALabBook(QString fichier, QObject *parent = 0);
    ~ALabBook();

    static QString createNewLabBook();
    AExp* addNewExp();
    AExp* getExp(QString const& expName);
    void updateExpList();
    QStringListModel* getExpList();
    int getExpNumberBook() const;
    QString getExpPrefix() const;
    QString getFolderName() const;
    QString getFileName() const;
    ALabBookInfoView* makeInfoWidget(QWidget *parent = nullptr);
    bool view();
    QVector<QPair<QString, bool> > getAnalysisList();
    void setAnalysisList(const QVector<QPair<QString, bool> > &analysisList);
    QString copyExp(const QString &sourceName);
    void exportAsODF();

public slots:
    bool save();
    int rmExp();
    bool load(const QString &fileName = QString());

signals:
    void openExpSignal(int exp);

private:
    QVector<AExp*> m_loadedAExpVector;

    QString m_name;
    QString m_expPrefix;
    QString m_compagny;
    QString m_signature;
    QString m_description;
    QString m_creationDate;
    int m_expNumberBook;

    QVector<QPair<QString,bool>> m_analysisList;

    QStringListModel *m_expListModel;
    ALabBookInfoView *m_labBookView;

    QString m_folderName;
    QFile m_saveFile;
};

#endif // ALABBOOK_H
