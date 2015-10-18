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
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testpostmanarbiter.h"

class TestFunctor : public Qt3DRender::QBufferFunctor
{
public:
    explicit TestFunctor(int size)
        : m_size(size)
    {}

    QByteArray operator ()() Q_DECL_FINAL
    {
        return QByteArrayLiteral("454");
    }

    bool operator ==(const Qt3DRender::QBufferFunctor &other) const
    {
        const TestFunctor *otherFunctor = functor_cast<TestFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return otherFunctor->m_size == m_size;
        return false;
    }

    QT3D_FUNCTOR(TestFunctor)

private:
    int m_size;
};

class tst_RenderBuffer : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DRender::Render::Buffer renderBuffer;
        Qt3DRender::QBuffer buffer(Qt3DRender::QBuffer::IndexBuffer);
        buffer.setUsage(Qt3DRender::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("Corvette"));
        buffer.setBufferFunctor(Qt3DRender::QBufferFunctorPtr(new TestFunctor(883)));

        // WHEN
        renderBuffer.setPeer(&buffer);

        // THEN
        QCOMPARE(renderBuffer.peerUuid(), buffer.id());
        QCOMPARE(renderBuffer.isDirty(), true);
        QCOMPARE(renderBuffer.type(), buffer.type());
        QCOMPARE(renderBuffer.usage(), buffer.usage());
        QCOMPARE(renderBuffer.data(), buffer.data());
        QCOMPARE(renderBuffer.bufferFunctor(), buffer.bufferFunctor());
        QVERIFY(*renderBuffer.bufferFunctor() == *buffer.bufferFunctor());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::Buffer renderBuffer;

        // THEN
        QCOMPARE(renderBuffer.isDirty(), false);
        QCOMPARE(renderBuffer.type(), Qt3DRender::QBuffer::VertexBuffer);
        QCOMPARE(renderBuffer.usage(), Qt3DRender::QBuffer::StaticDraw);
        QVERIFY(renderBuffer.data().isEmpty());
        QVERIFY(renderBuffer.peerUuid().isNull());
        QVERIFY(renderBuffer.bufferFunctor().isNull());

        // GIVEN
        Qt3DRender::QBuffer buffer(Qt3DRender::QBuffer::IndexBuffer);
        buffer.setUsage(Qt3DRender::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("C7"));
        buffer.setBufferFunctor(Qt3DRender::QBufferFunctorPtr(new TestFunctor(73)));

        // WHEN
        renderBuffer.updateFromPeer(&buffer);
        renderBuffer.cleanup();

        // THEN
        QCOMPARE(renderBuffer.isDirty(), false);
        QCOMPARE(renderBuffer.type(), Qt3DRender::QBuffer::VertexBuffer);
        QCOMPARE(renderBuffer.usage(), Qt3DRender::QBuffer::StaticDraw);
        QVERIFY(renderBuffer.data().isEmpty());
        QVERIFY(renderBuffer.bufferFunctor().isNull());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::Render::Buffer renderBuffer;

        // THEN
        QVERIFY(renderBuffer.type() != Qt3DRender::QBuffer::IndexBuffer);
        QVERIFY(renderBuffer.data().isEmpty());
        QVERIFY(renderBuffer.usage() != Qt3DRender::QBuffer::DynamicRead);
        QVERIFY(!renderBuffer.isDirty());

        // WHEN
        Qt3DCore::QScenePropertyChangePtr updateChange(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(static_cast<int>(Qt3DRender::QBuffer::IndexBuffer));
        updateChange->setPropertyName("type");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.type(), Qt3DRender::QBuffer::IndexBuffer);
        QVERIFY(renderBuffer.isDirty());

        renderBuffer.unsetDirty();
        QVERIFY(!renderBuffer.isDirty());

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(static_cast<int>(Qt3DRender::QBuffer::DynamicRead));
        updateChange->setPropertyName("usage");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.usage(), Qt3DRender::QBuffer::DynamicRead);
        QVERIFY(renderBuffer.isDirty());

        renderBuffer.unsetDirty();
        QVERIFY(!renderBuffer.isDirty());

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QByteArrayLiteral("LS9"));
        updateChange->setPropertyName("data");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.data(), QByteArrayLiteral("LS9"));
        QVERIFY(renderBuffer.isDirty());

        renderBuffer.unsetDirty();

        QVERIFY(!renderBuffer.isDirty());


        // WHEN
        Qt3DRender::QBufferFunctorPtr functor(new TestFunctor(355));
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QVariant::fromValue(functor));
        updateChange->setPropertyName("bufferFunctor");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.bufferFunctor(), functor);
        QVERIFY(renderBuffer.isDirty());

        renderBuffer.unsetDirty();
        QVERIFY(!renderBuffer.isDirty());

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(true);
        updateChange->setPropertyName("sync");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.isSync(), true);
        QVERIFY(!renderBuffer.isDirty());

        // WHEN
        TestArbiter arbiter;
        Qt3DCore::QBackendNodePrivate::get(&renderBuffer)->setArbiter(&arbiter);
        renderBuffer.executeFunctor();

        // THEN
        QCOMPARE(arbiter.events.count(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "data");
        QCOMPARE(change->value().toByteArray(), QByteArrayLiteral("454"));

    }
};


QTEST_APPLESS_MAIN(tst_RenderBuffer)

#include "tst_buffer.moc"
