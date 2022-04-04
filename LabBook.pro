QT += widgets
QT += axcontainer

DEFINES += QT_DEPRECATED_WARNINGS

#windows {
#    INCLUDEPATH += C:/Users/Antoine/GitHub/ALT_LabBook/quazip
#    INCLUDEPATH += C:/Users/Antoine/GitHub/ALT_LabBook/zlib128-dll/include
#    LIBS += C:/Users/Denis/git/ControlloAccessi/quazip/quazip/release/quazip.lib
#}

SOURCES += \
    AAnalysisManager.cpp \
    AAnalysisWidget.cpp \
    ACompleterTextEdit.cpp \
    ACompoundDatabase.cpp \
    ACompoundDatabaseCreator.cpp \
    ACompoundDatabaseView.cpp \
    AConditionWidget.cpp \
    ADoubleValidator.cpp \
    AExp.cpp \
    AExpTableDelegate.cpp \
    AExpTableModel.cpp \
    AExpView.cpp \
    AExportAsODF.cpp \
    ALT.cpp \
    ALabBook.cpp \
    ALabBookInfoView.cpp \
    ALabBookViewCreator.cpp \
    ALabBookViewer.cpp \
    ALabbookViewAbstract.cpp \
    AListWidgetAttachedFiles.cpp \
    AResearchDialog.cpp \
    ASchemeDialog.cpp \
    AStructDialog.cpp \
    MainWindow.cpp \
    main.cpp \
    flowlayout.cpp

HEADERS += \
    AAnalysisManager.h \
    AAnalysisWidget.h \
    ACompleterTextEdit.h \
    ACompoundDatabase.h \
    ACompoundDatabaseCreator.h \
    ACompoundDatabaseView.h \
    AConditionWidget.h \
    ADoubleValidator.h \
    AExp.h \
    AExpTableDelegate.h \
    AExpTableModel.h \
    AExpView.h \
    AExportAsODF.h \
    ALT.h \
    ALabBook.h \
    ALabBookInfoView.h \
    ALabBookViewAbstract.h \
    ALabBookViewCreator.h \
    ALabBookViewer.h \
    AListWidgetAttachedFiles.h \
    AResearchDialog.h \
    ASchemeDialog.h \
    AStructDialog.h \
    MainWindow.h \
    flowlayout.h \

FORMS += \
    ACompoundDatabaseCreator.ui \
    AExpView.ui \
    AExportAsODF.ui \
    ALabBookViewAbstract.ui \
    ASchemeDialog.ui \
    AStructDialog.ui \
    MainWindow.ui

RESOURCES += \
    General.qrc
