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
#include <Qt3DRenderer/private/renderattribute_p.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DCore/qscenepropertychange.h>

class tst_RenderAttribute : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3D::Render::RenderAttribute renderAttribute;

        Qt3D::QAttribute attribute;
        attribute.setAttributeType(Qt3D::QAttribute::IndexAttribute);
        attribute.setByteOffset(1200);
        attribute.setByteStride(883);
        attribute.setCount(427);
        attribute.setDivisor(305);
        attribute.setName(QStringLiteral("C3"));
        attribute.setDataType(Qt3D::QAbstractAttribute::UnsignedShort);
        attribute.setDataSize(3);

        Qt3D::QBuffer buffer(Qt3D::QBuffer::IndexBuffer);
        buffer.setUsage(Qt3D::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("Corvette"));
        attribute.setBuffer(&buffer);

        // WHEN
        renderAttribute.setPeer(&attribute);

        // THEN
        QCOMPARE(renderAttribute.peerUuid(), attribute.id());
        QCOMPARE(renderAttribute.isDirty(), true);
        QCOMPARE(renderAttribute.dataType(), attribute.dataType());
        QCOMPARE(renderAttribute.dataSize(), attribute.dataSize());
        QCOMPARE(renderAttribute.attributeType(), attribute.attributeType());
        QCOMPARE(renderAttribute.byteOffset(), attribute.byteOffset());
        QCOMPARE(renderAttribute.byteStride(), attribute.byteStride());
        QCOMPARE(renderAttribute.count(), attribute.count());
        QCOMPARE(renderAttribute.divisor(), attribute.divisor());
        QCOMPARE(renderAttribute.name(), attribute.name());
        QCOMPARE(renderAttribute.bufferId(), buffer.id());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3D::Render::RenderAttribute renderAttribute;

        // THEN
        QVERIFY(renderAttribute.peerUuid().isNull());
        QVERIFY(renderAttribute.bufferId().isNull());
        QVERIFY(renderAttribute.name().isEmpty());
        QCOMPARE(renderAttribute.isDirty(), false);
        QCOMPARE(renderAttribute.dataType(), Qt3D::QAbstractAttribute::Float);
        QCOMPARE(renderAttribute.dataSize(), 1U);
        QCOMPARE(renderAttribute.attributeType(), Qt3D::QAttribute::VertexAttribute);
        QCOMPARE(renderAttribute.byteOffset(), 0U);
        QCOMPARE(renderAttribute.byteStride(), 0U);
        QCOMPARE(renderAttribute.count(), 0U);
        QCOMPARE(renderAttribute.divisor(), 0U);

        // GIVEN
        Qt3D::QAttribute attribute;
        attribute.setAttributeType(Qt3D::QAttribute::IndexAttribute);
        attribute.setByteOffset(1200);
        attribute.setByteStride(883);
        attribute.setCount(427);
        attribute.setDivisor(305);
        attribute.setName(QStringLiteral("C3"));
        attribute.setDataType(Qt3D::QAbstractAttribute::Double);
        attribute.setDataSize(4);
        Qt3D::QBuffer buffer(Qt3D::QBuffer::IndexBuffer);
        buffer.setUsage(Qt3D::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("C7"));
        attribute.setBuffer(&buffer);

        // WHEN
        renderAttribute.updateFromPeer(&attribute);
        renderAttribute.cleanup();

        // THEN
        QVERIFY(renderAttribute.peerUuid().isNull());
        QVERIFY(renderAttribute.bufferId().isNull());
        QVERIFY(renderAttribute.name().isEmpty());
        QCOMPARE(renderAttribute.isDirty(), false);
        QCOMPARE(renderAttribute.dataType(), Qt3D::QAbstractAttribute::Float);
        QCOMPARE(renderAttribute.dataSize(), 1U);
        QCOMPARE(renderAttribute.attributeType(), Qt3D::QAttribute::VertexAttribute);
        QCOMPARE(renderAttribute.byteOffset(), 0U);
        QCOMPARE(renderAttribute.byteStride(), 0U);
        QCOMPARE(renderAttribute.count(), 0U);
        QCOMPARE(renderAttribute.divisor(), 0U);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3D::Render::RenderAttribute renderAttribute;

        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        Qt3D::QScenePropertyChangePtr updateChange(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(static_cast<int>(Qt3D::QAbstractAttribute::Int));
        updateChange->setPropertyName("dataType");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.dataType(), Qt3D::QAbstractAttribute::Int);
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(3);
        updateChange->setPropertyName("dataSize");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.dataSize(), 3U);
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(static_cast<int>(Qt3D::QAttribute::IndexAttribute));
        updateChange->setPropertyName("attributeType");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.attributeType(), Qt3D::QAttribute::IndexAttribute);
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(1340);
        updateChange->setPropertyName("count");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.count(), 1340U);
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(QStringLiteral("L88"));
        updateChange->setPropertyName("name");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.name(), QStringLiteral("L88"));
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(555);
        updateChange->setPropertyName("byteOffset");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.byteOffset(), 555U);
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(454);
        updateChange->setPropertyName("byteStride");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.byteStride(), 454U);
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(1450);
        updateChange->setPropertyName("divisor");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.divisor(), 1450U);
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeAdded, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        Qt3D::QNodeId bufferId = Qt3D::QNodeId::createId();
        updateChange->setValue(QVariant::fromValue(bufferId));
        updateChange->setPropertyName("buffer");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.bufferId(), bufferId);
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeRemoved, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(QVariant::fromValue(bufferId));
        updateChange->setPropertyName("buffer");
        renderAttribute.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderAttribute.bufferId(), Qt3D::QNodeId());
        QVERIFY(renderAttribute.isDirty());

        renderAttribute.unsetDirty();
        QVERIFY(!renderAttribute.isDirty());
    }
};


QTEST_APPLESS_MAIN(tst_RenderAttribute)

#include "tst_renderattribute.moc"
