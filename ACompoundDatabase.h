#ifndef ACOMPOUNDDATABASE_H
#define ACOMPOUNDDATABASE_H

#include <QFile>
#include <QJsonArray>
#include <QObject>
#include <QStringList>

class ACompoundDatabase : public QObject
{

Q_OBJECT

public:
    explicit ACompoundDatabase(QString fileName, QObject *parent = 0);
    ~ACompoundDatabase();
    static QString createNewDatabase();

    QVariantList query(QString const& name);
    QString getFileName() const;
    int contains(QString const& name);
    bool add(const QHash<QString, QVariant> &hash);
    void updateNameList();
    QString gifFile(int number) const;
    QString cdxmlFile(int number) const;
    QStringList* getNameList() const;
    QJsonArray* getCompoundArray();

public slots:
    bool save(const QVector<int> &index = QVector<int>());
    void load();
    void show();

private:
    QStringList *m_nameList;
    int m_compoundNumberDB;
    QString m_structFolderNameCDXML;
    QString m_structFolderNameGIF;
    QString m_databaseName;
    QFile m_saveFile;
    QJsonArray m_compoundsArray;

};

#endif // ACOMPOUNDDATABASE_H
