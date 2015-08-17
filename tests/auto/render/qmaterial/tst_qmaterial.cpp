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
#include <Qt3DCore/private/qpostman_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/QScenePropertyChange>

#include <Qt3DRenderer/QEffect>
#include <Qt3DRenderer/QMaterial>
#include <Qt3DRenderer/QParameter>
#include <Qt3DRenderer/QTechnique>
#include <Qt3DRenderer/QRenderPass>
#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/QParameterMapping>
#include <Qt3DRenderer/QDiffuseMapMaterial>
#include <Qt3DRenderer/QPerVertexColorMaterial>
#include <Qt3DRenderer/QNormalDiffuseMapMaterial>
#include <Qt3DRenderer/QDiffuseSpecularMapMaterial>
#include <Qt3DRenderer/QNormalDiffuseMapAlphaMaterial>
#include <Qt3DRenderer/QNormalDiffuseSpecularMapMaterial>

class TestMaterial : public Qt3D::QMaterial
{
public:
    explicit TestMaterial(Qt3D::QNode *parent = 0)
        : Qt3D::QMaterial(parent)
        , m_effect(new Qt3D::QEffect(this))
        , m_technique(new Qt3D::QTechnique(this))
        , m_renderPass(new Qt3D::QRenderPass(this))
        , m_shaderProgram(new Qt3D::QShaderProgram(this))
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

    Qt3D::QEffect *m_effect;
    Qt3D::QTechnique *m_technique;
    Qt3D::QRenderPass *m_renderPass;
    Qt3D::QShaderProgram *m_shaderProgram;
};

class TestArbiter;

class TestPostman : public Qt3D::QAbstractPostman
{
public:
    TestPostman(TestArbiter *arbiter)
        : m_arbiter(arbiter)
    {}

    void sceneChangeEvent(const Qt3D::QSceneChangePtr &) Q_DECL_FINAL
    {}

    void setScene(Qt3D::QScene *) Q_DECL_FINAL
    {}

    void notifyBackend(const Qt3D::QSceneChangePtr &e) Q_DECL_FINAL;

private:
    TestArbiter *m_arbiter;
};

class TestArbiter : public Qt3D::QAbstractArbiter
{
public:
    TestArbiter(Qt3D::QNode *node)
        : m_postman(new TestPostman(this))
        , m_node(node)
    {
        assignArbiter(m_node);
    }

    ~TestArbiter()
    {
        Qt3D::QNodePrivate::get(m_node)->setArbiter(Q_NULLPTR);
    }

    void sceneChangeEvent(const Qt3D::QSceneChangePtr &e) Q_DECL_FINAL
    {
        events.push_back(e);
    }

    void sceneChangeEventWithLock(const Qt3D::QSceneChangePtr &e) Q_DECL_FINAL
    {
        events.push_back(e);
    }

    void sceneChangeEventWithLock(const Qt3D::QSceneChangeList &e) Q_DECL_FINAL
    {
        events += QVector<Qt3D::QSceneChangePtr>::fromStdVector(e);
    }

    Qt3D::QAbstractPostman *postman() const Q_DECL_FINAL
    {
        return m_postman;
    }

    QVector<Qt3D::QSceneChangePtr> events;

private:
    TestPostman *m_postman;
    Qt3D::QNode *m_node;

    void assignArbiter(Qt3D::QNode *node)
    {
        Qt3D::QNodePrivate::get(node)->setArbiter(this);
        Q_FOREACH (Qt3D::QNode *n, node->childrenNodes())
            assignArbiter(n);
    }
};

void TestPostman::notifyBackend(const Qt3D::QSceneChangePtr &e)
{
    m_arbiter->sceneChangeEventWithLock(e);
}

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QMaterial : public Qt3D::QNode
{
    Q_OBJECT
public:
    ~tst_QMaterial()
    {
        QNode::cleanup();
    }

private:

    void compareEffects(const Qt3D::QEffect *original, const Qt3D::QEffect *clone)
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

    void compareTechniques(const Qt3D::QTechnique *original, const Qt3D::QTechnique *clone)
    {
        QCOMPARE(original->id(), clone->id());

        compareParameters(original->parameters(), clone->parameters());

        const int passesCount = original->renderPasses().size();
        QCOMPARE(passesCount, clone->renderPasses().size());

        for (int i = 0; i < passesCount; ++i)
            compareRenderPasses(original->renderPasses().at(i), clone->renderPasses().at(i));
    }

    void compareRenderPasses(const Qt3D::QRenderPass *original, const Qt3D::QRenderPass *clone)
    {
        QCOMPARE(original->id(), clone->id());

        compareParameters(original->parameters(), clone->parameters());
        compareRenderStates(original->renderStates(), clone->renderStates());
        compareAnnotations(original->annotations(), clone->annotations());
        compareBindings(original->bindings(), clone->bindings());
        compareShaderPrograms(original->shaderProgram(), clone->shaderProgram());
    }

    void compareParameters(const Qt3D::ParameterList &original, const Qt3D::ParameterList &clone)
    {
        QCOMPARE(original.size(), clone.size());
        const int parametersCount = original.size();
        for (int i = 0; i < parametersCount; ++i) {
            const Qt3D::QParameter *originParam = original.at(i);
            const Qt3D::QParameter *cloneParam = clone.at(i);
            QCOMPARE(originParam->id(), cloneParam->id());
            QCOMPARE(cloneParam->name(), originParam->name());
            QCOMPARE(cloneParam->value(), originParam->value());
        }
    }

    void compareAnnotations(const QList<Qt3D::QAnnotation *> &original, const QList<Qt3D::QAnnotation *> &clone)
    {
        const int annotationsCount = original.size();
        QCOMPARE(annotationsCount, clone.size());

        for (int i = 0; i < annotationsCount; ++i) {
            const Qt3D::QAnnotation *origAnnotation = original.at(i);
            const Qt3D::QAnnotation *cloneAnnotation = clone.at(i);
            QCOMPARE(origAnnotation->id(), cloneAnnotation->id());
            QCOMPARE(origAnnotation->name(), cloneAnnotation->name());
            QCOMPARE(origAnnotation->value(), cloneAnnotation->value());
        }
    }

    void compareBindings(const QList<Qt3D::QParameterMapping *> &original, const QList<Qt3D::QParameterMapping *> &clone)
    {
        const int bindingsCount = original.size();
        QCOMPARE(bindingsCount, clone.size());

        for (int i = 0; i < bindingsCount; ++i) {
            const Qt3D::QParameterMapping *origMapping = original.at(i);
            const Qt3D::QParameterMapping *cloneMapping = clone.at(i);

            QCOMPARE(origMapping->id(), cloneMapping->id());
            QCOMPARE(origMapping->bindingType(), cloneMapping->bindingType());
            QCOMPARE(origMapping->parameterName(), cloneMapping->parameterName());
            QCOMPARE(origMapping->shaderVariableName(), cloneMapping->shaderVariableName());
        }
    }

    void compareRenderStates(const QList<Qt3D::QRenderState *> &original, const QList<Qt3D::QRenderState *> &clone)
    {
        const int renderStatesCount = original.size();
        QCOMPARE(renderStatesCount, clone.size());

        for (int i = 0; i < renderStatesCount; ++i) {
            const Qt3D::QRenderState *originState = original.at(i);
            const Qt3D::QRenderState *cloneState = clone.at(i);
            QCOMPARE(originState->id(), originState->id());
            QVERIFY(originState->type() == cloneState->type());
        }
    }

    void compareShaderPrograms(const Qt3D::QShaderProgram *original, const Qt3D::QShaderProgram *clone)
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
        QTest::addColumn<Qt3D::QMaterial *>("material");

        Qt3D::QMaterial *material = new Qt3D::QMaterial();
        QTest::newRow("empty material") << material;
        material = new TestMaterial();
        QTest::newRow("test material") << material;
        material = new Qt3D::QPhongMaterial();
        QTest::newRow("QPhongMaterial") << material;
        material = new Qt3D::QDiffuseMapMaterial();
        QTest::newRow("QDiffuseMapMaterial") << material;
        material = new Qt3D::QDiffuseSpecularMapMaterial();
        QTest::newRow("QDiffuseMapSpecularMaterial") << material;
        material = new Qt3D::QPerVertexColorMaterial();
        QTest::newRow("QPerVertexColorMaterial") << material;
        material = new Qt3D::QNormalDiffuseMapMaterial();
        QTest::newRow("QNormalDiffuseMapMaterial") << material;
        material = new Qt3D::QNormalDiffuseMapAlphaMaterial();
        QTest::newRow("QNormalDiffuseMapAlphaMaterial") << material;
        material = new Qt3D::QNormalDiffuseSpecularMapMaterial();
        QTest::newRow("QNormalDiffuseSpecularMapMaterial") << material;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3D::QMaterial *, material);

        // WHEN
        Qt3D::QMaterial *clone = static_cast<Qt3D::QMaterial *>(QNode::clone(material));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        compareParameters(material->parameters(), clone->parameters());
        compareEffects(material->effect(), clone->effect());
    }

    void checkEffectUpdate()
    {
        // GIVEN
        QScopedPointer<Qt3D::QMaterial> material(new Qt3D::QMaterial());
        TestArbiter arbiter(material.data());

        // WHEN
        Qt3D::QEffect effect;
        material->setEffect(&effect);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3D::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "effect");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), effect.id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

        arbiter.events.clear();

        // GIVEN
        QScopedPointer<TestMaterial> material2(new TestMaterial());
        TestArbiter arbiter2(material2.data());

        // WHEN
        Qt3D::QEffect *oldEffect = material2->effect();
        material2->setEffect(&effect);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter2.events.size(), 2);
        change = arbiter2.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "effect");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), oldEffect->id());
        QCOMPARE(change->type(), Qt3D::NodeRemoved);

        change = arbiter2.events.last().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "effect");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), effect.id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);
    }

    void checkDynamicParametersAddedUpdates()
    {
        // GIVEN
        TestMaterial *material = new TestMaterial();
        TestArbiter arbiter(material);

        // WHEN (add parameter to material)
        Qt3D::QParameter *param = new Qt3D::QParameter("testParamMaterial", QVariant::fromValue(383.0f));
        material->addParameter(param);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3D::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), param->id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

        arbiter.events.clear();

        // WHEN (add parameter to effect)
        param = new Qt3D::QParameter("testParamEffect", QVariant::fromValue(383.0f));
        material->effect()->addParameter(param);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), param->id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

        arbiter.events.clear();

        // WHEN (add parameter to technique)
        param = new Qt3D::QParameter("testParamTechnique", QVariant::fromValue(383.0f));
        material->m_technique->addParameter(param);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), param->id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

        arbiter.events.clear();

        // WHEN (add parameter to renderpass)
        param = new Qt3D::QParameter("testParamRenderPass", QVariant::fromValue(383.0f));
        material->m_renderPass->addParameter(param);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), param->id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

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
        Qt3D::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "vertexShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), vertexCode);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray fragmentCode = QByteArrayLiteral("new fragment shader code");
        material->m_shaderProgram->setFragmentShaderCode(fragmentCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "fragmentShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), fragmentCode);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray geometryCode = QByteArrayLiteral("new geometry shader code");
        material->m_shaderProgram->setGeometryShaderCode(geometryCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "geometryShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), geometryCode);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray computeCode = QByteArrayLiteral("new compute shader code");
        material->m_shaderProgram->setComputeShaderCode(computeCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "computeShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), computeCode);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray tesselControlCode = QByteArrayLiteral("new tessellation control shader code");
        material->m_shaderProgram->setTessellationControlShaderCode(tesselControlCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "tessellationControlShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), tesselControlCode);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        const QByteArray tesselEvalCode = QByteArrayLiteral("new tessellation eval shader code");
        material->m_shaderProgram->setTessellationEvaluationShaderCode(tesselEvalCode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "tessellationEvaluationShaderCode");
        QCOMPARE(change->value().value<QByteArray>(), tesselEvalCode);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3D::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QMaterial)

#include "tst_qmaterial.moc"
