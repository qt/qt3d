/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QTest>
#include <Qt3DRender/qmemorybarrier.h>
#include <Qt3DRender/private/qmemorybarrier_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QMemoryBarrier : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QMemoryBarrier::Operations>("QMemoryBarrier::Operations");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QMemoryBarrier memoryBarrier;

        // THEN
        QCOMPARE(memoryBarrier.waitOperations(), Qt3DRender::QMemoryBarrier::None);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QMemoryBarrier memoryBarrier;

        {
            // WHEN
            QSignalSpy spy(&memoryBarrier, SIGNAL(waitOperationsChanged(QMemoryBarrier::Operations)));
            const Qt3DRender::QMemoryBarrier::Operations newValue(Qt3DRender::QMemoryBarrier::ShaderStorage|Qt3DRender::QMemoryBarrier::VertexAttributeArray);
            memoryBarrier.setWaitOperations(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(memoryBarrier.waitOperations(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            memoryBarrier.setWaitOperations(newValue);

            // THEN
            QCOMPARE(memoryBarrier.waitOperations(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkTypesUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QMemoryBarrier memoryBarrier;
        arbiter.setArbiterOnNode(&memoryBarrier);

        {
            // WHEN
            memoryBarrier.setWaitOperations(Qt3DRender::QMemoryBarrier::ShaderStorage);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &memoryBarrier);

            arbiter.clear();
        }

        {
            // WHEN
            memoryBarrier.setWaitOperations(Qt3DRender::QMemoryBarrier::ShaderStorage);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QMemoryBarrier)

#include "tst_qmemorybarrier.moc"
