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
#include <Qt3DRenderer/private/renderbuffer_p.h>
#include <Qt3DCore/qscenepropertychange.h>

class TestFunctor : public Qt3D::QBufferFunctor
{
public:
    explicit TestFunctor(int size)
        : m_size(size)
    {}

    QByteArray operator ()() Q_DECL_FINAL
    {
        return QByteArray();
    }

    bool operator ==(const Qt3D::QBufferFunctor &other) const
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
        Qt3D::Render::RenderBuffer renderBuffer;
        Qt3D::QBuffer buffer(Qt3D::QBuffer::IndexBuffer);
        buffer.setUsage(Qt3D::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("Corvette"));
        buffer.setBufferFunctor(Qt3D::QBufferFunctorPtr(new TestFunctor(883)));

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
        Qt3D::Render::RenderBuffer renderBuffer;

        // THEN
        QCOMPARE(renderBuffer.isDirty(), false);
        QCOMPARE(renderBuffer.type(), Qt3D::QBuffer::VertexBuffer);
        QCOMPARE(renderBuffer.usage(), Qt3D::QBuffer::StaticDraw);
        QVERIFY(renderBuffer.data().isEmpty());
        QVERIFY(renderBuffer.peerUuid().isNull());
        QVERIFY(renderBuffer.bufferFunctor().isNull());

        // GIVEN
        Qt3D::QBuffer buffer(Qt3D::QBuffer::IndexBuffer);
        buffer.setUsage(Qt3D::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("C7"));
        buffer.setBufferFunctor(Qt3D::QBufferFunctorPtr(new TestFunctor(73)));

        // WHEN
        renderBuffer.updateFromPeer(&buffer);
        renderBuffer.cleanup();

        // THEN
        QCOMPARE(renderBuffer.isDirty(), false);
        QCOMPARE(renderBuffer.type(), Qt3D::QBuffer::VertexBuffer);
        QCOMPARE(renderBuffer.usage(), Qt3D::QBuffer::StaticDraw);
        QVERIFY(renderBuffer.data().isEmpty());
        QVERIFY(renderBuffer.bufferFunctor().isNull());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3D::Render::RenderBuffer renderBuffer;

        // THEN
        QVERIFY(renderBuffer.type() != Qt3D::QBuffer::IndexBuffer);
        QVERIFY(renderBuffer.data().isEmpty());
        QVERIFY(renderBuffer.usage() != Qt3D::QBuffer::DynamicRead);
        QVERIFY(!renderBuffer.isDirty());

        // WHEN
        Qt3D::QScenePropertyChangePtr updateChange(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(static_cast<int>(Qt3D::QBuffer::IndexBuffer));
        updateChange->setPropertyName("type");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.type(), Qt3D::QBuffer::IndexBuffer);
        QVERIFY(renderBuffer.isDirty());

        renderBuffer.unsetDirty();
        QVERIFY(!renderBuffer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(static_cast<int>(Qt3D::QBuffer::DynamicRead));
        updateChange->setPropertyName("usage");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.usage(), Qt3D::QBuffer::DynamicRead);
        QVERIFY(renderBuffer.isDirty());

        renderBuffer.unsetDirty();
        QVERIFY(!renderBuffer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(QByteArrayLiteral("LS9"));
        updateChange->setPropertyName("data");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.data(), QByteArrayLiteral("LS9"));
        QVERIFY(renderBuffer.isDirty());

        renderBuffer.unsetDirty();

        QVERIFY(!renderBuffer.isDirty());


        // WHEN
        Qt3D::QBufferFunctorPtr functor(new TestFunctor(355));
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(QVariant::fromValue(functor));
        updateChange->setPropertyName("bufferFunctor");
        renderBuffer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderBuffer.bufferFunctor(), functor);
        QVERIFY(renderBuffer.isDirty());

        renderBuffer.unsetDirty();
        QVERIFY(!renderBuffer.isDirty());
    }
};


QTEST_APPLESS_MAIN(tst_RenderBuffer)

#include "tst_renderbuffer.moc"
