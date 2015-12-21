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

#include <QtTest/QtTest>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/QScenePropertyChange>
#include <Qt3DRender/private/qboundingvolumeprovider_p.h>
#include <Qt3DRender/private/pickboundingvolumejob_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qspheremesh.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferfunctor.h>
#include <Qt3DRender/qspheregeometry.h>
#include <Qt3DRender/qpickevent.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

class MyObjectPicker : public Qt3DRender::QObjectPicker
{
    Q_OBJECT
public:
    MyObjectPicker(Qt3DCore::QNode *parent = Q_NULLPTR)
        : Qt3DRender::QObjectPicker(parent)
    {}

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_FINAL
    {
        Qt3DRender::QObjectPicker::sceneChangeEvent(change);
    }
};

class PickableEntity : public QEntity
{
    Q_OBJECT
public:
    explicit PickableEntity(const QVector3D &position, float radius, QEntity *parent = Q_NULLPTR)
        : QEntity(parent)
        , picker(new MyObjectPicker(this))
        , mesh(new QSphereMesh(this))
        , transform(new Qt3DCore::QTransform(this))
        , acceptsEvents(true)
        , pressedCalled(0)
        , releasedCalled(0)
        , clickedCalled(0)
    {
        mesh->setRadius(radius);
        QSphereGeometry *g = static_cast<QSphereGeometry *>(mesh->geometry());
        QAttribute *positionAttr = static_cast<QAttribute *>(g->attributes().first());
        Qt3DRender::QBuffer *vertexBuffer = static_cast<Qt3DRender::QBuffer *>(positionAttr->buffer());

        // Load the geometry
        const QByteArray data = (*vertexBuffer->bufferFunctor())();
        vertexBuffer->setData(data);

        transform->setTranslation(position);

        addComponent(picker);
        addComponent(mesh);
        addComponent(transform);

        QObject::connect(picker, &QObjectPicker::pressed, this, &PickableEntity::onPressed);
        QObject::connect(picker, &QObjectPicker::released, this, &PickableEntity::onReleased);
        QObject::connect(picker, &QObjectPicker::clicked, this, &PickableEntity::onClicked);
    }

    MyObjectPicker *picker;
    QSphereMesh *mesh;
    Qt3DCore::QTransform *transform;

    bool acceptsEvents;
    int pressedCalled;
    int releasedCalled;
    int clickedCalled;

private Q_SLOTS:
    void onPressed(QPickEvent *e)
    {
        e->setAccepted(acceptsEvents);
        ++pressedCalled;
    }

    void onReleased(QPickEvent *e)
    {
        e->setAccepted(acceptsEvents);
        if (acceptsEvents)
            ++releasedCalled;
    }

    void onClicked(QPickEvent *e)
    {
        e->setAccepted(acceptsEvents);
        ++clickedCalled;
    }
};

class tst_Picking : public QObject
{
    Q_OBJECT
public:
    tst_Picking() {}
    ~tst_Picking() {}

private Q_SLOTS:
    void testEventPressedAcceptPropagation()
    {
        // GIVEN
        PickableEntity root(QVector3D(), 5.0f);
        PickableEntity child1(QVector3D(), 5.0f, &root);
        PickableEntity child2(QVector3D(), 5.0f, &root);
        PickableEntity child11(QVector3D(), 5.0f, &child1);

        // WHEN
        Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, child11.id()));
        e->setPropertyName("pressed");
        child11.picker->sceneChangeEvent(e);

        // THEN
        QCOMPARE(root.pressedCalled, 0);
        QCOMPARE(child1.pressedCalled, 0);
        QCOMPARE(child2.pressedCalled, 0);
        QCOMPARE(child11.pressedCalled, 1);

        // WHEN
        child11.pressedCalled = 0;
        child11.acceptsEvents = false;
        child11.picker->sceneChangeEvent(e);

        // THEN
        QCOMPARE(root.pressedCalled, 0);
        QCOMPARE(child1.pressedCalled, 1);
        QCOMPARE(child2.pressedCalled, 0);
        QCOMPARE(child11.pressedCalled, 1);

        // WHEN
        child1.acceptsEvents = false;
        child1.pressedCalled = 0;
        child11.acceptsEvents = false;
        child11.pressedCalled = 0;
        child11.picker->sceneChangeEvent(e);

        // THEN
        QCOMPARE(root.pressedCalled, 1);
        QCOMPARE(child1.pressedCalled, 1);
        QCOMPARE(child2.pressedCalled, 0);
        QCOMPARE(child11.pressedCalled, 1);
    }

    void testEventReleasedAcceptPropagation()
    {
        // GIVEN
        PickableEntity root(QVector3D(), 5.0f);
        PickableEntity child1(QVector3D(), 5.0f, &root);
        PickableEntity child2(QVector3D(), 5.0f, &root);
        PickableEntity child11(QVector3D(), 5.0f, &child1);

        // WHEN
        Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, child11.id()));
        e->setPropertyName("pressed");
        child11.picker->sceneChangeEvent(e);
        e->setPropertyName("released");
        child11.picker->sceneChangeEvent(e);

        // THEN
        QCOMPARE(root.releasedCalled, 0);
        QCOMPARE(child1.releasedCalled, 0);
        QCOMPARE(child2.releasedCalled, 0);
        QCOMPARE(child11.releasedCalled, 1);

        // WHEN
        child11.releasedCalled = 0;
        child11.pressedCalled = 0;
        child11.acceptsEvents = false;
        e->setPropertyName("pressed");
        child11.picker->sceneChangeEvent(e);
        e->setPropertyName("released");
        child11.picker->sceneChangeEvent(e);

        // THEN
        QCOMPARE(child1.pressedCalled, 1);
        QCOMPARE(child11.pressedCalled, 1);

        QCOMPARE(root.releasedCalled, 0);
        QCOMPARE(child1.releasedCalled, 1);
        QCOMPARE(child2.releasedCalled, 0);
        QCOMPARE(child11.releasedCalled, 0);
    }

    void testEventClickedAcceptPropagation()
    {
        // GIVEN
        PickableEntity root(QVector3D(), 5.0f);
        PickableEntity child1(QVector3D(), 5.0f, &root);
        PickableEntity child2(QVector3D(), 5.0f, &root);
        PickableEntity child11(QVector3D(), 5.0f, &child1);

        // WHEN
        Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, child11.id()));
        e->setPropertyName("clicked");
        child11.picker->sceneChangeEvent(e);

        // THEN
        QCOMPARE(root.clickedCalled, 0);
        QCOMPARE(child1.clickedCalled, 0);
        QCOMPARE(child2.clickedCalled, 0);
        QCOMPARE(child11.clickedCalled, 1);

        // WHEN
        child11.clickedCalled = 0;
        child11.acceptsEvents = false;
        child11.picker->sceneChangeEvent(e);

        // THEN
        QCOMPARE(root.clickedCalled, 0);
        QCOMPARE(child1.clickedCalled, 1);
        QCOMPARE(child2.clickedCalled, 0);
        QCOMPARE(child11.clickedCalled, 1);

        // WHEN
        child1.acceptsEvents = false;
        child1.clickedCalled = 0;
        child11.acceptsEvents = false;
        child11.clickedCalled = 0;
        child11.picker->sceneChangeEvent(e);

        // THEN
        QCOMPARE(root.clickedCalled, 1);
        QCOMPARE(child1.clickedCalled, 1);
        QCOMPARE(child2.clickedCalled, 0);
        QCOMPARE(child11.clickedCalled, 1);
    }
};

QTEST_APPLESS_MAIN(tst_Picking)

#include "tst_picking.moc"
