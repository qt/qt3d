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

#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qgeometryfactory.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferdatagenerator.h>

#include <Qt3DExtras/qspheremesh.h>
#include <Qt3DExtras/qcylindermesh.h>
#include <Qt3DExtras/qtorusmesh.h>
#include <Qt3DExtras/qcuboidmesh.h>
#include <Qt3DExtras/qplanemesh.h>

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QDefaultMeshes: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QDefaultMeshes()
    {
        QMetaObject::invokeMethod(this, "_q_cleanup", Qt::DirectConnection);
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QGeometryRenderer *>("geomRenderer");
        QTest::newRow("QSphereMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DExtras::QSphereMesh);
        QTest::newRow("QCylinderMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DExtras::QCylinderMesh);
        QTest::newRow("QTorusMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DExtras::QTorusMesh);
        QTest::newRow("QCuboidMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DExtras::QCuboidMesh);
        QTest::newRow("QPlaneMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DExtras::QPlaneMesh);
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QGeometryRenderer *, geomRenderer);

        // WHEN
        QScopedPointer<Qt3DRender::QGeometryRenderer> clone(static_cast<Qt3DRender::QGeometryRenderer *>(QNode::clone(geomRenderer)));

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(clone->id(), geomRenderer->id());
        QCOMPARE(clone->instanceCount(), geomRenderer->instanceCount());
        QCOMPARE(clone->vertexCount(), geomRenderer->vertexCount());
        QCOMPARE(clone->indexOffset(), geomRenderer->indexOffset());
        QCOMPARE(clone->firstInstance(), geomRenderer->firstInstance());
        QCOMPARE(clone->restartIndexValue(), geomRenderer->restartIndexValue());
        QCOMPARE(clone->primitiveRestartEnabled(), geomRenderer->primitiveRestartEnabled());
        QCOMPARE(clone->primitiveType(), geomRenderer->primitiveType());

        QCOMPARE(clone->geometryFactory(), geomRenderer->geometryFactory());
        if (geomRenderer->geometryFactory()) {
            QVERIFY(clone->geometryFactory());
            QVERIFY(*clone->geometryFactory() == *geomRenderer->geometryFactory());
        }

        if (geomRenderer->geometry() != Q_NULLPTR) {
            QVERIFY(clone->geometry() != Q_NULLPTR);
            QCOMPARE(clone->geometry()->id(), geomRenderer->geometry()->id());

            const Qt3DRender::QGeometry *geometry = geomRenderer->geometry();
            const Qt3DRender::QGeometry *clonedGeometry = clone->geometry();

            QCOMPARE(clonedGeometry->attributes().count(), geometry->attributes().count());

            for (int i = 0; i < geometry->attributes().count(); ++i) {
                const Qt3DRender::QAttribute *originalAttribute = static_cast<Qt3DRender::QAttribute *>(geometry->attributes().at(i));
                const Qt3DRender::QAttribute *cloneAttribute = static_cast<Qt3DRender::QAttribute *>(clonedGeometry->attributes().at(i));

                QCOMPARE(originalAttribute->id(), cloneAttribute->id());
                QCOMPARE(originalAttribute->name(), cloneAttribute->name());
                QCOMPARE(originalAttribute->count(), cloneAttribute->count());
                QCOMPARE(originalAttribute->byteStride(), cloneAttribute->byteStride());
                QCOMPARE(originalAttribute->byteOffset(), cloneAttribute->byteOffset());
                QCOMPARE(originalAttribute->divisor(), cloneAttribute->divisor());
                QCOMPARE(originalAttribute->attributeType(), cloneAttribute->attributeType());

                const Qt3DRender::QBuffer *buffer = originalAttribute->buffer();
                const Qt3DRender::QBuffer *clonedBuffer = cloneAttribute->buffer();
                QCOMPARE(buffer->id(), clonedBuffer->id());
                QCOMPARE(buffer->data(), clonedBuffer->data());
                QCOMPARE(buffer->usage(), clonedBuffer->usage());
                QCOMPARE(buffer->type(), clonedBuffer->type());
                QCOMPARE(buffer->dataGenerator(), clonedBuffer->dataGenerator());
                QCOMPARE(buffer->isSyncData(), clonedBuffer->isSyncData());
                if (buffer->dataGenerator()) {
                    QVERIFY(clonedBuffer->dataGenerator());
                    QVERIFY(*clonedBuffer->dataGenerator() == *buffer->dataGenerator());
                }
            }
        }
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QDefaultMeshes)

#include "tst_qdefaultmeshes.moc"
