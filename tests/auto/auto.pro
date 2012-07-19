TEMPLATE = subdirs
SUBDIRS = threed \
          cmake

!isEmpty(QT.qml.name): SUBDIRS += imports
contains(QT_CONFIG, qmltest) {
    SUBDIRS += qml3d
    !win32 : SUBDIRS += qml3d_visual
}
