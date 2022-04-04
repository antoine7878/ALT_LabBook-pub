#ifndef ALABBOOKINFOVIEW_H
#define ALABBOOKINFOVIEW_H

#include "ALabBookViewAbstract.h"

class ALabBookInfoView : public ALabBookViewAbstract
{
    Q_OBJECT
public:
    ALabBookInfoView(QVector<QString*> &vec, QWidget *parent = nullptr);

public slots:
    virtual void commit();

signals:
    void dataChanged();

private:
    QVector<QString*> m_labBookInfos;
};

#endif // ALABBOOKINFOVIEW_H
