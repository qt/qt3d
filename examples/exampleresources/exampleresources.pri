INCLUDEPATH +=  $$PWD

EXAMPLERESOURCESLIB=exampleresources

win32 {
    build_pass {
        CONFIG(debug, debug|release) {
            LIBS += $$shadowed($$PWD)/debug/$${QMAKE_PREFIX_STATICLIB}$${EXAMPLERESOURCESLIB}.$${QMAKE_EXTENSION_STATICLIB}
        } else {
            LIBS += $$shadowed($$PWD)/release/$${QMAKE_PREFIX_STATICLIB}$${EXAMPLERESOURCESLIB}.$${QMAKE_EXTENSION_STATICLIB}
        }
    }
} else {
    LIBS += $$shadowed($$PWD)/$${QMAKE_PREFIX_STATICLIB}$${EXAMPLERESOURCESLIB}.$${QMAKE_EXTENSION_STATICLIB}
}

unset(EXAMPLERESOURCESLIB)
