TEMPLATE = subdirs
contains(QT_CONFIG, widgets): contains(QT_CONFIG, opengl): SUBDIRS += qglinfo
