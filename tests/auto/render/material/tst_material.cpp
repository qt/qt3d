/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    QCOMPARE(backend.parameters().count(), frontendMaterial->parameters().count());
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
    QCOMPARE(backend.parameters().count(), 1);
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
