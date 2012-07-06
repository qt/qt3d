TEMPLATE = subdirs
SUBDIRS += qt3d
!isEmpty(QT.qml.name): SUBDIRS += quick3d
