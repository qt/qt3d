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

#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/private/qgeometry_p.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>

#include <QSignalSpy>

#include "testarbiter.h"

class FakeGeometry : public Qt3DRender::QGeometry
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
        QScopedPointer<Qt3DRender::QGeometry> geometry(new Qt3DRender::QGeometry());
        arbiter.setArbiterOnNode(geometry.data());

        // WHEN
        Qt3DRender::QAttribute attr;
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
        auto dNode = static_cast<Qt3DRender::QGeometryPrivate *>(Qt3DCore::QNodePrivate::get(geometry.data()));
        const QVector3D minExt(-1.0f, -1.0f, -1.0f);
        const QVector3D maxExt(1.0f, 1.0f, 1.0f);
        dNode->setExtent(minExt, maxExt);

        // THEN
        QCOMPARE(spyMinExtent.count(), 1);
        QCOMPARE(spyMaxExtent.count(), 1);
        QCOMPARE(geometry->minExtent(), minExt);
        QCOMPARE(geometry->maxExtent(), maxExt);

        spyMaxExtent.clear();
        spyMinExtent.clear();

        // WHEN
        dNode->setExtent(minExt, maxExt);

        // THEN
        QCOMPARE(spyMinExtent.count(), 0);
        QCOMPARE(spyMaxExtent.count(), 0);
    }
};

QTEST_MAIN(tst_QGeometry)

#include "tst_qgeometry.moc"
