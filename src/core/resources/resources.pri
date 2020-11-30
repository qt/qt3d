HEADERS += \
    $$PWD/qloadgltf_p.h \
    $$PWD/qresourcemanager_p.h \
    $$PWD/qhandle_p.h

SOURCES += \
    $$PWD/qresourcemanager.cpp


# Define proper SIMD flags for qresourcemanager.cpp
qtConfig(qt3d-simd-avx2) {
    CONFIG += simd
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_AVX2
}

qtConfig(qt3d-simd-sse2):!qtConfig(qt3d-simd-avx2) {
    CONFIG += simd
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_SSE2
}
