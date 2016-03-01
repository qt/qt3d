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

#include <QtTest/QtTest>
#include <Qt3DRender/private/renderpass_p.h>

#include <Qt3DCore/QScenePropertyChange>

#include <Qt3DRender/QAnnotation>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QParameter>

#include <Qt3DRender/QAlphaCoverage>
#include <Qt3DRender/QAlphaTest>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QColorMask>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QDepthMask>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QDithering>
#include <Qt3DRender/QFrontFace>
#include <Qt3DRender/QPolygonOffset>
#include <Qt3DRender/QScissorTest>
#include <Qt3DRender/QStencilTest>
#include <Qt3DRender/QStencilTestArguments>
#include <Qt3DRender/QStencilMask>
#include <Qt3DRender/QStencilOp>
#include <Qt3DRender/QStencilOpSeparate>
#include <Qt3DRender/QClipPlane>

#include <Qt3DRender/private/renderstates_p.h>
#include <Qt3DRender/private/managers_p.h>

#include "testrenderer.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

class tst_RenderRenderPass : public QObject
{
    Q_OBJECT
public:
    tst_RenderRenderPass()
        : m_renderStateManager(new RenderStateManager())
    {
    }
    ~tst_RenderRenderPass() {}

private slots:
    void shouldHaveInitialState()
    {
        // GIVEN
        RenderPass backend;

        // THEN
        QVERIFY(backend.shaderProgram().isNull());
        QVERIFY(backend.annotations().isEmpty());
        QVERIFY(backend.renderStates(m_renderStateManager).isEmpty());
        QVERIFY(backend.parameters().isEmpty());
    }

    void shouldHavePropertiesMirroringItsPeer()
    {
        // GIVEN
        QRenderPass frontend;
        frontend.setShaderProgram(new QShaderProgram(&frontend));

        frontend.addAnnotation(new QAnnotation(&frontend));

        frontend.addParameter(new QParameter(&frontend));

        QRenderState *frontendState = new QBlendEquationArguments();
        frontendState->setParent(&frontend);
        frontend.addRenderState(frontendState);

        RenderPass backend;

        RenderStateNode *backendState = m_renderStateManager->getOrCreateResource(frontendState->id());
        backendState->setPeer(frontendState);

        // WHEN
        backend.setPeer(&frontend);

        // THEN
        QCOMPARE(backend.shaderProgram(), frontend.shaderProgram()->id());

        QCOMPARE(backend.annotations().size(), 1);
        QCOMPARE(backend.annotations().first(), frontend.annotations().first()->id());

        QCOMPARE(backend.parameters().size(), 1);
        QCOMPARE(backend.parameters().first(), frontend.parameters().first()->id());

        QCOMPARE(backend.renderStates(m_renderStateManager).size(), 1);
        QCOMPARE(backend.renderStates(m_renderStateManager).first(), backendState);
    }

    void shouldHandleShaderPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QShaderProgram> shader(new QShaderProgram);

        RenderPass backend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, shader->id()));
        addChange->setValue(QVariant::fromValue(shader->id()));
        addChange->setPropertyName("shaderProgram");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.shaderProgram(), shader->id());
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, shader->id()));
        removeChange->setValue(QVariant::fromValue(shader->id()));
        removeChange->setPropertyName("shaderProgram");
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.shaderProgram().isNull());
    }

    void shouldHandleAnnotationsPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QAnnotation> annotation(new QAnnotation);

        RenderPass backend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, annotation->id()));
        addChange->setValue(QVariant::fromValue(annotation->id()));
        addChange->setPropertyName("annotation");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.annotations().size(), 1);
        QCOMPARE(backend.annotations().first(), annotation->id());
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, annotation->id()));
        removeChange->setValue(QVariant::fromValue(annotation->id()));
        removeChange->setPropertyName("annotation");
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.annotations().isEmpty());
    }

    void shouldHandleParametersPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QParameter> parameter(new QParameter);

        RenderPass backend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, parameter->id()));
        addChange->setValue(QVariant::fromValue(parameter->id()));
        addChange->setPropertyName("parameter");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.parameters().size(), 1);
        QCOMPARE(backend.parameters().first(), parameter->id());
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, parameter->id()));
        removeChange->setValue(QVariant::fromValue(parameter->id()));
        removeChange->setPropertyName("parameter");
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.parameters().isEmpty());
    }

    void shouldHandlePropertyChangeEvents()
    {
        QRenderState *frontendState = new QBlendEquationArguments();
        QNodePtr frontendStatePtr(frontendState);

        RenderPass backend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);

        RenderStateNode *backendState = m_renderStateManager->getOrCreateResource(frontendState->id());
        backendState->setPeer(frontendState);

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, frontendState->id()));
        addChange->setValue(QVariant::fromValue(frontendStatePtr));
        addChange->setPropertyName("renderState");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.renderStates(m_renderStateManager).size(), 1);
        QCOMPARE(backend.renderStates(m_renderStateManager).first(), backendState);
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, frontendState->id()));
        removeChange->setValue(QVariant::fromValue(frontendState->id()));
        removeChange->setPropertyName("renderState");
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.renderStates(m_renderStateManager).isEmpty());
    }

private:
    RenderStateManager *m_renderStateManager;
};

QTEST_APPLESS_MAIN(tst_RenderRenderPass)

#include "tst_renderpass.moc"
