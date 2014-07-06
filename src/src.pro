TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
    core \
    render \
    openal

config_bullet: SUBDIRS += bulletphysics

# Build the quick3d plugins last
SUBDIRS += quick3d
