INCLUDEPATH +=  $$PWD

win32 {
    build_pass {
        CONFIG(debug, debug|release) {
            LIBS += $$shadowed($$PWD)/debug/exampleresources.$${QMAKE_EXTENSION_STATICLIB}
        } else {
            LIBS += $$shadowed($$PWD)/release/exampleresources.$${QMAKE_EXTENSION_STATICLIB}
        }
    }
} else {
    LIBS += $$shadowed($$PWD)/libexampleresources.$${QMAKE_EXTENSION_STATICLIB}
}
