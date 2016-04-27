QT += 3dcore 3drender 3dinput 3dlogic qml 3dextras

INCLUDEPATH += $$PWD

win32 {
    build_pass {
        CONFIG(debug, debug|release) {
            common_static_lib = $$OUT_PWD/../examples-common/debug/$${QMAKE_PREFIX_STATICLIB}examples-common.$${QMAKE_EXTENSION_STATICLIB}
        } else {
            common_static_lib = $$OUT_PWD/../examples-common/release/$${QMAKE_PREFIX_STATICLIB}examples-common.$${QMAKE_EXTENSION_STATICLIB}
        }
    }
    LIBS += $$common_static_lib
    PRE_TARGETDEPS += $$common_static_lib
} else {
    LIBS += -L$$OUT_PWD/../examples-common -l$$qtLibraryTarget(examples-common)
    PRE_TARGETDEPS += $$OUT_PWD/../examples-common/lib$$qtLibraryTarget(examples-common).a
}

RESOURCES += $$OUT_PWD/../examples-common/examples-common.qrc
