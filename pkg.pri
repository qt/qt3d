#!CONFIG(pkg_pri_included) {
#CONFIG+=pkg_pri_included

# Config for making example and demo apps packageable

# Note that the paths here all assumed the including .pro file
# is exactly 3 levels of directory tree below the root

# Include this file after qt3d_pkg_dep.pri or qt3dquick_pkg_dep.pri

qtc_harmattan {
    CONFIG += maemo
    CONFIG += package
    QMAKE_CXXFLAGS += -Wno-psabi
    # The Qt SDK / Qt Creator harmattan integration needs some special treatment
    QT3D_INSTALL_BINS = /bin
    QT3D_INSTALL_LIBS = /usr/lib
    QT3D_INSTALL_PLUGINS = /usr/lib/qt4/plugins
    QT3D_INSTALL_IMPORTS = /usr/lib/qt4/imports
    QT3D_INSTALL_DATA = /usr/share/qt4
} else {
    QT3D_INSTALL_BINS = $$[QT_INSTALL_BINS]
    QT3D_INSTALL_LIBS = $$[QT_INSTALL_LIBS]
    QT3D_INSTALL_PLUGINS = $$[QT_INSTALL_PLUGINS]
    QT3D_INSTALL_IMPORTS = $$[QT_INSTALL_IMPORTS]
    QT3D_INSTALL_DATA = $$[QT_INSTALL_DATA]
}

qt3dquick_deploy_pkg {
    CONFIG += qt3d_deploy_pkg

    package {
        macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
            LIBS += -framework Qt3DQuick -F../../../src/quick3d
            INCLUDEPATH += ../../../src/quick3d/Qt3DQuick.framework/Versions/1/Headers
        } else {
            win32 {
                CONFIG(debug, debug|release) {
                    TARGET = $$member(TARGET, 0)d
                    LIBS += ..\\..\\..\\src\\quick3d\\debug\\Qt3DQuickd.lib
                } else {
                    LIBS += ..\\..\\..\\src\\quick3d\\release\\Qt3DQuick.lib
                }
            } else {
                LIBS += -L../../../src/quick3d -lQt3DQuick
            }
            INCLUDEPATH += ../../../include/Qt3DQuick
        }
        QT += declarative opengl
    
        maemo: icons.files = icon-l-qtquick3d.png
    }
    
    contains(TEMPLATE, app){
        symbian {
            ICON = ../qtquick3d.svg
        }
    }
}

qt3d_deploy_pkg {
    package {
        macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
            LIBS += -framework Qt3D -F../../../src/threed
            INCLUDEPATH += ../../../src/threed/Qt3D.framework/Versions/1/Headers
        } else {
            win32 {
                CONFIG(debug, debug|release) {
                    TARGET = $$member(TARGET, 0)d
                    LIBS += ..\\..\\..\\src\\threed\\debug\\Qt3Dd.lib
                } else {
                    LIBS += ..\\..\\..\\src\\threed\\release\\Qt3D.lib
                }
            } else {
                LIBS += -L../../../src/threed -lQt3D
            }
            INCLUDEPATH += ../../../include/Qt3D
        }
        QT += opengl
    }
}

contains(TEMPLATE, app) {
    package  {
        maemo {
            applnk.files = $${TARGET}.desktop
            applnk.path = /usr/share/applications

            # icons.files is set by qt3dquick_pkg_dep.pri or qt3d_pkg_dep.pri
            icons.path = /usr/share/themes/base/meegotouch/icons
            INSTALLS += icons applnk

            target.path += $$QT3D_INSTALL_BINS
            INSTALLS += target
        } else {
            mt {
                applnk.files = info.json
                applnk.path = /opt/mt/applications/$${TARGET}

                # icons.files is set by qt3dquick_pkg_dep.pri or qt3d_pkg_dep.pri
                icons.path = /opt/mt/applications/$${TARGET}
                INSTALLS += icons applnk
                target.path += /opt/mt/applications/$${TARGET}
                INSTALLS += target

                DEFINES += QT3D_USE_OPT=$${TARGET}
            } else {
                target.path += $$QT3D_INSTALL_BINS
                INSTALLS += target
            }
        }
    } else {
        DESTDIR = ../../../bin
    }
    symbian {
        vendorinfo = \
         "%{\"Nokia\"}" \
         ":\"Nokia\""

        my_deployment.pkg_prerules = vendorinfo
        DEPLOYMENT += my_deployment

        ICON = ../qt3d.svg
    }
}
#}

qt3d_deploy_qml {
    mt {
        distInstalls.files = qml
        distInstalls.path = /opt/mt/applications/$$TARGET
        INSTALLS += distInstalls
    } else {
        distInstalls.files = qml
        distInstalls.path = $$QT3D_INSTALL_DATA/quick3d/examples/$$TARGET
        INSTALLS += distInstalls
    }
    
    win32 {
        QMAKE_DEL_FILE = del /q
        QMAKE_DEL_DIR = rmdir /s /q
    }
    
    qmlResources.files = qml
    symbian {
        DEPLOYMENT += qmlResources
        qmlDeployment.sources = qml\*
        qmlDeployment.path = qml
        DEPLOYMENT += qmlDeployment
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
}
