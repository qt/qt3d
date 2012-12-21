TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = threed
qtHaveModule(qml): SUBDIRS += quick3d imports
