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

#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qboundingvolumespecifier.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QGeometry: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QGeometry()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QGeometry *>("geometry");
        QTest::addColumn<int>("attributeCount");

        Qt3DRender::QGeometry *defaultConstructed = new Qt3DRender::QGeometry();
        QTest::newRow("defaultConstructed") << defaultConstructed << 0;

        Qt3DRender::QGeometry *geometry1 = new Qt3DRender::QGeometry();
        geometry1->setVerticesPerPatch(2);
        Qt3DRender::QAttribute *attribute = new Qt3DRender::QAttribute(Q_NULLPTR, QStringLiteral("Attr1"), Qt3DRender::QAttribute::Float, 4, 454);
        geometry1->addAttribute(attribute);
        geometry1->addAttribute(new Qt3DRender::QAttribute(Q_NULLPTR, QStringLiteral("Attr2"), Qt3DRender::QAttribute::Float, 4, 555));
        geometry1->boundingVolumeSpecifier()->setPositionAttribute(attribute);
        QTest::newRow("2 attributes") << geometry1 << 2;


        Qt3DRender::QGeometry *geometry2 = new Qt3DRender::QGeometry();
        attribute = new Qt3DRender::QAttribute(Q_NULLPTR, QStringLiteral("Attr2"), Qt3DRender::QAttribute::Float, 4, 383);
        geometry2->addAttribute(new Qt3DRender::QAttribute(Q_NULLPTR, QStringLiteral("Attr1"), Qt3DRender::QAttribute::Float, 3, 427));
        geometry2->addAttribute(attribute);
        geometry2->addAttribute(new Qt3DRender::QAttribute(Q_NULLPTR, QStringLiteral("Attr3"), Qt3DRender::QAttribute::Float, 2, 327));
        geometry2->removeAttribute(attribute);
        geometry2->setVerticesPerPatch(3);
        QTest::newRow("3 - 1 attributes") << geometry2 << 2;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QGeometry *, geometry);
        QFETCH(int, attributeCount);

        // WHEN
        Qt3DRender::QGeometry *clone = static_cast<Qt3DRender::QGeometry *>(QNode::clone(geometry));

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(geometry->id(), clone->id());
        QCOMPARE(attributeCount, geometry->attributes().count());
        QCOMPARE(attributeCount, clone->attributes().count());
        QCOMPARE(geometry->verticesPerPatch(), clone->verticesPerPatch());
        if (geometry->boundingVolumeSpecifier()->positionAttribute())
                QCOMPARE(geometry->boundingVolumeSpecifier()->positionAttribute()->id(), clone->boundingVolumeSpecifier()->positionAttribute()->id());

        for (int i = 0; i < attributeCount; ++i) {
            Qt3DRender::QAttribute *originalAttribute = static_cast<Qt3DRender::QAttribute *>(geometry->attributes()[i]);
            Qt3DRender::QAttribute *cloneAttribute = static_cast<Qt3DRender::QAttribute *>(clone->attributes()[i]);

            QCOMPARE(originalAttribute->id(), cloneAttribute->id());
            QCOMPARE(originalAttribute->name(), cloneAttribute->name());
            QCOMPARE(originalAttribute->buffer(), cloneAttribute->buffer());
            QCOMPARE(originalAttribute->count(), cloneAttribute->count());
            QCOMPARE(originalAttribute->byteStride(), cloneAttribute->byteStride());
            QCOMPARE(originalAttribute->byteOffset(), cloneAttribute->byteOffset());
            QCOMPARE(originalAttribute->divisor(), cloneAttribute->divisor());
            QCOMPARE(originalAttribute->attributeType(), cloneAttribute->attributeType());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QGeometry> geometry(new Qt3DRender::QGeometry());
        TestArbiter arbiter(geometry.data());

        // WHEN
        Qt3DRender::QAttribute attr;
        geometry->addAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "attribute");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), attr.id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        geometry->addAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        geometry->removeAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "attribute");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), attr.id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();

        // WHEN
        geometry->setVerticesPerPatch(2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "verticesPerPatch");
        QCOMPARE(change->value().toInt(), 2);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        geometry->boundingVolumeSpecifier()->setPositionAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "boundingVolumeSpecifierPositionAttribute");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), attr.id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QGeometry)

#include "tst_qgeometry.moc"
