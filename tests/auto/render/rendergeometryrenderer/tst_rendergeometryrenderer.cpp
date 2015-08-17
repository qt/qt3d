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
#include <Qt3DRenderer/private/rendergeometryrenderer_p.h>
#include <Qt3DRenderer/qgeometry.h>
#include <Qt3DRenderer/qgeometryfunctor.h>
#include <Qt3DCore/qscenepropertychange.h>

class TestFunctor : public Qt3D::QGeometryFunctor
{
public:
    explicit TestFunctor(int size)
        : m_size(size)
    {}

    Qt3D::QGeometry *operator ()() Q_DECL_FINAL
    {
        return Q_NULLPTR;
    }

    bool operator ==(const Qt3D::QGeometryFunctor &other) const
    {
        const TestFunctor *otherFunctor = functor_cast<TestFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return otherFunctor->m_size == m_size;
        return false;
    }

    QT3D_FUNCTOR(TestFunctor)

private:
    int m_size;
};

class tst_RenderGeometryRenderer : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3D::Render::RenderGeometryRenderer renderGeometryRenderer;
        Qt3D::QGeometryRenderer geometryRenderer;
        Qt3D::QGeometry geometry;
        Qt3D::QGeometryFunctorPtr functor(new TestFunctor(1200));

        geometryRenderer.setInstanceCount(1584);
        geometryRenderer.setPrimitiveCount(1609);
        geometryRenderer.setBaseVertex(750);
        geometryRenderer.setBaseInstance(883);
        geometryRenderer.setRestartIndex(65536);
        geometryRenderer.setPrimitiveRestart(true);
        geometryRenderer.setPrimitiveType(Qt3D::QGeometryRenderer::Patches);
        geometryRenderer.setGeometry(&geometry);
        geometryRenderer.setGeometryFunctor(functor);

        // WHEN
        renderGeometryRenderer.setPeer(&geometryRenderer);

        // THEN
        QCOMPARE(renderGeometryRenderer.peerUuid(), geometryRenderer.id());
        QCOMPARE(renderGeometryRenderer.isDirty(), true);
        QCOMPARE(renderGeometryRenderer.instanceCount(), geometryRenderer.instanceCount());
        QCOMPARE(renderGeometryRenderer.primitiveCount(), geometryRenderer.primitiveCount());
        QCOMPARE(renderGeometryRenderer.baseVertex(), geometryRenderer.baseVertex());
        QCOMPARE(renderGeometryRenderer.baseInstance(), geometryRenderer.baseInstance());
        QCOMPARE(renderGeometryRenderer.restartIndex(), geometryRenderer.restartIndex());
        QCOMPARE(renderGeometryRenderer.primitiveRestart(), geometryRenderer.primitiveRestart());
        QCOMPARE(renderGeometryRenderer.primitiveType(), geometryRenderer.primitiveType());
        QCOMPARE(renderGeometryRenderer.geometryId(), geometry.id());
        QCOMPARE(renderGeometryRenderer.geometryFunctor(), functor);
        QVERIFY(*renderGeometryRenderer.geometryFunctor() == *functor);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3D::Render::RenderGeometryRenderer renderGeometryRenderer;

        // THEN
        QVERIFY(renderGeometryRenderer.peerUuid().isNull());
        QVERIFY(renderGeometryRenderer.geometryId().isNull());
        QCOMPARE(renderGeometryRenderer.isDirty(), false);
        QCOMPARE(renderGeometryRenderer.instanceCount(), 0);
        QCOMPARE(renderGeometryRenderer.primitiveCount(), 0);
        QCOMPARE(renderGeometryRenderer.baseVertex(), 0);
        QCOMPARE(renderGeometryRenderer.baseInstance(), 0);
        QCOMPARE(renderGeometryRenderer.restartIndex(), -1);
        QCOMPARE(renderGeometryRenderer.primitiveRestart(), false);
        QCOMPARE(renderGeometryRenderer.primitiveType(), Qt3D::QGeometryRenderer::Triangles);
        QVERIFY(renderGeometryRenderer.geometryFunctor().isNull());

        // GIVEN
        Qt3D::QGeometryRenderer geometryRenderer;
        Qt3D::QGeometry geometry;
        Qt3D::QGeometryFunctorPtr functor(new TestFunctor(1200));


        geometryRenderer.setInstanceCount(454);
        geometryRenderer.setPrimitiveCount(350);
        geometryRenderer.setBaseVertex(427);
        geometryRenderer.setBaseInstance(383);
        geometryRenderer.setRestartIndex(555);
        geometryRenderer.setPrimitiveRestart(true);
        geometryRenderer.setPrimitiveType(Qt3D::QGeometryRenderer::Patches);
        geometryRenderer.setGeometry(&geometry);
        geometryRenderer.setGeometryFunctor(functor);

        // WHEN
        renderGeometryRenderer.updateFromPeer(&geometryRenderer);
        renderGeometryRenderer.cleanup();

        // THEN
        QVERIFY(renderGeometryRenderer.peerUuid().isNull());
        QVERIFY(renderGeometryRenderer.geometryId().isNull());
        QCOMPARE(renderGeometryRenderer.isDirty(), false);
        QCOMPARE(renderGeometryRenderer.instanceCount(), 0);
        QCOMPARE(renderGeometryRenderer.primitiveCount(), 0);
        QCOMPARE(renderGeometryRenderer.baseVertex(), 0);
        QCOMPARE(renderGeometryRenderer.baseInstance(), 0);
        QCOMPARE(renderGeometryRenderer.restartIndex(), -1);
        QCOMPARE(renderGeometryRenderer.primitiveRestart(), false);
        QCOMPARE(renderGeometryRenderer.primitiveType(), Qt3D::QGeometryRenderer::Triangles);
        QVERIFY(renderGeometryRenderer.geometryFunctor().isNull());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3D::Render::RenderGeometryRenderer renderGeometryRenderer;

        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        Qt3D::QScenePropertyChangePtr updateChange(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setPropertyName("instanceCount");
        updateChange->setValue(2);
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.instanceCount(), 2);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setPropertyName("primitiveCount");
        updateChange->setValue(56);
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.primitiveCount(), 56);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setPropertyName("baseVertex");
        updateChange->setValue(65);
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.baseVertex(), 65);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setPropertyName("baseInstance");
        updateChange->setValue(82);
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.baseInstance(), 82);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setPropertyName("restartIndex");
        updateChange->setValue(46);
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.restartIndex(), 46);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setPropertyName("primitiveRestart");
        updateChange->setValue(true);
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.primitiveRestart(), true);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setPropertyName("primitiveType");
        updateChange->setValue(static_cast<int>(Qt3D::QGeometryRenderer::LineLoop));
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.primitiveType(), Qt3D::QGeometryRenderer::LineLoop);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setPropertyName("geometryFunctor");
        Qt3D::QGeometryFunctorPtr functor(new TestFunctor(1450));
        updateChange->setValue(QVariant::fromValue(functor));
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.geometryFunctor(), functor);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeAdded, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        Qt3D::QNodeId geometryId = Qt3D::QNodeId::createId();
        updateChange->setValue(QVariant::fromValue(geometryId));
        updateChange->setPropertyName("geometry");
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.geometryId(), geometryId);
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());

        // WHEN
        updateChange.reset(new Qt3D::QScenePropertyChange(Qt3D::NodeRemoved, Qt3D::QSceneChange::Node, Qt3D::QNodeId()));
        updateChange->setValue(QVariant::fromValue(geometryId));
        updateChange->setPropertyName("geometry");
        renderGeometryRenderer.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(renderGeometryRenderer.geometryId(), Qt3D::QNodeId());
        QVERIFY(renderGeometryRenderer.isDirty());

        renderGeometryRenderer.unsetDirty();
        QVERIFY(!renderGeometryRenderer.isDirty());
    }
};

QTEST_APPLESS_MAIN(tst_RenderGeometryRenderer)

#include "tst_rendergeometryrenderer.moc"
