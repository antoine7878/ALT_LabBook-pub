#ifndef ACONDITIONWIDGET_H
#define ACONDITIONWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

class QGridLayout;
class QStandardItemModel;
class QVBoxLayout;
class QHBoxLayout;
class ConditionLine;

class AConditionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AConditionWidget(QWidget *parent = nullptr);

    QJsonArray toJsonArray() const;
    void fromJsonArray(QJsonArray const& array);

    QStandardItemModel *getCompleterModel();
    QStringList getSchemeLines() const;


public slots:
    void updateCompleterModel();
    void addLine(const double &temperature = 0, const int &hour = 0, const int &min = 0);
    void removeLine();

signals:
    void completerModelChangedCond();

private:
    QList<ConditionLine*> m_conditions;
    QStandardItemModel* m_completerModel;

    QVBoxLayout *m_layout;
    QVBoxLayout *m_expLayout;
    QPushButton *m_addLineButton;
};


class ConditionLine : public QWidget
{
    Q_OBJECT
public:

    ConditionLine(const int &line, const double &temperature, const int &hour, const int &min, AConditionWidget *parent);
    void setRmButtonLine(const int &i);
    QJsonObject getJsonLine() const;
    QString getCompleterLine() const;
    QString getSchemeLine() const ;

private:

    QSpinBox *m_tempEditor;
    QLabel *m_tempLabel;
    QSpinBox *m_hourEditor;
    QLabel *m_hourLabel;
    QSpinBox *m_minEditor;
    QLabel  *m_minLabel;
    QPushButton *m_rmButton;

    QHBoxLayout *m_layout;
};

class ASpinBoxNoWheel : public QSpinBox
{
    Q_OBJECT
public:
    ASpinBoxNoWheel(QWidget *parent = nullptr);

protected:
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // ACONDITIONWIDGET_H
