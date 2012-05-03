TEMPLATE = subdirs
SUBDIRS = threed imports qml3d
!win32 : SUBDIRS += qml3d_visual
