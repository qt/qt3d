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

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>

#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/private/qgeometry_p.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>

#include <QSignalSpy>

#include "testpostmanarbiter.h"

class FakeGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT

public:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override
    {
        Qt3DRender::QGeometry::sceneChangeEvent(change);
    }
};

class tst_QGeometry: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QGeometry *>("geometry");
        QTest::addColumn<int>("attributeCount");

        Qt3DRender::QGeometry *defaultConstructed = new Qt3DRender::QGeometry();
        QTest::newRow("defaultConstructed") << defaultConstructed << 0;

        Qt3DRender::QGeometry *geometry1 = new Qt3DRender::QGeometry();
        Qt3DRender::QAttribute *attribute = new Qt3DRender::QAttribute(nullptr, QStringLiteral("Attr1"), Qt3DRender::QAttribute::Float, 4, 454);
        geometry1->addAttribute(attribute);
        geometry1->addAttribute(new Qt3DRender::QAttribute(nullptr, QStringLiteral("Attr2"), Qt3DRender::QAttribute::Float, 4, 555));
        geometry1->setBoundingVolumePositionAttribute(attribute);
        QTest::newRow("2 attributes") << geometry1 << 2;


        Qt3DRender::QGeometry *geometry2 = new Qt3DRender::QGeometry();
        attribute = new Qt3DRender::QAttribute(nullptr, QStringLiteral("Attr2"), Qt3DRender::QAttribute::Float, 4, 383);
        geometry2->addAttribute(new Qt3DRender::QAttribute(nullptr, QStringLiteral("Attr1"), Qt3DRender::QAttribute::Float, 3, 427));
        geometry2->addAttribute(attribute);
        geometry2->addAttribute(new Qt3DRender::QAttribute(nullptr, QStringLiteral("Attr3"), Qt3DRender::QAttribute::Float, 2, 327));
        geometry2->removeAttribute(attribute);
        QTest::newRow("3 - 1 attributes") << geometry2 << 2;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QGeometry *, geometry);
        QFETCH(int, attributeCount);

        // WHEN
        Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(geometry);
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges = creationChangeGenerator.creationChanges();

        // THEN
        QCOMPARE(creationChanges.size(), 1 + geometry->childNodes().size());

        const Qt3DCore::QNodeCreatedChangePtr<Qt3DRender::QGeometryData> creationChangeData =
                qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QGeometryData>>(creationChanges.first());
        const Qt3DRender::QGeometryData &cloneData = creationChangeData->data;

        QCOMPARE(geometry->id(), creationChangeData->subjectId());
        QCOMPARE(geometry->isEnabled(), creationChangeData->isNodeEnabled());
        QCOMPARE(geometry->metaObject(), creationChangeData->metaObject());

        QCOMPARE(attributeCount, geometry->attributes().count());
        QCOMPARE(attributeCount, cloneData.attributeIds.count());
        if (geometry->boundingVolumePositionAttribute())
                QCOMPARE(geometry->boundingVolumePositionAttribute()->id(), cloneData.boundingVolumePositionAttributeId);

        for (int i = 0; i < attributeCount; ++i) {
            Qt3DRender::QAttribute *originalAttribute = static_cast<Qt3DRender::QAttribute *>(geometry->attributes()[i]);
            QCOMPARE(originalAttribute->id(), cloneData.attributeIds.at(i));
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QGeometry> geometry(new Qt3DRender::QGeometry());
        arbiter.setArbiterOnNode(geometry.data());

        // WHEN
        Qt3DRender::QAttribute attr;
        geometry->addAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);
        QCOMPARE(arbiter.dirtyNodes.size(), 1);
        QCOMPARE(arbiter.dirtyNodes.front(), geometry.data());

        arbiter.dirtyNodes.clear();

        // WHEN
        geometry->addAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);
        QCOMPARE(arbiter.dirtyNodes.size(), 0);

        // WHEN
        geometry->removeAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);
        QCOMPARE(arbiter.dirtyNodes.size(), 1);
        QCOMPARE(arbiter.dirtyNodes.front(), geometry.data());

        arbiter.events.clear();
    }

    void checkAttributeBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QGeometry> geometry(new Qt3DRender::QGeometry);
        {
            // WHEN
            Qt3DRender::QAttribute attribute;
            geometry->addAttribute(&attribute);

            // THEN
            QCOMPARE(attribute.parent(), geometry.data());
            QCOMPARE(geometry->attributes().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(geometry->attributes().isEmpty());

        {
            // WHEN
            Qt3DRender::QGeometry someOtherGeometry;
            QScopedPointer<Qt3DRender::QAttribute> attribute(new Qt3DRender::QAttribute(&someOtherGeometry));
            geometry->addAttribute(attribute.data());

            // THEN
            QCOMPARE(attribute->parent(), &someOtherGeometry);
            QCOMPARE(geometry->attributes().size(), 1);

            // WHEN
            geometry.reset();
            attribute.reset();

            // THEN Should not crash when the attribute is destroyed (tests for failed removal of destruction helper)
        }
    }

    void checkExtentUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<FakeGeometry> geometry(new FakeGeometry());
        arbiter.setArbiterOnNode(geometry.data());
        QSignalSpy spyMinExtent(geometry.data(), SIGNAL(minExtentChanged(QVector3D)));
        QSignalSpy spyMaxExtent(geometry.data(), SIGNAL(maxExtentChanged(QVector3D)));

        // THEN
        QVERIFY(spyMinExtent.isValid());
        QVERIFY(spyMaxExtent.isValid());
        QCOMPARE(geometry->minExtent(), QVector3D());
        QCOMPARE(geometry->maxExtent(), QVector3D());

        // WHEN
        Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
        valueChange->setPropertyName("extent");
        valueChange->setValue(QVariant::fromValue(QPair<QVector3D, QVector3D>(QVector3D(10.0f, 10.f, 10.0f),
                                                                              QVector3D())));
        geometry->sceneChangeEvent(valueChange);

        // THEN
        QCOMPARE(spyMinExtent.count(), 1);
        QCOMPARE(spyMaxExtent.count(), 0);
        QCOMPARE(geometry->minExtent(), QVector3D(10.0f, 10.0f, 10.0f));

        spyMinExtent.clear();

        // WHEN
        valueChange->setPropertyName("extent");
        valueChange->setValue(QVariant::fromValue(QPair<QVector3D, QVector3D>(QVector3D(10.0f, 10.f, 10.0f),
                                                                              QVector3D(11.0f, 11.f, 11.0f))));
        geometry->sceneChangeEvent(valueChange);

        // THEN
        QCOMPARE(spyMinExtent.count(), 0);
        QCOMPARE(spyMaxExtent.count(), 1);
        QCOMPARE(geometry->maxExtent(), QVector3D(11.0f, 11.0f, 11.0f));

        spyMaxExtent.clear();
    }
};

QTEST_MAIN(tst_QGeometry)

#include "tst_qgeometry.moc"
