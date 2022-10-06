// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QtTest>
#include <QMatrix4x4>
#include <Qt3DCore/private/qresourcemanager_p.h>

class tst_ArrayPolicy : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void benchmarkDynamicAllocateSmallResources();
    void benchmarkDynamicReleaseSmallResources();
    void benchmarkDynamicAllocateBigResources();
    void benchmarkDynamicReleaseBigResources();
};

struct SmallType
{
    quint32 data;
};

struct BigType
{
    QMatrix4x4 a;
};

template<typename T>
void benchmarkAllocateResources()
{
    Qt3DCore::ArrayAllocatingPolicy<T> allocator;

    const int max = (1 << 16) - 1;
    QBENCHMARK_ONCE {
        for (int i = 0; i < max; i++) {
            (void) allocator.allocateResource();
        }
    }
}

template<typename T>
void benchmarkReleaseResources()
{
    Qt3DCore::ArrayAllocatingPolicy<T> allocator;

    const int max = (1 << 16) - 1;
    std::vector<Qt3DCore::QHandle<T>> resources(max);
    for (size_t i = 0; i < max; i++) {
        resources[i] = allocator.allocateResource();
    }

    QBENCHMARK_ONCE {
        for (auto ptr : std::as_const(resources)) {
            allocator.releaseResource(ptr);
        }
    }
}

void tst_ArrayPolicy::benchmarkDynamicAllocateSmallResources()
{
    benchmarkAllocateResources<SmallType>();
}

void tst_ArrayPolicy::benchmarkDynamicReleaseSmallResources()
{
    benchmarkReleaseResources<SmallType>();
}

void tst_ArrayPolicy::benchmarkDynamicAllocateBigResources()
{
    benchmarkAllocateResources<BigType>();
}

void tst_ArrayPolicy::benchmarkDynamicReleaseBigResources()
{
    benchmarkReleaseResources<BigType>();
}

QTEST_APPLESS_MAIN(tst_ArrayPolicy)

#include "tst_bench_arraypolicy.moc"
