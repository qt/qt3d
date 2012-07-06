TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = threed
!isEmpty(QT.qml.name): SUBDIRS += quick3d imports
