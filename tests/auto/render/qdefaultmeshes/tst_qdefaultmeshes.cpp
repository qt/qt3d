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

#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qgeometryfunctor.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferfunctor.h>

#include <Qt3DRender/qspheremesh.h>
#include <Qt3DRender/qcylindermesh.h>
#include <Qt3DRender/qtorusmesh.h>
#include <Qt3DRender/qcuboidmesh.h>
#include <Qt3DRender/qplanemesh.h>

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QDefaultMeshes: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QDefaultMeshes()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QGeometryRenderer *>("geomRenderer");
        QTest::newRow("QSphereMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DRender::QSphereMesh);
        QTest::newRow("QCylinderMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DRender::QCylinderMesh);
        QTest::newRow("QTorusMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DRender::QTorusMesh);
        QTest::newRow("QCuboidMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DRender::QCuboidMesh);
        QTest::newRow("QPlaneMesh") << static_cast<Qt3DRender::QGeometryRenderer *>(new Qt3DRender::QPlaneMesh);
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
        QCOMPARE(clone->primitiveCount(), geomRenderer->primitiveCount());
        QCOMPARE(clone->baseVertex(), geomRenderer->baseVertex());
        QCOMPARE(clone->baseInstance(), geomRenderer->baseInstance());
        QCOMPARE(clone->restartIndex(), geomRenderer->restartIndex());
        QCOMPARE(clone->primitiveRestart(), geomRenderer->primitiveRestart());
        QCOMPARE(clone->primitiveType(), geomRenderer->primitiveType());

        QCOMPARE(clone->geometryFunctor(), geomRenderer->geometryFunctor());
        if (geomRenderer->geometryFunctor()) {
            QVERIFY(clone->geometryFunctor());
            QVERIFY(*clone->geometryFunctor() == *geomRenderer->geometryFunctor());
        }

        if (geomRenderer->geometry() != Q_NULLPTR) {
            QVERIFY(clone->geometry() != Q_NULLPTR);
            QCOMPARE(clone->geometry()->id(), geomRenderer->geometry()->id());

            const Qt3DRender::QGeometry *geometry = geomRenderer->geometry();
            const Qt3DRender::QGeometry *clonedGeometry = clone->geometry();

            QCOMPARE(clonedGeometry->attributes().count(), geometry->attributes().count());
            QCOMPARE(geometry->verticesPerPatch(), clonedGeometry->verticesPerPatch());

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
                QCOMPARE(buffer->bufferFunctor(), clonedBuffer->bufferFunctor());
                QCOMPARE(buffer->isSync(), clonedBuffer->isSync());
                if (buffer->bufferFunctor()) {
                    QVERIFY(clonedBuffer->bufferFunctor());
                    QVERIFY(*clonedBuffer->bufferFunctor() == *buffer->bufferFunctor());
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
