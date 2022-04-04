#ifndef ADOUBLEVALIDATOR_H
#define ADOUBLEVALIDATOR_H

#include <QDoubleValidator>

class ADoubleValidator: public QDoubleValidator
{
    Q_OBJECT
public:  
    ADoubleValidator(double bottom, double top, int decimals, QObject* parent);
    QValidator::State validate (QString& input, int& pos) const;
};

#endif // ADOUBLEVALIDATOR_H
