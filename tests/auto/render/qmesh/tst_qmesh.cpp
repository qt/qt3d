// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/private/qmesh_p.h>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/private/qscene_p.h>
#include "testarbiter.h"

class MyQMesh : public Qt3DRender::QMesh
{
    Q_OBJECT
public:
    explicit MyQMesh(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QMesh(parent)
    {}
};

class tst_QMesh : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QMesh mesh;

        // THEN
        QCOMPARE(mesh.source(), QUrl());
        QCOMPARE(mesh.meshName(), QString());
        QCOMPARE(mesh.status(), Qt3DRender::QMesh::None);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QMesh mesh;

        {
            // WHEN
            QSignalSpy spy(&mesh, SIGNAL(sourceChanged(QUrl)));
            const QUrl newValue(QStringLiteral("qrc:/mesh.obj"));
            mesh.setSource(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mesh.source(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mesh.setSource(newValue);

            // THEN
            QCOMPARE(mesh.source(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&mesh, SIGNAL(meshNameChanged(QString)));
            const QString newValue = QStringLiteral("MainBody");
            mesh.setMeshName(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mesh.meshName(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mesh.setMeshName(newValue);

            // THEN
            QCOMPARE(mesh.meshName(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkSourceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DCore::QAspectEngine engine;
        Qt3DCore::QScene *scene = Qt3DCore::QAspectEnginePrivate::get(&engine)->m_scene;
        Qt3DRender::QMesh mesh;

        arbiter.setArbiterOnNode(&mesh);
        Qt3DCore::QNodePrivate *meshd = Qt3DCore::QNodePrivate::get(&mesh);
        meshd->setScene(scene);
        QCoreApplication::processEvents();
        arbiter.clear();

        {
            // WHEN
            mesh.setSource(QUrl(QStringLiteral("qrc:/toyplane.obj")));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mesh);

            arbiter.clear();
        }

        {
            // WHEN
            mesh.setSource(QStringLiteral("qrc:/toyplane.obj"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkMeshNameUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DCore::QAspectEngine engine;
        Qt3DCore::QScene *scene = Qt3DCore::QAspectEnginePrivate::get(&engine)->m_scene;
        Qt3DRender::QMesh mesh;

        arbiter.setArbiterOnNode(&mesh);
        Qt3DCore::QNodePrivate *meshd = Qt3DCore::QNodePrivate::get(&mesh);
        meshd->setScene(scene);
        QCoreApplication::processEvents();
        arbiter.clear();

        {
            // WHEN
            mesh.setMeshName(QStringLiteral("Phil"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mesh);

            arbiter.clear();
        }

        {
            // WHEN
            mesh.setMeshName(QStringLiteral("Phil"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkGeometryFactoryIsAccessibleEvenWithNoScene() // QTBUG-65506
    {
        // GIVEN
        Qt3DRender::QMesh mesh;

        // WHEN
        mesh.setSource(QUrl(QStringLiteral("some_path")));

        // THEN
        QVERIFY(!Qt3DRender::QMeshPrivate::get(&mesh)->m_geometryFactory.isNull());
    }
};

QTEST_MAIN(tst_QMesh)

#include "tst_qmesh.moc"
