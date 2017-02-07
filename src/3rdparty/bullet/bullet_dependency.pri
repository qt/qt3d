QT_FOR_CONFIG += 3dcore-private
qtConfig(system-bullet):!if(cross_compile:host_build) {
    QMAKE_USE_PRIVATE += bullet
} else {
    include(bullet.pri)
}
