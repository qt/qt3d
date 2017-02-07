
CONFIG -= precompile_header

CONFIG += warn_on

INCLUDEPATH += $$PWD

gcc|clang: {
    # Stop compiler complaining about ignored qualifiers on return types
    QMAKE_CFLAGS_WARN_ON += \
        -Wno-unused-parameter \
        -Wno-unused-variable \
        -Wno-extra \
        -Wno-maybe-uninitialized \
        -Wno-sign-compare \
        -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
}

include(Bullet3Common/bullet3common.pri)
include(LinearMath/linearmath.pri)
include(BulletCollision/bulletcollision.pri)
include(BulletDynamics/bulletdynamics.pri)
include(BulletInverseDynamics/bulletinversedynamics.pri)
include(BulletSoftBody/bulletsoftbody.pri)
