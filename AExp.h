#ifndef AEXP_H
#define AEXP_H

#include <QString>
#include <QDir>
#include <QStringList>
#include <QObject>
#include <QJsonArray>
#include <QDate>

#include "ALabBook.h"
#include "AExpTableModel.h"

class QAxObject;
class AConditionsTableModel;

struct Position{
    double leftX = 0;
    double rightX = 0;
    double topY = 0;
    double bottomY = 0;
    double centerX = 0;
    double centerY = 0;
};

class AExp : public QObject
{
Q_OBJECT

public:    
    enum CompletionState{
        Created = 6,
        OnGoing = 16,
        Finished = 12,
        Closed = 3
    };


    AExp(QString expName, const QString &folderName, bool isNew, const QVector<QPair<QString, bool>> &analysis = QVector<QPair<QString,bool>>());
    void setCompleterModelCond(QStandardItemModel *model);
    ~AExp();

    int getExpNumber() const;
    QString getUserTitle() const;
    QString getExpName() const;
    QString getProcedure() const;
    QString getNotes() const;
    QString getReference() const;
    QDate getDate() const;
    QString getDirectory() const;
    QString getSchemePathCDXML() const;
    QString getSchemePathGIF() const;
    QVector<QPair<QString, bool>> getAnalysis();
    QHash<QString, QVariant> getHashDatabase(int row);
    int getRowCountCondition() const;
    QString getSaveDirectoryPath() const;
    QModelIndex getQModelIndex(const int row, const int col);
    AExpTableModel* getModel();
    int getSignificantNumbers() const;
    QStandardItemModel* getCompleterModel();
    QJsonArray getExpConditions() const;
    AExp::CompletionState getCompletionState() const;

    void insertRowCond(const int row);
    void removeRowCond(const int row);

    void setUserTitle(QString const& str);
    void setProcedure(QString const& str);
    void setNotes(QString const& str);
    void setReference(QString const& str);
    void setSignificantNumbers(const int n);
    void setExpConditions(const QJsonArray expConditions);
    void setDate(const QDate &date);
    void setCompletionState(AExp::CompletionState state);

    bool load();
    void insertRow(const int &row, const int &role, const int &position);
    int getRowCount();
    void loopRole(const int row);
    void removeRow(const int row);
    void moveRowUp(const int row);
    void moveRowDown(const int row);
    void updateScheme(const QStringList &conditions);
    void receiveQuery(const QVariantList &list, int row);
    bool canUpdateHere(const QModelIndex &index);

public slots:
    bool save();
    void setAnalysis(const QVector<QPair<QString, bool> > &list);
    void updateCompleterModelExp();
    void updateCompleterModelCond();
    void updateModel(const QModelIndex &index);
    void buttonStructClicked(int row);

protected:
    void constructChemDraws();
    double makeVScheme(const QVector<int> &rowsTop, const QVector<int> &rowsBottom);
    double makeHSchemeBox(const QVector<int> &rowsTop);
    Position addStructAtXY(const int row, QAxObject* chemDraw, double x, double y, bool atRight, bool atTop);
    Position addLabelAtXY(const QString &text, QAxObject* chemDraw, double x, double y, bool atRight, bool atTop);
    Position pasteAtXY(QAxObject* chemDraw, double x, double y, bool atRight, bool atBottom, bool atLeft = false);
    QString makeLabel(const int row);

signals:
    void modelChanged();

private:
    static double m_interCompoundDistance;

    QDir m_saveDir;
    QFile m_saveFile;

    AExpTableModel* m_tableModel;

    int m_expNumber;
    QString m_userTitle;
    QDate m_creationDate;
    QString m_expName;
    QString m_procedure;
    QString m_notes;
    QString m_reference;
    int m_counterPath;
    int m_significantNumbers;
    QVector<QPair<QString, bool>> m_analysis;
    QJsonArray m_expConditions;
    QStringList m_expConditionsScheme;
    CompletionState m_completionState;

    QAxObject *m_schemeChemDraw;
    QAxObject *m_chemDrawVbox;
    QAxObject *m_chemDrawHbox;
    QAxObject *m_tempChemDraw;
    double m_copyWidth;
    double m_copyHeight;

    QStandardItemModel *m_completerModelCond;
    QStandardItemModel *m_completerModelExp;
    QStandardItemModel *m_completerModel;
    int m_previousComplterModelExpSize;
    int m_previousComplterModelCondSize;

    bool m_inQuery;
};

#endif // AEXP_H
