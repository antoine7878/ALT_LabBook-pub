#ifndef ALT_H
#define ALT_H

#include <QString>
#include <QAbstractItemModel>
#include <QTextLayout>
#include <QTextEdit>

class QLineEdit;

namespace ALT
{
    QString displayX(double x, int s);
    void displayModel(QAbstractItemModel *model);
    bool copyRecursively(const QString &sourceFolder, const QString &destFolder, bool skipAttachedFiles = false);
    QString toPlainText(const QString &html);
};

#endif // ALT_H
