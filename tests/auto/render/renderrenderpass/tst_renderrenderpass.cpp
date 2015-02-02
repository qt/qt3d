/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <Qt3DRenderer/private/renderrenderpass_p.h>

#include <Qt3DCore/QScenePropertyChange>

#include <Qt3DRenderer/QAnnotation>
#include <Qt3DRenderer/QParameterMapping>
#include <Qt3DRenderer/QRenderPass>
#include <Qt3DRenderer/QShaderProgram>

#include <Qt3DRenderer/QAlphaCoverage>
#include <Qt3DRenderer/QAlphaTest>
#include <Qt3DRenderer/QBlendEquation>
#include <Qt3DRenderer/QBlendState>
#include <Qt3DRenderer/QColorMask>
#include <Qt3DRenderer/QCullFace>
#include <Qt3DRenderer/QDepthMask>
#include <Qt3DRenderer/QDepthTest>
#include <Qt3DRenderer/QDithering>
#include <Qt3DRenderer/QFrontFace>
#include <Qt3DRenderer/QPolygonOffset>
#include <Qt3DRenderer/QScissorTest>
#include <Qt3DRenderer/QStencilTest>

using namespace Qt3D;
using namespace Qt3D::Render;

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
        RenderRenderPass backend;

        // THEN
        QVERIFY(backend.shaderProgram().isNull());
        QVERIFY(backend.annotations().isEmpty());
        QVERIFY(backend.bindings().isEmpty());
        QVERIFY(backend.renderStates().isEmpty());
    }

    void shouldHavePropertiesMirroringItsPeer_data()
    {
        QTest::addColumn<QRenderState*>("state");

        QRenderState *state = Q_NULLPTR;

        state = new QAlphaCoverage;
        QTest::newRow("alphacoverage") << state;

        state = new QAlphaTest;
        QTest::newRow("alphatest") << state;

        state = new QBlendEquation;
        QTest::newRow("blendequation") << state;

        state = new QBlendState;
        QTest::newRow("blendstate") << state;

        state = new QColorMask;
        QTest::newRow("colormask") << state;

        state = new QCullFace;
        QTest::newRow("cullface") << state;

        state = new QDepthMask;
        QTest::newRow("depthmask") << state;

        state = new QDepthTest;
        QTest::newRow("depthtest") << state;

        state = new QDithering;
        QTest::newRow("dithering") << state;

        state = new QFrontFace;
        QTest::newRow("frontface") << state;

        state = new QPolygonOffset;
        QTest::newRow("polygonoffset") << state;

        state = new QScissorTest;
        QTest::newRow("scissortest") << state;

        state = new QStencilTest;
        QTest::newRow("stenciltest") << state;
    }

    void shouldHavePropertiesMirroringItsPeer()
    {
        // GIVEN
        QRenderPass frontend;
        frontend.setShaderProgram(new QShaderProgram(&frontend));

        frontend.addAnnotation(new QAnnotation(&frontend));

        frontend.addBinding(new QParameterMapping(&frontend));

        QFETCH(QRenderState*, state);
        state->setParent(&frontend);
        frontend.addRenderState(state);

        RenderRenderPass backend;

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

        QCOMPARE(backend.renderStates().size(), 1);
        QCOMPARE(backend.renderStates().first(), state);
    }

    void shouldHandleShaderPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QShaderProgram> shader(new QShaderProgram);

        QNode *node = Q_NULLPTR;
        RenderRenderPass backend;

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, node));
        addChange->setValue(QVariant::fromValue(shader->id()));
        addChange->setPropertyName(QByteArrayLiteral("shaderProgram"));
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.shaderProgram(), shader->id());

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, node));
        removeChange->setValue(QVariant::fromValue(shader->id()));
        removeChange->setPropertyName(QByteArrayLiteral("shaderProgram"));
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.shaderProgram().isNull());
    }

    void shouldHandleAnnotationsPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QAnnotation> annotation(new QAnnotation);

        QNode *node = Q_NULLPTR;
        RenderRenderPass backend;

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, node));
        addChange->setValue(QVariant::fromValue(annotation->id()));
        addChange->setPropertyName(QByteArrayLiteral("annotation"));
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.annotations().size(), 1);
        QCOMPARE(backend.annotations().first(), annotation->id());

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, node));
        removeChange->setValue(QVariant::fromValue(annotation->id()));
        removeChange->setPropertyName(QByteArrayLiteral("annotation"));
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.annotations().isEmpty());
    }

    void shouldHandleBindingsPropertyChangeEvents()
    {
        // GIVEN
        QScopedPointer<QParameterMapping> binding(new QParameterMapping);

        QNode *node = Q_NULLPTR;
        RenderRenderPass backend;

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, node));
        addChange->setValue(QVariant::fromValue(binding.data()));
        addChange->setPropertyName(QByteArrayLiteral("binding"));
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.bindings().size(), 1);
        QCOMPARE(backend.bindings().first().id(), binding->id());
        QCOMPARE(backend.bindings().first().bindingType(), binding->bindingType());
        QCOMPARE(backend.bindings().first().parameterName(), binding->parameterName());
        QCOMPARE(backend.bindings().first().shaderVariableName(), binding->shaderVariableName());

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, node));
        removeChange->setValue(QVariant::fromValue(binding->id()));
        removeChange->setPropertyName(QByteArrayLiteral("binding"));
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.bindings().isEmpty());
    }

    void shouldHandlePropertyChangeEvents_data()
    {
        shouldHavePropertiesMirroringItsPeer_data();
    }

    void shouldHandlePropertyChangeEvents()
    {
        // GIVEN
        QFETCH(QRenderState*, state);
        QScopedPointer<QRenderState> statePtr(state);
        Q_UNUSED(statePtr);

        QNode *node = Q_NULLPTR;
        RenderRenderPass backend;

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(NodeAdded, node));
        addChange->setValue(QVariant::fromValue(state));
        addChange->setPropertyName(QByteArrayLiteral("renderState"));
        backend.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(backend.renderStates().size(), 1);
        QCOMPARE(backend.renderStates().first(), state);

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(NodeRemoved, node));
        removeChange->setValue(QVariant::fromValue(state->id()));
        removeChange->setPropertyName(QByteArrayLiteral("renderState"));
        backend.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(backend.renderStates().isEmpty());
    }
};

QTEST_APPLESS_MAIN(tst_RenderRenderPass)

#include "tst_renderrenderpass.moc"
