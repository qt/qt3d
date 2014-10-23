/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QMatrix4x4>
#include <Qt3DCore/QResourcesManager>

class tst_DynamicArrayPolicy : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void benchmarkAllocateSmallResources();
    void benchmarkReleaseSmallResources();
    void benchmarkAllocateBigResources();
    void benchmarkReleaseBigResources();
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
    Qt3D::ArrayAllocatingPolicy<T, 16> allocator;

    const int max = (1 << 16) - 1;
    QBENCHMARK_ONCE {
        for (int i = 0; i < max; i++) {
            T* ptr = allocator.allocateResource();
            Q_UNUSED(ptr);
        }
    }
}

template<typename T>
void benchmarkReleaseResources()
{
    Qt3D::ArrayAllocatingPolicy<int, 16> allocator;

    const int max = (1 << 16) - 1;
    QVector<int*> resources(max);
    for (int i = 0; i < max; i++) {
        resources[i] = allocator.allocateResource();
    }

    QBENCHMARK_ONCE {
        foreach (int* ptr, resources) {
            allocator.releaseResource(ptr);
        }
    }
}

void tst_DynamicArrayPolicy::benchmarkAllocateSmallResources()
{
    benchmarkAllocateResources<SmallType>();
}

void tst_DynamicArrayPolicy::benchmarkReleaseSmallResources()
{
    benchmarkReleaseResources<SmallType>();
}

void tst_DynamicArrayPolicy::benchmarkAllocateBigResources()
{
    benchmarkAllocateResources<BigType>();
}

void tst_DynamicArrayPolicy::benchmarkReleaseBigResources()
{
    benchmarkReleaseResources<BigType>();
}

QTEST_APPLESS_MAIN(tst_DynamicArrayPolicy)

#include "tst_bench_dynamicarraypolicy.moc"
