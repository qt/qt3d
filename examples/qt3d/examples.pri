TEMPLATE = app

INCLUDEPATH += $$PWD/common
SOURCES += $$PWD/common/window.cpp
HEADERS += $$PWD/common/window.h

target.path = $$[QT_INSTALL_EXAMPLES]/qt3d/$$TARGET
INSTALLS += target
