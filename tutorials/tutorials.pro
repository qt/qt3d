TEMPLATE = subdirs
SUBDIRS += qt3d
qtHaveModule(qml): SUBDIRS += quick3d

# These tutorial directories contain code that is of instructive value only,
# and therefore it is not packaged for installation along with demos and
# examples, by default.

# To install these apps as part of learning Qt3D simply follow the
# technique used in the examples directories.
