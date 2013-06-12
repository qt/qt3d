TEMPLATE = subdirs
SUBDIRS = threed \
          cmake

qtHaveModule(qml): SUBDIRS += imports
qtHaveModule(qmltest) {
    SUBDIRS += qml3d

    !win32 : SUBDIRS += qml3d_visual \
                        qml3d_cpp
}
