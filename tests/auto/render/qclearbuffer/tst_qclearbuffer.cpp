/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qclearbuffer.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QClearBuffer: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QClearBuffer()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QClearBuffer *>("clearBuffer");
        QTest::addColumn<Qt3DRender::QClearBuffer::BufferType>("bufferType");

        Qt3DRender::QClearBuffer *defaultConstructed = new Qt3DRender::QClearBuffer();
        QTest::newRow("defaultConstructed") << defaultConstructed << Qt3DRender::QClearBuffer::None;

        Qt3DRender::QClearBuffer *allBuffers = new Qt3DRender::QClearBuffer();
        allBuffers->setBuffers(Qt3DRender::QClearBuffer::AllBuffers);
        QTest::newRow("allBuffers") << allBuffers << Qt3DRender::QClearBuffer::AllBuffers;

        Qt3DRender::QClearBuffer *depthBuffer = new Qt3DRender::QClearBuffer();
        depthBuffer->setBuffers(Qt3DRender::QClearBuffer::DepthBuffer);
        QTest::newRow("depthBuffer") << depthBuffer << Qt3DRender::QClearBuffer::DepthBuffer;

        Qt3DRender::QClearBuffer *colorDepthBuffer = new Qt3DRender::QClearBuffer();
        colorDepthBuffer->setBuffers(Qt3DRender::QClearBuffer::ColorDepthBuffer);
        QTest::newRow("colorDepthBuffer") << colorDepthBuffer << Qt3DRender::QClearBuffer::ColorDepthBuffer;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QClearBuffer *, clearBuffer);
        QFETCH(Qt3DRender::QClearBuffer::BufferType, bufferType);

        // THEN
        QCOMPARE(clearBuffer->buffers(), bufferType);

        // WHEN
        Qt3DRender::QClearBuffer *clone = static_cast<Qt3DRender::QClearBuffer *>(QNode::clone(clearBuffer));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(clearBuffer->id(), clone->id());
        QCOMPARE(clearBuffer->buffers(), clone->buffers());

        delete clearBuffer;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QClearBuffer> clearBuffer(new Qt3DRender::QClearBuffer());
        TestArbiter arbiter(clearBuffer.data());

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffer::AllBuffers);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "buffers");
        QCOMPARE(change->subjectId(), clearBuffer->id());
        QCOMPARE(change->value().value<Qt3DRender::QClearBuffer::BufferType>(), Qt3DRender::QClearBuffer::AllBuffers);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffer::AllBuffers);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffer::ColorDepthBuffer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "buffers");
        QCOMPARE(change->subjectId(), clearBuffer->id());
        QCOMPARE(change->value().value<Qt3DRender::QClearBuffer::BufferType>(), Qt3DRender::QClearBuffer::ColorDepthBuffer);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QClearBuffer)

#include "tst_qclearbuffer.moc"
