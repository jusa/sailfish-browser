TARGET = tst_declarativetabmodel

QT += quick sql

include(../test_common.pri)
include(../mocks/declarativewebpage//declarativewebpage_mock.pri)
include(../mocks/declarativewebcontainer/declarativewebcontainer_mock.pri)
include(../common/testobject.pri)
include(../../../src/history/history.pri)

SOURCES += tst_declarativetabmodel.cpp
