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
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/qboundingvolumespecifier.h>

class tst_RenderGeometry : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DRender::Render::Geometry renderGeometry;

        Qt3DRender::QGeometry geometry;
        Qt3DRender::QAttribute attr1;
        Qt3DRender::QAttribute attr2;
        Qt3DRender::QAttribute attr4;
        Qt3DRender::QAttribute attr3;

        geometry.addAttribute(&attr1);
        geometry.addAttribute(&attr2);
        geometry.addAttribute(&attr3);
        geometry.addAttribute(&attr4);
        geometry.setVerticesPerPatch(4);
        geometry.boundingVolumeSpecifier()->setPositionAttribute(&attr1);

        // WHEN
        renderGeometry.setPeer(&geometry);

        // THEN
        QCOMPARE(renderGeometry.peerUuid(), geometry.id());
        QCOMPARE(renderGeometry.isDirty(), true);
        QCOMPARE(renderGeometry.attributes().count(), 4);
        QCOMPARE(renderGeometry.verticesPerPatch(), 4);
        QCOMPARE(renderGeometry.boundingPositionAttribute(), attr1.id());

        for (int i = 0; i < 4; ++i)
            QCOMPARE(geometry.attributes().at(i)->id(), renderGeometry.attributes().at(i));
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::Geometry renderGeometry;

        // THEN
        QCOMPARE(renderGeometry.isDirty(), false);
        QVERIFY(renderGeometry.attributes().isEmpty());
        QVERIFY(renderGeometry.peerUuid().isNull());
        QCOMPARE(renderGeometry.verticesPerPatch(), 0);
        QCOMPARE(renderGeometry.boundingPositionAttribute(), Qt3DCore::QNodeId());

        // GIVEN
        Qt3DRender::QGeometry geometry;
        Qt3DRender::QAttribute attr1;
        Qt3DRender::QAttribute attr2;
        Qt3DRender::QAttribute attr4;
        Qt3DRender::QAttribute attr3;
        geometry.boundingVolumeSpecifier()->setPositionAttribute(&attr1);

        geometry.addAttribute(&attr1);
        geometry.addAttribute(&attr2);
        geometry.addAttribute(&attr3);
        geometry.addAttribute(&attr4);
        geometry.setVerticesPerPatch(4);

        // WHEN
        renderGeometry.updateFromPeer(&geometry);
        renderGeometry.cleanup();

        // THEN
        QCOMPARE(renderGeometry.isDirty(), false);
        QVERIFY(renderGeometry.attributes().isEmpty());
        QCOMPARE(renderGeometry.verticesPerPatch(), 0);
        QCOMPARE(renderGeometry.boundingPositionAttribute(), Qt3DCore::QNodeId());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::Render::Geometry renderGeometry;
        Qt3DCore::QNodeId geometryId = Qt3DCore::QNodeId::createId();

        // WHEN
        Qt3DCore::QScenePropertyChangePtr updateChange(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeAdded, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QVariant::fromValue(geometryId));
        updateChange->setPropertyName("attribute");
        renderGeometry.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometry.attributes().count(), 1);
        QVERIFY(renderGeometry.isDirty());

        renderGeometry.unsetDirty();
        QVERIFY(!renderGeometry.isDirty());

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeRemoved, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QVariant::fromValue(geometryId));
        updateChange->setPropertyName("attribute");
        renderGeometry.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometry.attributes().count(), 0);
        QVERIFY(renderGeometry.isDirty());

        renderGeometry.unsetDirty();
        QVERIFY(!renderGeometry.isDirty());

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QVariant::fromValue(3));
        updateChange->setPropertyName("verticesPerPatch");
        renderGeometry.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometry.verticesPerPatch(), 3);
        QVERIFY(!renderGeometry.isDirty());

        // WHEN
        const Qt3DCore::QNodeId boundingAttrId = Qt3DCore::QNodeId::createId();
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QVariant::fromValue(boundingAttrId));
        updateChange->setPropertyName("boundingVolumeSpecifierPositionAttribute");
        renderGeometry.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometry.boundingPositionAttribute(), boundingAttrId);
        QVERIFY(!renderGeometry.isDirty());
    }
};


QTEST_APPLESS_MAIN(tst_RenderGeometry)

#include "tst_geometry.moc"
