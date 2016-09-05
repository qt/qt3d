/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
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
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/private/qtechnique_p.h>
#include <Qt3DRender/private/technique_p.h>
#include <Qt3DRender/private/shaderparameterpack_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_Technique : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::Technique backendTechnique;

        // THEN
        QCOMPARE(backendTechnique.isEnabled(), false);
        QVERIFY(backendTechnique.peerId().isNull());

        QCOMPARE(backendTechnique.graphicsApiFilter()->m_minor, 0);
        QCOMPARE(backendTechnique.graphicsApiFilter()->m_major, 0);
        QCOMPARE(backendTechnique.graphicsApiFilter()->m_profile, Qt3DRender::QGraphicsApiFilter::NoProfile);
        QCOMPARE(backendTechnique.graphicsApiFilter()->m_vendor, QString());
        QCOMPARE(backendTechnique.graphicsApiFilter()->m_extensions, QStringList());

        QCOMPARE(backendTechnique.parameters().size(), 0);
        QCOMPARE(backendTechnique.filterKeys().size(), 0);
        QCOMPARE(backendTechnique.renderPasses().size(), 0);
    }

    void checkCleanupState()
    {
        // GIVEN
        Qt3DRender::Render::Technique backendTechnique;

        // WHEN
        backendTechnique.setEnabled(true);

        {
            Qt3DRender::QTechnique technique;
            Qt3DRender::QRenderPass pass;
            Qt3DRender::QParameter parameter;
            Qt3DRender::QFilterKey filterKey;

            technique.addRenderPass(&pass);
            technique.addParameter(&parameter);
            technique.addFilterKey(&filterKey);

            simulateInitialization(&technique, &backendTechnique);
        }

        backendTechnique.cleanup();

        // THEN
        QCOMPARE(backendTechnique.isEnabled(), false);
        QCOMPARE(backendTechnique.parameters().size(), 0);
        QCOMPARE(backendTechnique.filterKeys().size(), 0);
        QCOMPARE(backendTechnique.renderPasses().size(), 0);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DRender::QTechnique technique;
        Qt3DRender::QRenderPass pass;
        Qt3DRender::QParameter parameter;
        Qt3DRender::QFilterKey filterKey;

        technique.addRenderPass(&pass);
        technique.addParameter(&parameter);
        technique.addFilterKey(&filterKey);

        {
            // WHEN
            Qt3DRender::Render::Technique backendTechnique;
            simulateInitialization(&technique, &backendTechnique);

            // THEN
            QCOMPARE(backendTechnique.isEnabled(), true);
            QCOMPARE(backendTechnique.peerId(), technique.id());
            Qt3DRender::GraphicsApiFilterData apiFilterData = Qt3DRender::QGraphicsApiFilterPrivate::get(technique.graphicsApiFilter())->m_data;
            QCOMPARE(*backendTechnique.graphicsApiFilter(), apiFilterData);
            QCOMPARE(backendTechnique.parameters().size(), 1);
            QCOMPARE(backendTechnique.parameters().first(), parameter.id());
            QCOMPARE(backendTechnique.filterKeys().size(), 1);
            QCOMPARE(backendTechnique.filterKeys().first(), filterKey.id());
            QCOMPARE(backendTechnique.renderPasses().size(), 1);
            QCOMPARE(backendTechnique.renderPasses().first(), pass.id());
        }
        {
            // WHEN
            Qt3DRender::Render::Technique backendTechnique;
            technique.setEnabled(false);
            simulateInitialization(&technique, &backendTechnique);

            // THEN
            QCOMPARE(backendTechnique.peerId(), technique.id());
            QCOMPARE(backendTechnique.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::Technique backendTechnique;
        TestRenderer renderer;
        backendTechnique.setRenderer(&renderer);

        {
            // WHEN
            const bool newValue = false;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("enabled");
            change->setValue(newValue);
            backendTechnique.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendTechnique.isEnabled(), newValue);
        }
        {
            // WHEN
            Qt3DRender::GraphicsApiFilterData newValue;
            newValue.m_major = 4;
            newValue.m_minor = 5;
            newValue.m_vendor = QStringLiteral("ATI");

            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("graphicsApiFilterData");
            change->setValue(QVariant::fromValue(newValue));
            backendTechnique.sceneChangeEvent(change);

            // THEN
            QCOMPARE(*backendTechnique.graphicsApiFilter(), newValue);
        }
        {
            Qt3DRender::QParameter parameter;

            {
                // WHEN
                const auto change = Qt3DCore::QPropertyNodeAddedChangePtr::create(Qt3DCore::QNodeId(), &parameter);
                change->setPropertyName("parameter");
                backendTechnique.sceneChangeEvent(change);

                // THEN
                QCOMPARE(backendTechnique.parameters().size(), 1);
                QCOMPARE(backendTechnique.parameters().first(), parameter.id());
            }
            {
                // WHEN
                const auto change = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &parameter);
                change->setPropertyName("parameter");
                backendTechnique.sceneChangeEvent(change);

                // THEN
                QCOMPARE(backendTechnique.parameters().size(), 0);
            }
        }
        {
            Qt3DRender::QFilterKey filterKey;

            {
                // WHEN
                const auto change = Qt3DCore::QPropertyNodeAddedChangePtr::create(Qt3DCore::QNodeId(), &filterKey);
                change->setPropertyName("filterKeys");
                backendTechnique.sceneChangeEvent(change);

                // THEN
                QCOMPARE(backendTechnique.filterKeys().size(), 1);
                QCOMPARE(backendTechnique.filterKeys().first(), filterKey.id());
            }
            {
                // WHEN
                const auto change = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &filterKey);
                change->setPropertyName("filterKeys");
                backendTechnique.sceneChangeEvent(change);

                // THEN
                QCOMPARE(backendTechnique.filterKeys().size(), 0);
            }
        }
        {
            Qt3DRender::QRenderPass pass;

            {
                // WHEN
                const auto change = Qt3DCore::QPropertyNodeAddedChangePtr::create(Qt3DCore::QNodeId(), &pass);
                change->setPropertyName("pass");
                backendTechnique.sceneChangeEvent(change);

                // THEN
                QCOMPARE(backendTechnique.renderPasses().size(), 1);
                QCOMPARE(backendTechnique.renderPasses().first(), pass.id());
            }
            {
                // WHEN
                const auto change = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &pass);
                change->setPropertyName("pass");
                backendTechnique.sceneChangeEvent(change);

                // THEN
                QCOMPARE(backendTechnique.renderPasses().size(), 0);
            }
        }
    }

};

QTEST_MAIN(tst_Technique)

#include "tst_technique.moc"
