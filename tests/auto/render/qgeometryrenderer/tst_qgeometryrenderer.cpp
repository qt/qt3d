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

#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DCore/qgeometry.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DRender/private/qgeometryrenderer_p.h>

#include "testarbiter.h"

class tst_QGeometryRenderer: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QGeometryRenderer> geometryRenderer(new Qt3DRender::QGeometryRenderer());
        arbiter.setArbiterOnNode(geometryRenderer.data());

        // WHEN
        geometryRenderer->setInstanceCount(256);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setVertexCount(1340);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setIndexOffset(883);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setFirstInstance(1200);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setIndexBufferByteOffset(91);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setRestartIndexValue(65535);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setVerticesPerPatch(2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setPrimitiveRestartEnabled(true);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Patches);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        Qt3DCore::QGeometry geom;
        geometryRenderer->setGeometry(&geom);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        Qt3DCore::QGeometry geom2;
        geometryRenderer->setGeometry(&geom2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();

        // WHEN
        geometryRenderer->setSortIndex(42.f);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), geometryRenderer.data());

        arbiter.clear();
    }

    void checkGeometryBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QGeometryRenderer> geometryRenderer(new Qt3DRender::QGeometryRenderer);
        {
            // WHEN
            Qt3DCore::QGeometry geometry;
            geometryRenderer->setGeometry(&geometry);

            // THEN
            QCOMPARE(geometry.parent(), geometryRenderer.data());
            QCOMPARE(geometryRenderer->geometry(), &geometry);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(geometryRenderer->geometry() == nullptr);

        {
            // WHEN
            Qt3DRender::QGeometryRenderer someOtherGeometryRenderer;
            QScopedPointer<Qt3DCore::QGeometry> geometry(new Qt3DCore::QGeometry(&someOtherGeometryRenderer));
            geometryRenderer->setGeometry(geometry.data());

            // THEN
            QCOMPARE(geometry->parent(), &someOtherGeometryRenderer);
            QCOMPARE(geometryRenderer->geometry(), geometry.data());

            // WHEN
            geometryRenderer.reset();
            geometry.reset();

            // THEN Should not crash when the geometry is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QGeometryRenderer)

#include "tst_qgeometryrenderer.moc"
