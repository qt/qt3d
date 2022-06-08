// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DRender/private/qboundingvolumeprovider_p.h>
#include <Qt3DRender/private/pickboundingvolumejob_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/private/qobjectpicker_p.h>
#include <Qt3DExtras/qspheremesh.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DExtras/qspheregeometry.h>
#include <Qt3DRender/qpickevent.h>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
using namespace Qt3DRender::Render;

class MyObjectPicker : public Qt3DRender::QObjectPicker
{
    Q_OBJECT
public:
    MyObjectPicker(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QObjectPicker(parent)
    {}
};

class PickableEntity : public QEntity
{
    Q_OBJECT
public:
    explicit PickableEntity(const QVector3D &position, float radius, QEntity *parent = nullptr)
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
        QSphereGeometry *g = static_cast<QSphereGeometry *>(mesh->view()->geometry());
        QAttribute *positionAttr = static_cast<QAttribute *>(g->attributes().first());
        Qt3DCore::QBuffer *vertexBuffer = static_cast<Qt3DCore::QBuffer *>(positionAttr->buffer());
        Q_UNUSED(vertexBuffer);

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
    tst_Picking()
    {
        qRegisterMetaType<Qt3DCore::QNode*>();
    }

    ~tst_Picking() {}

private Q_SLOTS:
    void testEventPressedAcceptPropagation()
    {
        // GIVEN
        Qt3DCore::QScene scene;
        PickableEntity root(QVector3D(), 5.0f);
        PickableEntity child1(QVector3D(), 5.0f, &root);
        PickableEntity child2(QVector3D(), 5.0f, &root);
        PickableEntity child11(QVector3D(), 5.0f, &child1);
        Qt3DCore::QNodePrivate::get(root.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child1.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child2.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child11.picker)->setScene(&scene);

        QCoreApplication::processEvents();

        auto dpicker = [](QObjectPicker *node) {
            return static_cast<QObjectPickerPrivate *>(QObjectPickerPrivate::get(node));
        };

        // WHEN
        Qt3DRender::QPickEventPtr event(new Qt3DRender::QPickEvent());
        dpicker(child11.picker)->pressedEvent(event.data());

        // THEN
        QCOMPARE(root.pressedCalled, 0);
        QCOMPARE(child1.pressedCalled, 0);
        QCOMPARE(child2.pressedCalled, 0);
        QCOMPARE(child11.pressedCalled, 1);

        // WHEN
        child11.pressedCalled = 0;
        child11.acceptsEvents = false;
        dpicker(child11.picker)->pressedEvent(event.data());

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
        dpicker(child11.picker)->pressedEvent(event.data());

        // THEN
        QCOMPARE(root.pressedCalled, 1);
        QCOMPARE(child1.pressedCalled, 1);
        QCOMPARE(child2.pressedCalled, 0);
        QCOMPARE(child11.pressedCalled, 1);
    }

    void testEventReleasedAcceptPropagation()
    {
        // GIVEN
        Qt3DCore::QScene scene;
        PickableEntity root(QVector3D(), 5.0f);
        PickableEntity child1(QVector3D(), 5.0f, &root);
        PickableEntity child2(QVector3D(), 5.0f, &root);
        PickableEntity child11(QVector3D(), 5.0f, &child1);
        Qt3DCore::QNodePrivate::get(root.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child1.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child2.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child11.picker)->setScene(&scene);

        QCoreApplication::processEvents();

        auto dpicker = [](QObjectPicker *node) {
            return static_cast<QObjectPickerPrivate *>(QObjectPickerPrivate::get(node));
        };

        // WHEN
        Qt3DRender::QPickEventPtr event(new Qt3DRender::QPickEvent());
        dpicker(child11.picker)->pressedEvent(event.data());
        dpicker(child11.picker)->releasedEvent(event.data());

        // THEN
        QCOMPARE(root.releasedCalled, 0);
        QCOMPARE(child1.releasedCalled, 0);
        QCOMPARE(child2.releasedCalled, 0);
        QCOMPARE(child11.releasedCalled, 1);

        // WHEN
        child11.releasedCalled = 0;
        child11.pressedCalled = 0;
        child11.acceptsEvents = false;
        dpicker(child11.picker)->pressedEvent(event.data());
        dpicker(child11.picker)->releasedEvent(event.data());

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
        Qt3DCore::QScene scene;
        PickableEntity root(QVector3D(), 5.0f);
        PickableEntity child1(QVector3D(), 5.0f, &root);
        PickableEntity child2(QVector3D(), 5.0f, &root);
        PickableEntity child11(QVector3D(), 5.0f, &child1);
        Qt3DCore::QNodePrivate::get(root.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child1.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child2.picker)->setScene(&scene);
        Qt3DCore::QNodePrivate::get(child11.picker)->setScene(&scene);

        QCoreApplication::processEvents();

        auto dpicker = [](QObjectPicker *node) {
            return static_cast<QObjectPickerPrivate *>(QObjectPickerPrivate::get(node));
        };

        // WHEN
        Qt3DRender::QPickEventPtr event(new Qt3DRender::QPickEvent());
        dpicker(child11.picker)->clickedEvent(event.data());

        // THEN
        QCOMPARE(root.clickedCalled, 0);
        QCOMPARE(child1.clickedCalled, 0);
        QCOMPARE(child2.clickedCalled, 0);
        QCOMPARE(child11.clickedCalled, 1);

        // WHEN
        child11.clickedCalled = 0;
        child11.acceptsEvents = false;
        dpicker(child11.picker)->clickedEvent(event.data());

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
        dpicker(child11.picker)->clickedEvent(event.data());

        // THEN
        QCOMPARE(root.clickedCalled, 1);
        QCOMPARE(child1.clickedCalled, 1);
        QCOMPARE(child2.clickedCalled, 0);
        QCOMPARE(child11.clickedCalled, 1);
    }
};

QTEST_MAIN(tst_Picking)

#include "tst_picking.moc"
