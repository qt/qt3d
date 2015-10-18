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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QPhongMaterial>
#include <Qt3DRender/QParameterMapping>
#include <Qt3DRender/QDiffuseMapMaterial>
#include <Qt3DRender/QPerVertexColorMaterial>
#include <Qt3DRender/QNormalDiffuseMapMaterial>
#include <Qt3DRender/QDiffuseSpecularMapMaterial>
#include <Qt3DRender/QNormalDiffuseMapAlphaMaterial>
#include <Qt3DRender/QNormalDiffuseSpecularMapMaterial>

#include "testpostmanarbiter.h"

class TestMaterial : public Qt3DRender::QMaterial
{
public:
    explicit TestMaterial(Qt3DCore::QNode *parent = 0)
        : Qt3DRender::QMaterial(parent)
        , m_effect(new Qt3DRender::QEffect(this))
        , m_technique(new Qt3DRender::QTechnique(this))
        , m_renderPass(new Qt3DRender::QRenderPass(this))
        , m_shaderProgram(new Qt3DRender::QShaderProgram(this))
    {
        m_renderPass->setShaderProgram(m_shaderProgram);
        m_technique->addPass(m_renderPass);
        m_effect->addTechnique(m_technique);
        setEffect(m_effect);
    }

    ~TestMaterial()
    {
        QNode::cleanup();
    }

    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QShaderProgram *m_shaderProgram;
};

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QMaterial : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QMaterial()
        : Qt3DCore::QNode()
    {
        qRegisterMetaType<Qt3DRender::QEffect*>("Qt3DRender::QEffect*");
    }

    ~tst_QMaterial()
    {
        QNode::cleanup();
    }

private:

    void compareEffects(const Qt3DRender::QEffect *original, const Qt3DRender::QEffect *clone)
    {
        bool isEffectNull = (original == Q_NULLPTR);
        if (isEffectNull) {
            QVERIFY(!clone);
        } else {
            QVERIFY(clone);
            QCOMPARE(original->id(), clone->id());

            compareParameters(original->parameters(), clone->parameters());

            const int techniqueCounts = original->techniques().size();
            QCOMPARE(techniqueCounts, clone->techniques().size());

            for (int i = 0; i < techniqueCounts; ++i)
                compareTechniques(original->techniques().at(i), clone->techniques().at(i));
        }
    }

    void compareTechniques(const Qt3DRender::QTechnique *original, const Qt3DRender::QTechnique *clone)
    {
        QCOMPARE(original->id(), clone->id());

        compareParameters(original->parameters(), clone->parameters());

        const int passesCount = original->renderPasses().size();
        QCOMPARE(passesCount, clone->renderPasses().size());

        for (int i = 0; i < passesCount; ++i)
            compareRenderPasses(original->renderPasses().at(i), clone->renderPasses().at(i));
    }

    void compareRenderPasses(const Qt3DRender::QRenderPass *original, const Qt3DRender::QRenderPass *clone)
    {
        QCOMPARE(original->id(), clone->id());

        compareParameters(original->parameters(), clone->parameters());
        compareRenderStates(original->renderStates(), clone->renderStates());
        compareAnnotations(original->annotations(), clone->annotations());
        compareBindings(original->bindings(), clone->bindings());
        compareShaderPrograms(original->shaderProgram(), clone->shaderProgram());
    }

    void compareParameters(const Qt3DRender::ParameterList &original, const Qt3DRender::ParameterList &clone)
    {
        QCOMPARE(original.size(), clone.size());
        const int parametersCount = original.size();
        for (int i = 0; i < parametersCount; ++i) {
            const Qt3DRender::QParameter *originParam = original.at(i);
            const Qt3DRender::QParameter *cloneParam = clone.at(i);
            QCOMPARE(originParam->id(), cloneParam->id());
            QCOMPARE(cloneParam->name(), originParam->name());
            QCOMPARE(cloneParam->value(), originParam->value());
        }
    }

    void compareAnnotations(const QList<Qt3DRender::QAnnotation *> &original, const QList<Qt3DRender::QAnnotation *> &clone)
    {
        const int annotationsCount = original.size();
        QCOMPARE(annotationsCount, clone.size());

        for (int i = 0; i < annotationsCount; ++i) {
            const Qt3DRender::QAnnotation *origAnnotation = original.at(i);
            const Qt3DRender::QAnnotation *cloneAnnotation = clone.at(i);
            QCOMPARE(origAnnotation->id(), cloneAnnotation->id());
            QCOMPARE(origAnnotation->name(), cloneAnnotation->name());
            QCOMPARE(origAnnotation->value(), cloneAnnotation->value());
        }
    }

    void compareBindings(const QList<Qt3DRender::QParameterMapping *> &original, const QList<Qt3DRender::QParameterMapping *> &clone)
    {
        const int bindingsCount = original.size();
        QCOMPARE(bindingsCount, clone.size());

        for (int i = 0; i < bindingsCount; ++i) {
            const Qt3DRender::QParameterMapping *origMapping = original.at(i);
            const Qt3DRender::QParameterMapping *cloneMapping = clone.at(i);

            QCOMPARE(origMapping->id(), cloneMapping->id());
            QCOMPARE(origMapping->bindingType(), cloneMapping->bindingType());
            QCOMPARE(origMapping->parameterName(), cloneMapping->parameterName());
            QCOMPARE(origMapping->shaderVariableName(), cloneMapping->shaderVariableName());
        }
    }

    void compareRenderStates(const QList<Qt3DRender::QRenderState *> &original, const QList<Qt3DRender::QRenderState *> &clone)
    {
        const int renderStatesCount = original.size();
        QCOMPARE(renderStatesCount, clone.size());

        for (int i = 0; i < renderStatesCount; ++i) {
            const Qt3DRender::QRenderState *originState = original.at(i);
            const Qt3DRender::QRenderState *cloneState = clone.at(i);
            QCOMPARE(originState->id(), originState->id());
            QVERIFY(originState->type() == cloneState->type());
        }
    }

    void compareShaderPrograms(const Qt3DRender::QShaderProgram *original, const Qt3DRender::QShaderProgram *clone)
    {
        bool isOriginalNull = (original == Q_NULLPTR);
        if (isOriginalNull) {
            QVERIFY(!clone);
        } else {
            QVERIFY(clone);
            QCOMPARE(original->id(), clone->id());
            QVERIFY(original->vertexShaderCode() == clone->vertexShaderCode());
            QVERIFY(original->fragmentShaderCode() == clone->fragmentShaderCode());
            QVERIFY(original->geometryShaderCode() == clone->geometryShaderCode());
            QVERIFY(original->computeShaderCode() == clone->computeShaderCode());
            QVERIFY(original->tessellationControlShaderCode() == clone->tessellationControlShaderCode());
            QVERIFY(original->tessellationEvaluationShaderCode() == clone->tessellationEvaluationShaderCode());
        }
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QMaterial *>("material");

        Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial();
        QTest::newRow("empty material") << material;
        material = new TestMaterial();
        QTest::newRow("test material") << material;
        material = new Qt3DRender::QPhongMaterial();
        QTest::newRow("QPhongMaterial") << material;
        material = new Qt3DRender::QDiffuseMapMaterial();
        QTest::newRow("QDiffuseMapMaterial") << material;
        material = new Qt3DRender::QDiffuseSpecularMapMaterial();
        QTest::newRow("QDiffuseMapSpecularMaterial") << material;
        material = new Qt3DRender::QPerVertexColorMaterial();
        QTest::newRow("QPerVertexColorMaterial") << material;
        material = new Qt3DRender::QNormalDiffuseMapMaterial();
        QTest::newRow("QNormalDiffuseMapMaterial") << material;
        material = new Qt3DRender::QNormalDiffuseMapAlphaMaterial();
        QTest::newRow("QNormalDiffuseMapAlphaMaterial") << material;
        material = new Qt3DRender::QNormalDiffuseSpecularMapMaterial();
        QTest::newRow("QNormalDiffuseSpecularMapMaterial") << material;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QMaterial *, material);

        // WHEN
        Qt3DRender::QMaterial *clone = static_cast<Qt3DRender::QMaterial *>(QNode::clone(material));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        compareParameters(material->parameters(), clone->parameters());
        compareEffects(material->effect(), clone->effect());
    }

    void checkEffectUpdate()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QMaterial> material(new Qt3DRender::QMaterial());
        TestArbiter arbiter(material.data());

        // WHEN
        Qt3DRender::QEffect effect;
        material->setEffect(&effect);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "effect");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), effect.id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // GIVEN
        QScopedPointer<TestMaterial> material2(new TestMaterial());
        TestArbiter arbiter2(material2.data());

        // WHEN
        material2->setEffect(&effect);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter2.events.size(), 1);
        change = arbiter2.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "effect");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), effect.id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
    }

    void checkDynamicParametersAddedUpdates()
    {
        // GIVEN
        TestMaterial *material = new TestMaterial();
        TestArbiter arbiter(material);

        // WHEN (add parameter to material)
        Qt3DRender::QParameter *param = new Qt3DRender::QParameter("testParamMaterial", QVariant::fromValue(383.0f));
        material->addParameter(param);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), param->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN (add parameter to effect)
        param = new Qt3DRender::QParameter("testParamEffect", QVariant::fromValue(383.0f));
        material->effect()->addParameter(param);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), param->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN (add parameter to technique)
        param = new Qt3DRender::QParameter("testParamTechnique", QVariant::fromValue(383.0f));
        material->m_technique->addParameter(param);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), param->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN (add parameter to renderpass)
        param = new Qt3DRender::QParameter("testParamRenderPass", QVariant::fromValue(383.0f));
        material->m_renderPass->addParameter(param);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), param->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();
    }

    void checkShaderProgramUpdates()
    {
        // GIVEN
        TestMaterial *material = new TestMaterial();
        TestArbiter arbiter(material);

        // WHEN
        const QByteArray vertexCode = QByteArrayLiteral("new vertex shader code");
        material->m_shaderProgram->setVertexShaderCode(vertexCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "vertexShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), vertexCode);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray fragmentCode = QByteArrayLiteral("new fragment shader code");
        material->m_shaderProgram->setFragmentShaderCode(fragmentCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "fragmentShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), fragmentCode);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray geometryCode = QByteArrayLiteral("new geometry shader code");
        material->m_shaderProgram->setGeometryShaderCode(geometryCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "geometryShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), geometryCode);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray computeCode = QByteArrayLiteral("new compute shader code");
        material->m_shaderProgram->setComputeShaderCode(computeCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "computeShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), computeCode);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray tesselControlCode = QByteArrayLiteral("new tessellation control shader code");
        material->m_shaderProgram->setTessellationControlShaderCode(tesselControlCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "tessellationControlShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), tesselControlCode);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray tesselEvalCode = QByteArrayLiteral("new tessellation eval shader code");
        material->m_shaderProgram->setTessellationEvaluationShaderCode(tesselEvalCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "tessellationEvaluationShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), tesselEvalCode);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QMaterial)

#include "tst_qmaterial.moc"
