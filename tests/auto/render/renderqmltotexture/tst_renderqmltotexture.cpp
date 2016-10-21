/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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
#include <Qt3DRender/qrenderqmltotexture.h>
#include <Qt3DRender/private/qrenderqmltotexture_p.h>
#include <Qt3DRender/private/renderqmltotexture_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <qbackendnodetester.h>
#include "testrenderer.h"

class tst_RenderQmlToTexture : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::RenderQmlToTexture backendRenderQmlToTexture;

        // THEN
        QCOMPARE(backendRenderQmlToTexture.isEnabled(), false);
        QVERIFY(backendRenderQmlToTexture.peerId().isNull());
        QCOMPARE(backendRenderQmlToTexture.m_context, nullptr);
        QCOMPARE(backendRenderQmlToTexture.m_graphicsContext, nullptr);
        QCOMPARE(backendRenderQmlToTexture.m_renderThread, nullptr);
        QCOMPARE(backendRenderQmlToTexture.m_textureId, Qt3DCore::QNodeId());
        QCOMPARE(backendRenderQmlToTexture.m_texture, nullptr);
        QCOMPARE(backendRenderQmlToTexture.m_initialized, false);
        QCOMPARE(backendRenderQmlToTexture.m_renderInitialized, false);
        QCOMPARE(backendRenderQmlToTexture.m_renderOnce, false);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DRender::QRenderQmlToTexture renderQmlToTexture;

        {
            // WHEN
            Qt3DRender::Render::RenderQmlToTexture backendRenderQmlToTexture;
            simulateInitialization(&renderQmlToTexture, &backendRenderQmlToTexture);

            // THEN
            QCOMPARE(backendRenderQmlToTexture.isEnabled(), true);
            QCOMPARE(backendRenderQmlToTexture.peerId(), renderQmlToTexture.id());
            QCOMPARE(backendRenderQmlToTexture.m_textureId, Qt3DCore::QNodeId());
            QCOMPARE(backendRenderQmlToTexture.m_sharedObject.data(), nullptr);
            QCOMPARE(backendRenderQmlToTexture.m_renderOnce, false);
        }
        {
            // WHEN
            Qt3DRender::Render::RenderQmlToTexture backendRenderQmlToTexture;
            renderQmlToTexture.setEnabled(false);
            simulateInitialization(&renderQmlToTexture, &backendRenderQmlToTexture);

            // THEN
            QCOMPARE(backendRenderQmlToTexture.peerId(), renderQmlToTexture.id());
            QCOMPARE(backendRenderQmlToTexture.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::RenderQmlToTexture backendRenderQmlToTexture;
        TestRenderer renderer;
        QScopedPointer<Qt3DRender::QTexture2D> texture(new Qt3DRender::QTexture2D());
        backendRenderQmlToTexture.setRenderer(&renderer);

        {
             // WHEN
             const bool newValue = false;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("enabled");
             change->setValue(newValue);
             backendRenderQmlToTexture.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendRenderQmlToTexture.isEnabled(), newValue);
        }
        {
             // WHEN
             const Qt3DCore::QNodeId newValue = texture.data()->id();
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("textureId");
             change->setValue(QVariant::fromValue(newValue));
             backendRenderQmlToTexture.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendRenderQmlToTexture.m_textureId, newValue);
        }
        {
             // WHEN
             const QSharedPointer<Qt3DRender::RenderQmlToTextureSharedObject> newValue
                     = QSharedPointer<Qt3DRender::RenderQmlToTextureSharedObject>(new Qt3DRender::RenderQmlToTextureSharedObject(nullptr));
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("sharedObject");
             change->setValue(QVariant::fromValue(newValue));
             backendRenderQmlToTexture.sceneChangeEvent(change);

             // THEN
             QCOMPARE(backendRenderQmlToTexture.m_sharedObject, newValue);
             QCOMPARE(backendRenderQmlToTexture.m_sharedObject.data(), newValue.data());
        }
        {
             // WHEN
             const bool newValue = true;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("renderOnce");
             change->setValue(QVariant::fromValue(newValue));
             backendRenderQmlToTexture.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendRenderQmlToTexture.m_renderOnce, newValue);
        }
    }

};

QTEST_MAIN(tst_RenderQmlToTexture)

#include "tst_renderqmltotexture.moc"
