
# Determine which qdoc3 binary we should be using.  We try to use the
# one from the Qt we built against.  Otherwise use "qdoc3" on the PATH.
TOPSRC          = $$PWD/..
QDOCCONF_FILE   = $$PWD/config/qt3d-online.qdocconf
DESTDIR         = $$OUT_PWD/doc

win32:!win32-g++ {
    unixstyle = false
} else :win32-g++:isEmpty(QMAKE_SH) {
    unixstyle = false
} else {
    unixstyle = true
}

$$unixstyle {
    exists($$[QT_INSTALL_BINS]/qdoc3) {
        QDOC = $$[QT_INSTALL_BINS]/qdoc3
    } else {
        QDOC = qdoc3
    }
    docs.commands = (TOPSRC=$$TOPSRC DESTDIR=$$DESTDIR $$QDOC $$QDOCCONF_FILE)
} else {
    exists($$[QT_INSTALL_BINS]\\qdoc3.exe) {
        QDOC = $$[QT_INSTALL_BINS]\\qdoc3
    } else {
        QDOC = qdoc3
    }
    docs.commands = (set TOPSRC=$$TOPSRC&set DESTDIR=$$DESTDIR&$$QDOC $$QDOCCONF_FILE)
}

QMAKE_EXTRA_TARGETS += docs
