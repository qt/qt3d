// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DCore/qgeometry.h>
#include <Qt3DCore/private/qgeometry_p.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>

#include <QSignalSpy>

#include "testarbiter.h"

class FakeGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
};

class tst_QGeometry: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DCore::QGeometry> geometry(new Qt3DCore::QGeometry());
        arbiter.setArbiterOnNode(geometry.data());

        // WHEN
        Qt3DCore::QAttribute attr;
        geometry->addAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometry.data());

        arbiter.clear();

        // WHEN
        geometry->addAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        geometry->removeAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometry.data());
    }

    void checkAttributeBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QGeometry> geometry(new Qt3DCore::QGeometry);
        {
            // WHEN
            Qt3DCore::QAttribute attribute;
            geometry->addAttribute(&attribute);

            // THEN
            QCOMPARE(attribute.parent(), geometry.data());
            QCOMPARE(geometry->attributes().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(geometry->attributes().isEmpty());

        {
            // WHEN
            Qt3DCore::QGeometry someOtherGeometry;
            QScopedPointer<Qt3DCore::QAttribute> attribute(new Qt3DCore::QAttribute(&someOtherGeometry));
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
        auto dNode = static_cast<Qt3DCore::QGeometryPrivate *>(Qt3DCore::QNodePrivate::get(geometry.data()));
        const QVector3D minExt(-1.0f, -1.0f, -1.0f);
        const QVector3D maxExt(1.0f, 1.0f, 1.0f);
        dNode->setExtent(minExt, maxExt);

        // THEN
        QCOMPARE(spyMinExtent.size(), 1);
        QCOMPARE(spyMaxExtent.size(), 1);
        QCOMPARE(geometry->minExtent(), minExt);
        QCOMPARE(geometry->maxExtent(), maxExt);

        spyMaxExtent.clear();
        spyMinExtent.clear();

        // WHEN
        dNode->setExtent(minExt, maxExt);

        // THEN
        QCOMPARE(spyMinExtent.size(), 0);
        QCOMPARE(spyMaxExtent.size(), 0);
    }
};

QTEST_MAIN(tst_QGeometry)

#include "tst_qgeometry.moc"
