/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qclearbuffers.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QClearBuffers: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QClearBuffers()
    {
        QMetaObject::invokeMethod(this, "_q_cleanup", Qt::DirectConnection);
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QClearBuffers *>("clearBuffers");
        QTest::addColumn<Qt3DRender::QClearBuffers::BufferType>("bufferType");

        Qt3DRender::QClearBuffers *defaultConstructed = new Qt3DRender::QClearBuffers();
        QTest::newRow("defaultConstructed") << defaultConstructed << Qt3DRender::QClearBuffers::None;

        Qt3DRender::QClearBuffers *allBuffers = new Qt3DRender::QClearBuffers();
        allBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
        QTest::newRow("allBuffers") << allBuffers << Qt3DRender::QClearBuffers::AllBuffers;

        Qt3DRender::QClearBuffers *depthBuffer = new Qt3DRender::QClearBuffers();
        depthBuffer->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
        QTest::newRow("depthBuffer") << depthBuffer << Qt3DRender::QClearBuffers::DepthBuffer;

        Qt3DRender::QClearBuffers *colorDepthBuffer = new Qt3DRender::QClearBuffers();
        colorDepthBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
        QTest::newRow("colorDepthBuffer") << colorDepthBuffer << Qt3DRender::QClearBuffers::ColorDepthBuffer;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QClearBuffers *, clearBuffers);
        QFETCH(Qt3DRender::QClearBuffers::BufferType, bufferType);

        // THEN
        QCOMPARE(clearBuffers->buffers(), bufferType);

        // WHEN
        Qt3DRender::QClearBuffers *clone = static_cast<Qt3DRender::QClearBuffers *>(QNode::clone(clearBuffers));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(clearBuffers->id(), clone->id());
        QCOMPARE(clearBuffers->buffers(), clone->buffers());

        delete clearBuffers;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QClearBuffers> clearBuffer(new Qt3DRender::QClearBuffers());
        TestArbiter arbiter(clearBuffer.data());

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QNodePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "buffers");
        QCOMPARE(change->subjectId(), clearBuffer->id());
        QCOMPARE(change->value().value<Qt3DRender::QClearBuffers::BufferType>(), Qt3DRender::QClearBuffers::AllBuffers);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "buffers");
        QCOMPARE(change->subjectId(), clearBuffer->id());
        QCOMPARE(change->value().value<Qt3DRender::QClearBuffers::BufferType>(), Qt3DRender::QClearBuffers::ColorDepthBuffer);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QClearBuffers)

#include "tst_qclearbuffers.moc"
