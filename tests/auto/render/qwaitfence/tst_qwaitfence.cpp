/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QTest>
#include <Qt3DRender/qwaitfence.h>
#include <Qt3DRender/private/qwaitfence_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include "testpostmanarbiter.h"

class tst_QWaitFence : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QWaitFence::HandleType>("HandleType");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QWaitFence waitFence;

        // THEN
        QCOMPARE(waitFence.handleType(), Qt3DRender::QWaitFence::NoHandle);
        QCOMPARE(waitFence.handle(), QVariant());
        QCOMPARE(waitFence.waitOnCPU(), false);
        QCOMPARE(waitFence.timeout(), quint64(-1));
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QWaitFence waitFence;

        {
            // WHEN
            QSignalSpy spy(&waitFence, SIGNAL(handleTypeChanged(HandleType)));
            const Qt3DRender::QWaitFence::HandleType newValue = Qt3DRender::QWaitFence::OpenGLFenceId;
            waitFence.setHandleType(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(waitFence.handleType(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            waitFence.setHandleType(newValue);

            // THEN
            QCOMPARE(waitFence.handleType(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&waitFence, SIGNAL(handleChanged(QVariant)));
            const QVariant newValue(883);
            waitFence.setHandle(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(waitFence.handle(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            waitFence.setHandle(newValue);

            // THEN
            QCOMPARE(waitFence.handle(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&waitFence, SIGNAL(waitOnCPUChanged(bool)));
            const bool newValue = true;
            waitFence.setWaitOnCPU(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(waitFence.waitOnCPU(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            waitFence.setWaitOnCPU(newValue);

            // THEN
            QCOMPARE(waitFence.waitOnCPU(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&waitFence, SIGNAL(timeoutChanged(quint64)));
            const quint64 newValue = 984;
            waitFence.setTimeout(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(waitFence.timeout(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            waitFence.setTimeout(newValue);

            // THEN
            QCOMPARE(waitFence.timeout(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DRender::QWaitFence waitFence;

        waitFence.setHandleType(Qt3DRender::QWaitFence::OpenGLFenceId);
        waitFence.setHandle(QVariant(1200));
        waitFence.setWaitOnCPU(true);
        waitFence.setTimeout(1584);

        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&waitFence);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QWaitFenceData>>(creationChanges.first());
            const Qt3DRender::QWaitFenceData cloneData = creationChangeData->data;

            QCOMPARE(waitFence.handleType(), cloneData.handleType);
            QCOMPARE(waitFence.handle(), cloneData.handle);
            QCOMPARE(waitFence.waitOnCPU(), cloneData.waitOnCPU);
            QCOMPARE(waitFence.timeout(), cloneData.timeout);
            QCOMPARE(waitFence.id(), creationChangeData->subjectId());
            QCOMPARE(waitFence.isEnabled(), true);
            QCOMPARE(waitFence.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(waitFence.metaObject(), creationChangeData->metaObject());
        }

        // WHEN
        waitFence.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&waitFence);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QWaitFenceData>>(creationChanges.first());
            const Qt3DRender::QWaitFenceData cloneData = creationChangeData->data;

            QCOMPARE(waitFence.handleType(), cloneData.handleType);
            QCOMPARE(waitFence.handle(), cloneData.handle);
            QCOMPARE(waitFence.waitOnCPU(), cloneData.waitOnCPU);
            QCOMPARE(waitFence.timeout(), cloneData.timeout);
            QCOMPARE(waitFence.id(), creationChangeData->subjectId());
            QCOMPARE(waitFence.isEnabled(), false);
            QCOMPARE(waitFence.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(waitFence.metaObject(), creationChangeData->metaObject());
        }
    }

    void checkHandleTypeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QWaitFence waitFence;
        arbiter.setArbiterOnNode(&waitFence);

        {
            // WHEN
            waitFence.setHandleType(Qt3DRender::QWaitFence::OpenGLFenceId);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &waitFence);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            waitFence.setHandleType(Qt3DRender::QWaitFence::OpenGLFenceId);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }
    }

    void checkHandleUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QWaitFence waitFence;
        arbiter.setArbiterOnNode(&waitFence);

        {
            // WHEN
            waitFence.setHandle(QVariant(883));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &waitFence);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            waitFence.setHandle(QVariant(883));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }
    }

    void checkWaitOnCPUUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QWaitFence waitFence;
        arbiter.setArbiterOnNode(&waitFence);

        {
            // WHEN
            waitFence.setWaitOnCPU(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &waitFence);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            waitFence.setWaitOnCPU(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.events.size(), 0);
        }
    }

    void checkTimeoutUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QWaitFence waitFence;
        arbiter.setArbiterOnNode(&waitFence);

        {
            // WHEN
            waitFence.setTimeout(quint64(600));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &waitFence);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            waitFence.setTimeout(quint64(600));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }
    }
};

QTEST_MAIN(tst_QWaitFence)

#include "tst_qwaitfence.moc"
