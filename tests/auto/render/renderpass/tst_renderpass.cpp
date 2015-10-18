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

#include <QtTest/QtTest>
#include <Qt3DRender/private/renderpass_p.h>

#include <Qt3DCore/QScenePropertyChange>

#include <Qt3DRender/QAnnotation>
#include <Qt3DRender/QParameterMapping>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QParameter>

#include <Qt3DRender/QAlphaCoverage>
#include <Qt3DRender/QAlphaTest>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendState>
#include <Qt3DRender/QColorMask>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QDepthMask>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QDithering>
#include <Qt3DRender/QFrontFace>
#include <Qt3DRender/QPolygonOffset>
#include <Qt3DRender/QScissorTest>
#include <Qt3DRender/QStencilTest>
#include <Qt3DRender/QStencilTestSeparate>
#include <Qt3DRender/QStencilMask>
#include <Qt3DRender/QStencilOp>
#include <Qt3DRender/QStencilOpSeparate>
#include <Qt3DRender/QClipPlane>

#include <Qt3DRender/private/renderstates_p.h>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

Q_DECLARE_METATYPE(RenderState*)

class tst_RenderRenderPass : public QObject
{
    Q_OBJECT
public:
    tst_RenderRenderPass() {}
    ~tst_RenderRenderPass() {}

private slots:
    void shouldHaveInitialState()
    {
        // GIVEN
        RenderPass backend;

        // THEN
        QVERIFY(backend.shaderProgram().isNull());
        QVERIFY(backend.annotations().isEmpty());
        QVERIFY(backend.bindings().isEmpty());
        QVERIFY(backend.renderStates().isEmpty());
        QVERIFY(backend.parameters().isEmpty());
    }

    void shouldHavePropertiesMirroringItsPeer_data()
    {
        QTest::addColumn<QRenderState*>("frontendState");
        QTest::addColumn<RenderState*>("backendState");

        QRenderState *frontendState = Q_NULLPTR;
        RenderState *backendState = Q_NULLPTR;

        QAlphaCoverage *alphaCoverage = new QAlphaCoverage;
        frontendState = alphaCoverage;
        backendState = AlphaCoverage::getOrCreate();
        QTest::newRow("alphacoverage") << frontendState << backendState;

        QAlphaTest *alphaTest = new QAlphaTest;
        frontendState = alphaTest;
        backendState = AlphaFunc::getOrCreate(alphaTest->func(), alphaTest->clamp());
        QTest::newRow("alphatest") << frontendState << backendState;

        QBlendEquation *blendEquation = new QBlendEquation;
        frontendState = blendEquation;
        backendState = BlendEquation::getOrCreate(blendEquation->mode());
        QTest::newRow("blendequation") << frontendState << backendState;

        QBlendState *blendState = new QBlendState;
        frontendState = blendState;
        backendState = BlendState::getOrCreate(blendState->srcRGB(), blendState->dstRGB());
        QTest::newRow("blendstate") << frontendState << backendState;

        QColorMask *colorMask = new QColorMask;
        frontendState = colorMask;
        backendState = ColorMask::getOrCreate(colorMask->isRed(),
                                              colorMask->isGreen(),
                                              colorMask->isBlue(),
                                              colorMask->isAlpha());
        QTest::newRow("colormask") << frontendState << backendState;

        QCullFace *cullFace = new QCullFace;
        frontendState = cullFace;
        backendState = CullFace::getOrCreate(cullFace->mode());
        QTest::newRow("cullface") << frontendState << backendState;

        QDepthMask *depthMask = new QDepthMask;
        frontendState = depthMask;
        backendState = DepthMask::getOrCreate(depthMask->mask());
        QTest::newRow("depthmask") << frontendState << backendState;

        QDepthTest *depthTest = new QDepthTest;
        frontendState = depthTest;
        backendState = DepthTest::getOrCreate(depthTest->func());
        QTest::newRow("depthtest") << frontendState << backendState;

        QDithering *dithering = new QDithering;
        frontendState = dithering;
        backendState = Dithering::getOrCreate();
        QTest::newRow("dithering") << frontendState << backendState;

        QFrontFace *frontFace = new QFrontFace;
        frontendState = frontFace;
        backendState = FrontFace::getOrCreate(frontFace->direction());
        QTest::newRow("frontface") << frontendState << backendState;

        QPolygonOffset *polygonOffset = new QPolygonOffset;
        frontendState = polygonOffset;
        backendState = PolygonOffset::getOrCreate(polygonOffset->factor(),
                                                  polygonOffset->units());
        QTest::newRow("polygonoffset") << frontendState << backendState;

        QScissorTest *scissorTest = new QScissorTest;
        frontendState = scissorTest;
        backendState = ScissorTest::getOrCreate(scissorTest->left(),
                                                scissorTest->bottom(),
                                                scissorTest->width(),
                                                scissorTest->height());
        QTest::newRow("scissortest") << frontendState << backendState;

        QStencilTest *stencilTest = new QStencilTest;
        frontendState = stencilTest;
        backendState = StencilTest::getOrCreate(stencilTest->front()->func(),
                                                stencilTest->front()->ref(),
                                                stencilTest->front()->mask(),
                                                stencilTest->back()->func(),
                                                stencilTest->back()->ref(),
                                                stencilTest->back()->mask());
        QTest::newRow("stenciltest") << frontendState << backendState;

        QStencilMask *stencilMask = new QStencilMask;
        frontendState = stencilMask;
        backendState = StencilMask::getOrCreate(stencilMask->frontMask(), stencilMask->backMask());

        QTest::newRow("stencilmask") << frontendState << backendState;

        QStencilOp *stencilOp = new QStencilOp;
        frontendState = stencilOp;
        backendState = StencilOp::getOrCreate(stencilOp->front()->stencilFail(),
                                              stencilOp->front()->depthFail(),
                                              stencilOp->front()->stencilDepthPass(),
                                              stencilOp->back()->stencilFail(),
                                              stencilOp->back()->depthFail(),
                                              stencilOp->back()->stencilDepthPass());
        QTest::newRow("stencilop") << frontendState << backendState;

        QClipPlane *clipPlane = new QClipPlane;
        frontendState = clipPlane;
        backendState = ClipPlane::getOrCreate(clipPlane->plane());
    }

    void shouldHavePropertiesMirroringItsPeer()
    {
        // GIVEN
        QRenderPass frontend;
        frontend.setShaderProgram(new QShaderProgram(&frontend));

        frontend.addAnnotation(new QAnnotation(&frontend));

        frontend.addBinding(new QParameterMapping(&frontend));

        frontend.addParameter(new QParameter(&frontend));

        QFETCH(QRenderState*, frontendState);
        frontendState->setParent(&frontend);
        frontend.addRenderState(frontendState);

        RenderPass backend;

        QFETCH(RenderState*, backendState);

        // WHEN
        backend.setPeer(&frontend);

        // THEN
        QCOMPARE(backend.shaderProgram(), frontend.shaderProgram()->id());

        QCOMPARE(backend.annotations().size(), 1);
        QCOMPARE(backend.annotations().first(), frontend.annotations().first()->id());

        QCOMPARE(backend.bindings().size(), 1);
        QCOMPARE(backend.bindings().first().id(), frontend.bindings().first()->id());
        QCOMPARE(backend.bindings().first().bindingType(), frontend.bindings().first()->bindingType());
        QCOMPARE(backend.bindings().first().parameterName(), frontend.bindings().first()->parameterName());
        QCOMPARE(backend.bindings().first().shaderVariableName(), frontend.bindings().first()->shaderVariableName());

        QCOMPARE(backend.parameters().size(), 1);
        QCOMPARE(backend.parameters().first(), frontend.parameters().first()->id());

        QCOMPARE(backend.renderStates().size(), 1);
        QCOMPARE(backend.renderStates().first(), backendState);
    }

    void shouldHandleShaderPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QShaderProgram> shader(new QShaderProgram);

        RenderPass backend;

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, shader->id()));
        addChange->setValue(QVariant::fromValue(shader->id()));
        addChange->setPropertyName("shaderProgram");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.shaderProgram(), shader->id());

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

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, annotation->id()));
        addChange->setValue(QVariant::fromValue(annotation->id()));
        addChange->setPropertyName("annotation");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.annotations().size(), 1);
        QCOMPARE(backend.annotations().first(), annotation->id());

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, annotation->id()));
        removeChange->setValue(QVariant::fromValue(annotation->id()));
        removeChange->setPropertyName("annotation");
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.annotations().isEmpty());
    }

    void shouldHandleBindingsPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QParameterMapping> binding(new QParameterMapping);

        RenderPass backend;

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, binding->id()));
        addChange->setValue(QVariant::fromValue(binding.data()));
        addChange->setPropertyName("binding");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.bindings().size(), 1);
        QCOMPARE(backend.bindings().first().id(), binding->id());
        QCOMPARE(backend.bindings().first().bindingType(), binding->bindingType());
        QCOMPARE(backend.bindings().first().parameterName(), binding->parameterName());
        QCOMPARE(backend.bindings().first().shaderVariableName(), binding->shaderVariableName());

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, binding->id()));
        removeChange->setValue(QVariant::fromValue(binding->id()));
        removeChange->setPropertyName("binding");
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.bindings().isEmpty());
    }

    void shouldHandleParametersPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QParameter> parameter(new QParameter);

        RenderPass backend;

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, parameter->id()));
        addChange->setValue(QVariant::fromValue(parameter->id()));
        addChange->setPropertyName("parameter");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.parameters().size(), 1);
        QCOMPARE(backend.parameters().first(), parameter->id());

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, parameter->id()));
        removeChange->setValue(QVariant::fromValue(parameter->id()));
        removeChange->setPropertyName("parameter");
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.parameters().isEmpty());
    }

    void shouldHandlePropertyChangeEvents_data()
    {
        shouldHavePropertiesMirroringItsPeer_data();
    }

    void shouldHandlePropertyChangeEvents()
    {
        // GIVEN
        QFETCH(QRenderState*, frontendState);
        QNodePtr frontendStatePtr(frontendState);

        RenderPass backend;

        QFETCH(RenderState*, backendState);

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, QSceneChange::Node, frontendState->id()));
        addChange->setValue(QVariant::fromValue(frontendStatePtr));
        addChange->setPropertyName("renderState");
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.renderStates().size(), 1);
        QCOMPARE(backend.renderStates().first(), backendState);

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, frontendState->id()));
        removeChange->setValue(QVariant::fromValue(frontendState->id()));
        removeChange->setPropertyName("renderState");
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.renderStates().isEmpty());
    }
};

QTEST_APPLESS_MAIN(tst_RenderRenderPass)

#include "tst_renderpass.moc"
