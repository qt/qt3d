// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtTest/qtest.h>

#include <QtCore/QScopedPointer>
#include <QtCore/private/qfactoryloader_p.h>

#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qgeometry.h>

#include <Qt3DRender/private/qgeometryloaderfactory_p.h>
#include <Qt3DRender/private/qgeometryloaderinterface_p.h>

#include "../../../../src/plugins/geometryloaders/qtgeometryloaders-config.h"

using namespace Qt3DCore;
using namespace Qt3DRender;

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, geometryLoader,
    (QGeometryLoaderFactory_iid, QLatin1String("/geometryloaders"), Qt::CaseInsensitive))

class tst_geometryloaders : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testOBJLoader_data();
    void testOBJLoader();
    void testPLYLoader();
    void testSTLLoader();
    void testGLTFLoader();
#ifdef QT_3DGEOMETRYLOADERS_FBX
    void testFBXLoader();
#endif
};

void tst_geometryloaders::testOBJLoader_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::newRow("nominal case") << QStringLiteral(":/cube.obj");
    QTest::newRow("trailing space + crlf") << QStringLiteral(":/cube2.obj");
}
void tst_geometryloaders::testOBJLoader()
{
    QScopedPointer<QGeometryLoaderInterface> loader;
    loader.reset(qLoadPlugin<QGeometryLoaderInterface, QGeometryLoaderFactory>(geometryLoader(), QStringLiteral("obj")));
    QVERIFY(loader);
    if (!loader)
        return;

    QFETCH(QString, fileName);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Could not open test file for reading");
        return;
    }

    bool loaded = loader->load(&file, QStringLiteral("Cube"));
    QVERIFY(loaded);
    if (!loaded)
        return;

    QGeometry *geometry = loader->geometry();
    QVERIFY(geometry);
    if (!geometry)
        return;

    QCOMPARE(geometry->attributes().size(), 3);
    for (QAttribute *attr : geometry->attributes()) {
        switch (attr->attributeType()) {
        case QAttribute::IndexAttribute:
            QCOMPARE(attr->count(), 36u);
            break;
        case QAttribute::VertexAttribute:
            QCOMPARE(attr->count(), 24u);
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
    }

    file.close();
}

void tst_geometryloaders::testPLYLoader()
{
    QScopedPointer<QGeometryLoaderInterface> loader;
    loader.reset(qLoadPlugin<QGeometryLoaderInterface, QGeometryLoaderFactory>(geometryLoader(), QStringLiteral("ply")));
    QVERIFY(loader);
    if (!loader)
        return;

    QFile file(QStringLiteral(":/cube.ply"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Could not open test file for reading");
        return;
    }

    bool loaded = loader->load(&file, QStringLiteral("Cube"));
    QVERIFY(loaded);
    if (!loaded)
        return;

    QGeometry *geometry = loader->geometry();
    QVERIFY(geometry);
    if (!geometry)
        return;

    QCOMPARE(geometry->attributes().size(), 3);
    for (QAttribute *attr : geometry->attributes()) {
        switch (attr->attributeType()) {
        case QAttribute::IndexAttribute:
            QCOMPARE(attr->count(), 36u);
            break;
        case QAttribute::VertexAttribute:
            QCOMPARE(attr->count(), 24u);
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
    }

    file.close();
}

void tst_geometryloaders::testSTLLoader()
{
    QScopedPointer<QGeometryLoaderInterface> loader;
    loader.reset(qLoadPlugin<QGeometryLoaderInterface, QGeometryLoaderFactory>(geometryLoader(), QStringLiteral("stl")));
    QVERIFY(loader);
    if (!loader)
        return;

    QFile file(QStringLiteral(":/cube.stl"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Could not open test file for reading");
        return;
    }

    bool loaded = loader->load(&file, QStringLiteral("Cube"));
    QVERIFY(loaded);
    if (!loaded)
        return;

    QGeometry *geometry = loader->geometry();
    QVERIFY(geometry);
    if (!geometry)
        return;

    QCOMPARE(geometry->attributes().size(), 3);
    for (QAttribute *attr : geometry->attributes()) {
        QCOMPARE(attr->count(), 36u);
    }

    file.close();
}

void tst_geometryloaders::testGLTFLoader()
{
    QScopedPointer<QGeometryLoaderInterface> loader;
    loader.reset(qLoadPlugin<QGeometryLoaderInterface, QGeometryLoaderFactory>(geometryLoader(), QStringLiteral("gltf")));
    QVERIFY(loader);
    if (!loader)
        return;

    QFile file(QStringLiteral(":/cube.gltf"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Could not open test file for reading");
        return;
    }

    bool loaded = loader->load(&file, QStringLiteral("Cube"));
    QVERIFY(loaded);
    if (!loaded)
        return;

    QGeometry *geometry = loader->geometry();
    QVERIFY(geometry);
    if (!geometry)
        return;

    QCOMPARE(geometry->attributes().size(), 3);
    for (QAttribute *attr : geometry->attributes()) {
        switch (attr->attributeType()) {
        case QAttribute::IndexAttribute:
            QCOMPARE(attr->count(), 36u);
            break;
        case QAttribute::VertexAttribute:
            QCOMPARE(attr->count(), 24u);
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
    }

    file.close();
}

#ifdef QT_3DGEOMETRYLOADERS_FBX
void tst_geometryloaders::testFBXLoader()
{
    QScopedPointer<QGeometryLoaderInterface> loader;
    loader.reset(qLoadPlugin<QGeometryLoaderInterface, QGeometryLoaderFactory>(geometryLoader(), QStringLiteral("fbx")));
    QVERIFY(loader);
    if (!loader)
        return;

    QFile file(QStringLiteral(":/cube.fbx"));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug("Could not open test file for reading");
        return;
    }

    bool loaded = loader->load(&file, QStringLiteral("Cube"));
    QVERIFY(loaded);
    if (!loaded)
        return;

    QGeometry *geometry = loader->geometry();
    QVERIFY(geometry);
    if (!geometry)
        return;

    QCOMPARE(geometry->attributes().count(), 3);
    for (QAttribute *attr : geometry->attributes()) {
        switch (attr->attributeType()) {
        case QAttribute::IndexAttribute:
            QCOMPARE(attr->count(), 36u);
            break;
        case QAttribute::VertexAttribute:
            QCOMPARE(attr->count(), 24u);
            break;
        }
    }

    file.close();
}
#endif

QTEST_MAIN(tst_geometryloaders)

#include "tst_geometryloaders.moc"
