TEMPLATE = app
TARGET = moon
CONFIG += qt warn_on

SOURCES += main.cpp
HEADERS += ../qmlres.h

QT += declarative

!package:DESTDIR = ../../../bin

qmlResources.files = qml
symbian {
    DEPLOYMENT = qmlResources
} else {
    macx {
        qmlResources.path = Contents/Resources
        QMAKE_BUNDLE_DATA += qmlResources
    } else {
        !package {
            qmlResources.input = qmlResources.files
            qmlResources.output = $$OUT_PWD/../../../bin/resources/examples/$$TARGET/qml
            qmlResources.commands = $$QMAKE_COPY_DIR ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
            qmlResources.CONFIG += no_link_no_clean
            qmlResources.variable_out = POST_TARGETDEPS
            QMAKE_EXTRA_COMPILERS += qmlResources
        }
    }
}

# for make install use in packages
distInstalls.files = qml
distInstalls.path = $$[QT_INSTALL_DATA]/quick3d/examples/$$TARGET
INSTALLS += distInstalls

package {
    maemo {
        applnk.files = moon.desktop
        applnk.path = /usr/share/applications

        icons.files = icon-l-qtquick3d.png
        icons.path = /usr/share/themes/base/meegotouch/icons
        INSTALLS += icons applnk
    }

    target.path = $$[QT_INSTALL_BINS]
    INSTALLS += target
}
