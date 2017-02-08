/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DPhysics/qcuboidcollisionshape.h>
#include <Qt3DPhysics/private/qcuboidcollisionshape_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>

#include "testpostmanarbiter.h"

class tst_QCuboidCollisionShape: public QObject
{
    Q_OBJECT
public:
    tst_QCuboidCollisionShape()
    {
    }

private Q_SLOTS:
    void checkCreationData()
    {
        // GIVEN
        Qt3DPhysics::QCuboidCollisionShape shape;
        shape.setXExtent(0.5f);
        shape.setYExtent(1.5f);
        shape.setZExtent(2.5f);

        // WHEN
        Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&shape);
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges = creationChangeGenerator.creationChanges();

        // THEN
        QCOMPARE(creationChanges.size(), 1);

        const Qt3DCore::QNodeCreatedChangePtr<Qt3DPhysics::QCuboidCollisionShapeData> creationChangeData =
               qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DPhysics::QCuboidCollisionShapeData>>(creationChanges.first());
        const Qt3DPhysics::QCuboidCollisionShapeData &creationData = creationChangeData->data;

        // THEN
        QCOMPARE(creationChangeData->subjectId(), shape.id());
        QCOMPARE(creationChangeData->isNodeEnabled(), shape.isEnabled());
        QCOMPARE(creationChangeData->metaObject(), shape.metaObject());
        QCOMPARE(creationChangeData->parentId(), shape.parentNode() ? shape.parentNode()->id() : Qt3DCore::QNodeId());
        QCOMPARE(creationData.xExtent, shape.xExtent());
        QCOMPARE(creationData.yExtent, shape.yExtent());
        QCOMPARE(creationData.zExtent, shape.zExtent());
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DPhysics::QCuboidCollisionShape> shape(new Qt3DPhysics::QCuboidCollisionShape());
        arbiter.setArbiterOnNode(shape.data());

        // WHEN
        shape->setXExtent(1.5f);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QPropertyUpdatedChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "xExtent");
        QCOMPARE(change->value().toFloat(), 1.5f);
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        shape->setYExtent(2.0f);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "yExtent");
        QCOMPARE(change->value().toFloat(), 2.0f);
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        shape->setZExtent(3.0f);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "zExtent");
        QCOMPARE(change->value().toFloat(), 3.0f);
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();
    }
};

QTEST_MAIN(tst_QCuboidCollisionShape)

#include "tst_qcuboidcollisionshape.moc"
