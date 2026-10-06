// Copyright (C) 2026 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/qtest.h>

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qgeometryrenderer.h>

#include <Qt3DRender/private/qsceneimportfactory_p.h>
#include <Qt3DRender/private/qsceneimporter_p.h>

class tst_assimpPlugin : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void init();
    void cleanup();
    void importMesh();

private:
    Qt3DRender::QSceneImporter *m_importer{ nullptr };
};

void tst_assimpPlugin::init()
{
    m_importer = Qt3DRender::QSceneImportFactory::create(QStringLiteral("assimp"), { });
}

void tst_assimpPlugin::cleanup()
{
    delete m_importer;
    m_importer = nullptr;
}

void tst_assimpPlugin::importMesh()
{
    if (m_importer == nullptr)
        QSKIP("Missing assimp importer");

    m_importer->setSource(QUrl(QStringLiteral("qrc:/triangle.gltf")));

    Qt3DCore::QEntity *rootEntity = m_importer->scene();
    QVERIFY(rootEntity != nullptr);

    auto geometryRenderers = rootEntity->componentsOfType<Qt3DRender::QGeometryRenderer>();
    QCOMPARE(geometryRenderers.size(), 1);

    Qt3DRender::QGeometryRenderer *geometryRenderer = geometryRenderers.front();
    QCOMPARE(geometryRenderer->primitiveType(), Qt3DRender::QGeometryRenderer::Triangles);

    Qt3DCore::QGeometry *geometry = geometryRenderer->geometry();
    QVERIFY(geometry != nullptr);

    const auto attributes = geometry->attributes();
    QCOMPARE(attributes.size(), 3);

    for (const Qt3DCore::QAttribute *attr : attributes) {
        QCOMPARE(attr->count(), 3);
    }

    delete rootEntity;
}

QTEST_MAIN(tst_assimpPlugin)

#include "tst_assimpplugin.moc"
