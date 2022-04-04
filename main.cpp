#include <QApplication>
#include <QAxObject>
#include <QStyleFactory>
#include <QLocale>
#include <QDebug>

#include "MainWindow.h"
#include "ALT.h"

int main(int argc, char *argv[])
{
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QApplication app(argc, argv);
//    app.setFont(QFont("MS Shell Dlg 2", 11));
    QLocale::setDefault(QLocale::English);
    QAxObject *schemeChemDraw = new QAxObject("{84328ED3-9299-409F-8FCC-6BB1BE585D08}");
    delete schemeChemDraw;
    QString str;
    str =   "QTableView QHeaderView::section:horizontal {"
            "background:  white;"
            "padding-bottom: 1px solid black;"
            "padding-left: 1px solid black;"
            "padding-right: 1px solid black;"
            "padding-top: 0px solid black;"
            "}"

            "QTableView QHeaderView::section:vertical {"
            "background:  white;"
            "padding-bottom: 1px solid black;"
            "padding-left: 0px solid black;"
            "padding-right: 1px solid black;"
            "padding-top: 1px solid black;"
            "}"

            "QTableView QTableCornerButton::section {"
            "background:  white;"
            "border-bottom: 1px solid black;"
            "border-left: 0px solid black;"
            "border-right: 1px solid black;"
            "border-top: 0px solid black;"
            "}";

    qApp->setStyleSheet(str);
    MainWindow mw;
    mw.show();

    return app.exec();
}
