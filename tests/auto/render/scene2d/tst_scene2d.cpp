/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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
#include <Qt3DQuickScene2D/qscene2d.h>
#include <private/qscene2d_p.h>
#include <private/scene2d_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <qbackendnodetester.h>
#include "testrenderer.h"

using namespace Qt3DRender::Quick;
using namespace Qt3DRender::Render::Quick;

class tst_Scene2D : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Scene2D backendScene2d;

        // THEN
        QCOMPARE(backendScene2d.isEnabled(), false);
        QVERIFY(backendScene2d.peerId().isNull());
        QCOMPARE(backendScene2d.m_context, nullptr);
        QCOMPARE(backendScene2d.m_shareContext, nullptr);
        QCOMPARE(backendScene2d.m_renderThread, nullptr);
        QCOMPARE(backendScene2d.m_outputId, Qt3DCore::QNodeId());
        QCOMPARE(backendScene2d.m_initialized, false);
        QCOMPARE(backendScene2d.m_renderInitialized, false);
        QCOMPARE(backendScene2d.m_renderPolicy, QScene2D::Continuous);
        backendScene2d.cleanup();
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DRender::Quick::QScene2D frontend;
        TestRenderer renderer;

        {
            // WHEN
            QScopedPointer<Scene2D> backendScene2d(new Scene2D());
            backendScene2d->setRenderer(&renderer);
            simulateInitialization(&frontend, backendScene2d.data());

            // THEN
            QCOMPARE(backendScene2d->isEnabled(), true);
            QCOMPARE(backendScene2d->peerId(), frontend.id());
            QCOMPARE(backendScene2d->m_outputId, Qt3DCore::QNodeId());
            QVERIFY(backendScene2d->m_sharedObject.data() != nullptr);
            QCOMPARE(backendScene2d->m_renderPolicy, QScene2D::Continuous);
            backendScene2d->cleanup();
        }
        {
            // WHEN
            QScopedPointer<Scene2D> backendScene2d(new Scene2D());
            frontend.setEnabled(false);
            backendScene2d->setRenderer(&renderer);
            simulateInitialization(&frontend, backendScene2d.data());

            // THEN
            QCOMPARE(backendScene2d->peerId(), frontend.id());
            QCOMPARE(backendScene2d->isEnabled(), false);
            backendScene2d->cleanup();
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        QScopedPointer<Scene2D> backendScene2d(new Scene2D());
        TestRenderer renderer;
        QScopedPointer<Qt3DRender::QRenderTargetOutput> output(new Qt3DRender::QRenderTargetOutput());
        backendScene2d->setRenderer(&renderer);

        {
             // WHEN
             const bool newValue = false;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("enabled");
             change->setValue(newValue);
             backendScene2d->sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendScene2d->isEnabled(), newValue);
        }
        {
             // WHEN
             const Qt3DCore::QNodeId newValue = output.data()->id();
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("output");
             change->setValue(QVariant::fromValue(newValue));
             backendScene2d->sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendScene2d->m_outputId, newValue);
        }
        {
             // WHEN
             const QSharedPointer<Qt3DRender::Quick::Scene2DSharedObject> newValue
                     = QSharedPointer<Qt3DRender::Quick::Scene2DSharedObject>(
                         new Qt3DRender::Quick::Scene2DSharedObject(nullptr));
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("sharedObject");
             change->setValue(QVariant::fromValue(newValue));
             backendScene2d->sceneChangeEvent(change);

             // THEN
             QCOMPARE(backendScene2d->m_sharedObject, newValue);
             QCOMPARE(backendScene2d->m_sharedObject.data(), newValue.data());
        }
        {
             // WHEN
             const QScene2D::RenderPolicy newValue = QScene2D::SingleShot;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("renderPolicy");
             change->setValue(QVariant::fromValue(newValue));
             backendScene2d->sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendScene2d->m_renderPolicy, newValue);
        }

        backendScene2d->cleanup();
    }

};

QTEST_MAIN(tst_Scene2D)

#include "tst_scene2d.moc"
