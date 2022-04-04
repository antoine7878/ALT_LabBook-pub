#include "ADoubleValidator.h"


ADoubleValidator::ADoubleValidator(double bottom, double top, int decimals, QObject* parent = 0) :
    QDoubleValidator(bottom, top, decimals, parent)
{

}
QValidator::State ADoubleValidator::validate(QString& input, int& pos) const
{
    if(input == QString())
        return Acceptable;
    else
        return QDoubleValidator::validate(input, pos);
}
