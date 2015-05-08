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
#include <Qt3DRenderer/private/rendermaterial_p.h>

#include <Qt3DRenderer/QMaterial>
#include <Qt3DRenderer/QParameter>
#include <Qt3DRenderer/QEffect>
#include <Qt3DCore/QScenePropertyChange>


using namespace Qt3D;
using namespace Qt3D::Render;

class tst_RenderMaterial : public QObject
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
};


void tst_RenderMaterial::shouldHaveInitialState()
{
    // GIVEN
    RenderMaterial backend;

    // THEN
    QVERIFY(backend.parameters().isEmpty());
    QVERIFY(backend.effect().isNull());
    QVERIFY(backend.isEnabled());
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
    RenderMaterial backend;

    // GIVEN
    backend.setPeer(frontendMaterial);

    // THEN
    QVERIFY(backend.isEnabled() == frontendMaterial->isEnabled());
    QCOMPARE(backend.effect(), frontendMaterial->effect() ? frontendMaterial->effect()->id() : QNodeId());
    QCOMPARE(backend.parameters().count(), frontendMaterial->parameters().count());

    int c = 0;
    Q_FOREACH (QParameter *p, frontendMaterial->parameters())
        QCOMPARE(p->id(), backend.parameters().at(c++));

    delete frontendMaterial;
}

void tst_RenderMaterial::shouldHandleParametersPropertyChange()
{
    // GIVEN
    QScopedPointer<QParameter> parameter(new QParameter());
    RenderMaterial backend;

    // WHEN
    QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, parameter->id()));
    addChange->setValue(QVariant::fromValue(parameter->id()));
    addChange->setPropertyName("parameter");
    backend.sceneChangeEvent(addChange);

    // THEN
    QCOMPARE(backend.parameters().count(), 1);
    QCOMPARE(backend.parameters().first(), parameter->id());

    // WHEN
    QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, parameter->id()));
    removeChange->setValue(QVariant::fromValue(parameter->id()));
    removeChange->setPropertyName("parameter");
    backend.sceneChangeEvent(removeChange);

    // THEN
    QVERIFY(backend.parameters().isEmpty());
}

void tst_RenderMaterial::shouldHandleEnablePropertyChange()
{
    // GIVEN
    RenderMaterial backend;

    // WHEN
    QScenePropertyChangePtr updateChange(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, QNodeId()));
    updateChange->setValue(false);
    updateChange->setPropertyName("enabled");
    backend.sceneChangeEvent(updateChange);

    // THEN
    QVERIFY(!backend.isEnabled());

    // WHEN
    QScenePropertyChangePtr secondUpdateChange(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, QNodeId()));
    secondUpdateChange->setValue(true);
    secondUpdateChange->setPropertyName("enabled");
    backend.sceneChangeEvent(secondUpdateChange);

    // THEN
    QVERIFY(backend.isEnabled());
}

QTEST_APPLESS_MAIN(tst_RenderMaterial)

#include "tst_rendermaterial.moc"
