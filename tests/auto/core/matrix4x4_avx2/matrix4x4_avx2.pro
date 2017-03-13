TARGET = tst_matrix4x4_avx2
CONFIG += testcase
QT += testlib 3dcore 3dcore-private

QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_AVX2

SOURCES += tst_matrix4x4_avx2.cpp

