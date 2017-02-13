
CONFIG -= precompile_header

CONFIG += warn_on

INCLUDEPATH += $$PWD

clang: {
    QMAKE_CFLAGS_WARN_ON += \
        -Wno-unused-parameter \
        -Wno-unused-variable \
        -Wno-extra \
        -Wno-sign-compare
    QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
} gcc:!clang {
    QMAKE_CFLAGS_WARN_ON += \
        -Wno-unused-parameter \
        -Wno-unused-variable \
        -Wno-extra \
        -Wno-sign-compare \
        -Wno-maybe-uninitialized \
        -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
}

include(Bullet3Common/bullet3common.pri)
include(LinearMath/linearmath.pri)
include(BulletCollision/bulletcollision.pri)
include(BulletDynamics/bulletdynamics.pri)
include(BulletInverseDynamics/bulletinversedynamics.pri)
include(BulletSoftBody/bulletsoftbody.pri)
