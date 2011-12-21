TEMPLATE = subdirs
SUBDIRS += basket builder cube cylinder geometry nesting solarsystem tank teapot

contains(QT_CONFIG, opengl): contains(QT_CONFIG, widgets): SUBDIRS += graphicsview
