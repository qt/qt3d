// Copyright (C) 2026 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/qtest.h>

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DAnimation/qmorphinganimation.h>

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
    void importMorphTarget();

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

void tst_assimpPlugin::importMorphTarget()
{
    if (m_importer == nullptr)
        QSKIP("Missing assimp importer");

    m_importer->setSource(QUrl(QStringLiteral("qrc:/morph_target.gltf")));

    Qt3DCore::QEntity *rootEntity = m_importer->scene();
    QVERIFY(rootEntity != nullptr);

    auto *morphingAnimation = rootEntity->findChild<Qt3DAnimation::QMorphingAnimation *>();
    QVERIFY(morphingAnimation != nullptr);
    QCOMPARE(morphingAnimation->morphTargetList().size(), 2);

    for (const Qt3DAnimation::QMorphTarget* morphTarget : morphingAnimation->morphTargetList()) {
        QVERIFY(morphTarget->attributeList().size() == 1);
        Qt3DCore::QAttribute* attr = morphTarget->attributeList().front();
        QCOMPARE(attr->count(), 3);
    }

    delete rootEntity;
}

QTEST_MAIN(tst_assimpPlugin)

#include "tst_assimpplugin.moc"
