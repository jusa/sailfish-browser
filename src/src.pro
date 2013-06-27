QT += opengl qml quick dbus concurrent quick
# The name of your app
TARGET = sailfish-browser

TARGETPATH = /usr/bin
target.path = $$TARGETPATH

DEPLOYMENT_PATH = /usr/share/$$TARGET
qml.path = $$DEPLOYMENT_PATH

INSTALLS += target qml

DEFINES += DEPLOYMENT_PATH=\"\\\"\"$${DEPLOYMENT_PATH}/\"\\\"\"

PKGCONFIG +=  nemotransferengine-qt5

# Include qtmozembed
isEmpty(QTEMBED_LIB) {
  CONFIG += link_pkgconfig
  PKGCONFIG += qtembedwidget
} else {
  LIBS+=$$QTEMBED_LIB
}

isEmpty(DEFAULT_COMPONENT_PATH) {
  DEFINES += DEFAULT_COMPONENTS_PATH=\"\\\"/usr/lib/mozembedlite/\\\"\"
} else {
  DEFINES += DEFAULT_COMPONENTS_PATH=\"\\\"$$DEFAULT_COMPONENT_PATH\\\"\"
}

# C++ sources
SOURCES += \
    declarativebrowsertab.cpp \
    sailfishbrowser.cpp \
    declarativebookmarkmodel.cpp \
    bookmark.cpp \
    declarativewebutils.cpp \
    browserservice.cpp \
    dbusadaptor.cpp \
    declarativewebthumbnail.cpp \
    downloadmanager.cpp

# C++ headers
HEADERS += \
    declarativebrowsertab.h \
    declarativebookmarkmodel.h \
    bookmark.h \
    declarativewebutils.h \
    browserservice.h \
    dbusadaptor.h \
    declarativewebthumbnail.h \
    downloadmanager.h

# QML files and folders
qml.files = *.qml pages cover browser.qml

OTHER_FILES = pages/BrowserPage.qml \
              rpm/sailfish-browser.spec
