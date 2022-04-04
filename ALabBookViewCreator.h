#ifndef ALABBOOKVIEWCREATOR_H
#define ALABBOOKVIEWCREATOR_H

#include "ALabBookViewAbstract.h"

class ALabBookViewCreator : public ALabBookViewAbstract
{
    Q_OBJECT
public:
    ALabBookViewCreator(QWidget *parent = nullptr);

public slots:
    void create();
    void browseSlot();
    QString execFile();
    void updateCreateButton();
    void connectUpdates();

};

#endif // ALABBOOKVIEWCREATOR_H
