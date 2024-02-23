// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/material_p.h>

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QEffect>
#include "testrenderer.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

class tst_RenderMaterial : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

public:
    tst_RenderMaterial() {}

private slots:
    void shouldHaveInitialState();
    void shouldHavePropertiesMirroringFromItsPeer_data();
    void shouldHavePropertiesMirroringFromItsPeer();
    void shouldHandleParametersPropertyChange();
    void shouldHandleEnablePropertyChange();
    void shouldHandleEffectPropertyChange();
};


void tst_RenderMaterial::shouldHaveInitialState()
{
    // GIVEN
    Material backend;

    // THEN
    QVERIFY(backend.parameters().isEmpty());
    QVERIFY(backend.effect().isNull());
    QVERIFY(!backend.isEnabled());
}

void tst_RenderMaterial::shouldHavePropertiesMirroringFromItsPeer_data()
{
    QTest::addColumn<QMaterial *>("frontendMaterial");

    QMaterial *emptyMaterial = new QMaterial();
    QTest::newRow("emptyMaterial") << emptyMaterial;

    QMaterial *simpleMaterial = new QMaterial();
    simpleMaterial->addParameter(new QParameter());
    simpleMaterial->setEffect(new QEffect());
    QTest::newRow("simpleMaterial") << simpleMaterial;

    QMaterial *manyParametersMaterial = new QMaterial();
    manyParametersMaterial->addParameter(new QParameter());
    manyParametersMaterial->addParameter(new QParameter());
    manyParametersMaterial->addParameter(new QParameter());
    manyParametersMaterial->addParameter(new QParameter());
    simpleMaterial->setEffect(new QEffect());
    QTest::newRow("manyParametersMaterial") << manyParametersMaterial;

    QMaterial *disabledSimpleMaterial = new QMaterial();
    disabledSimpleMaterial->setEnabled(false);
    disabledSimpleMaterial->addParameter(new QParameter());
    disabledSimpleMaterial->setEffect(new QEffect());
    QTest::newRow("simpleDisabledMaterial") << disabledSimpleMaterial;

    QMaterial *manyParametersDisabledMaterial = new QMaterial();
    manyParametersDisabledMaterial->setEnabled(false);
    manyParametersDisabledMaterial->setEffect(new QEffect());
    manyParametersDisabledMaterial->addParameter(new QParameter());
    manyParametersDisabledMaterial->addParameter(new QParameter());
    manyParametersDisabledMaterial->addParameter(new QParameter());
    manyParametersDisabledMaterial->addParameter(new QParameter());
    manyParametersDisabledMaterial->addParameter(new QParameter());
    manyParametersDisabledMaterial->addParameter(new QParameter());
    manyParametersDisabledMaterial->addParameter(new QParameter());
    QTest::newRow("manyParametersDisabledMaterial") << manyParametersDisabledMaterial;
}

void tst_RenderMaterial::shouldHavePropertiesMirroringFromItsPeer()
{
    // WHEN
    QFETCH(QMaterial *, frontendMaterial);
    TestRenderer renderer;
    Material backend;

    // GIVEN
    backend.setRenderer(&renderer);
    simulateInitializationSync(frontendMaterial, &backend);

    // THEN
    QVERIFY(backend.isEnabled() == frontendMaterial->isEnabled());
    QCOMPARE(backend.effect(), frontendMaterial->effect() ? frontendMaterial->effect()->id() : QNodeId());
    QCOMPARE(backend.parameters().size(), frontendMaterial->parameters().size());
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::MaterialDirty);

    int c = 0;
    const auto frontendMaterialParameters = frontendMaterial->parameters();
    for (QParameter *p : frontendMaterialParameters)
        QCOMPARE(p->id(), backend.parameters().at(c++));

    delete frontendMaterial;
}

void tst_RenderMaterial::shouldHandleParametersPropertyChange()
{
    // GIVEN
    QParameter *parameter = new QParameter();
    Material backend;
    TestRenderer renderer;
    backend.setRenderer(&renderer);

    QMaterial material;
    simulateInitializationSync(&material, &backend);

    // WHEN
    material.addParameter(parameter);
    backend.syncFromFrontEnd(&material, false);

    // THEN
    QCOMPARE(backend.parameters().size(), 1);
    QCOMPARE(backend.parameters().first(), parameter->id());
    QVERIFY(renderer.dirtyBits() != 0);

    // WHEN
    material.removeParameter(parameter);
    backend.syncFromFrontEnd(&material, false);

    // THEN
    QVERIFY(backend.parameters().isEmpty());
}

void tst_RenderMaterial::shouldHandleEnablePropertyChange()
{
    // GIVEN
    Material backend;
    TestRenderer renderer;
    backend.setRenderer(&renderer);

    QMaterial material;
    simulateInitializationSync(&material, &backend);

    // WHEN
    material.setEnabled(false);
    backend.syncFromFrontEnd(&material, false);

    // THEN
    QVERIFY(!backend.isEnabled());
    QVERIFY(renderer.dirtyBits() != 0);

    // WHEN
    material.setEnabled(true);
    backend.syncFromFrontEnd(&material, false);

    // THEN
    QVERIFY(backend.isEnabled());

}

void tst_RenderMaterial::shouldHandleEffectPropertyChange()
{
    // GIVEN
    Material backend;
    TestRenderer renderer;
    backend.setRenderer(&renderer);

    QMaterial material;
    simulateInitializationSync(&material, &backend);

    // WHEN
    QEffect effect;
    material.setEffect(&effect);
    backend.syncFromFrontEnd(&material, false);

    // THEN
    QCOMPARE(backend.effect(), effect.id());
    QVERIFY(renderer.dirtyBits() != 0);
}

QTEST_APPLESS_MAIN(tst_RenderMaterial)

#include "tst_material.moc"
