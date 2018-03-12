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
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/filterkey_p.h>
#include <Qt3DRender/private/techniquemanager_p.h>
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
        QCOMPARE(backendTechnique.isCompatibleWithRenderer(), false);
        QVERIFY(backendTechnique.nodeManager() == nullptr);
    }

    void checkCleanupState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Technique backendTechnique;
        Qt3DRender::Render::NodeManagers nodeManagers;

        // WHEN
        backendTechnique.setRenderer(&renderer);
        backendTechnique.setEnabled(true);
        backendTechnique.setNodeManager(&nodeManagers);
        backendTechnique.setCompatibleWithRenderer(true);

        {
            Qt3DRender::QTechnique technique;
            Qt3DRender::QRenderPass pass;
            Qt3DRender::QParameter parameter;
            Qt3DRender::QFilterKey filterKey;

            technique.addRenderPass(&pass);
            technique.addParameter(&parameter);
            technique.addFilterKey(&filterKey);

            simulateInitializationSync(&technique, &backendTechnique);
        }

        backendTechnique.cleanup();

        // THEN
        QCOMPARE(backendTechnique.isEnabled(), false);
        QCOMPARE(backendTechnique.parameters().size(), 0);
        QCOMPARE(backendTechnique.filterKeys().size(), 0);
        QCOMPARE(backendTechnique.renderPasses().size(), 0);
        QCOMPARE(backendTechnique.isCompatibleWithRenderer(), false);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
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
            Qt3DRender::Render::NodeManagers nodeManagers;

            backendTechnique.setRenderer(&renderer);
            backendTechnique.setNodeManager(&nodeManagers);
            simulateInitializationSync(&technique, &backendTechnique);

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
            QCOMPARE(backendTechnique.isCompatibleWithRenderer(), false);
            const QVector<Qt3DCore::QNodeId> dirtyTechniques = nodeManagers.techniqueManager()->takeDirtyTechniques();
            QCOMPARE(dirtyTechniques.size(), 1);
            QCOMPARE(dirtyTechniques.first(), backendTechnique.peerId());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::Technique backendTechnique;
            Qt3DRender::Render::NodeManagers nodeManagers;

            backendTechnique.setNodeManager(&nodeManagers);
            backendTechnique.setRenderer(&renderer);
            technique.setEnabled(false);
            simulateInitializationSync(&technique, &backendTechnique);

            // THEN
            QCOMPARE(backendTechnique.peerId(), technique.id());
            QCOMPARE(backendTechnique.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
        }
    }

    void checkSetCompatibleWithRenderer()
    {
        // GIVEN
        Qt3DRender::Render::Technique backendTechnique;

        // THEN
        QCOMPARE(backendTechnique.isCompatibleWithRenderer(), false);

        // WHEN
        backendTechnique.setCompatibleWithRenderer(true);

        // THEN
        QCOMPARE(backendTechnique.isCompatibleWithRenderer(), true);
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::QTechnique technique;
        Qt3DRender::Render::Technique backendTechnique;
        Qt3DRender::Render::NodeManagers nodeManagers;

        TestRenderer renderer;
        backendTechnique.setRenderer(&renderer);
        backendTechnique.setNodeManager(&nodeManagers);

        {
            // WHEN
            const bool newValue = true;
            technique.setEnabled(newValue);
            backendTechnique.syncFromFrontEnd(&technique, false);

            // THEN
            QCOMPARE(backendTechnique.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            QCOMPARE(nodeManagers.techniqueManager()->takeDirtyTechniques().size(), 1);
        }
        {
            // WHEN
            backendTechnique.setCompatibleWithRenderer(true);
            QCOMPARE(nodeManagers.techniqueManager()->takeDirtyTechniques().size(), 0);

            technique.graphicsApiFilter()->setMajorVersion(4);
            technique.graphicsApiFilter()->setMinorVersion(5);
            technique.graphicsApiFilter()->setVendor(QStringLiteral("ATI"));

            backendTechnique.syncFromFrontEnd(&technique, false);

            // THEN
            QCOMPARE(backendTechnique.graphicsApiFilter()->m_major, technique.graphicsApiFilter()->majorVersion());
            QCOMPARE(backendTechnique.graphicsApiFilter()->m_minor, technique.graphicsApiFilter()->minorVersion());
            QCOMPARE(backendTechnique.graphicsApiFilter()->m_vendor, technique.graphicsApiFilter()->vendor());
            QCOMPARE(backendTechnique.isCompatibleWithRenderer(), false);

            const QVector<Qt3DCore::QNodeId> dirtyTechniques = nodeManagers.techniqueManager()->takeDirtyTechniques();
            QCOMPARE(dirtyTechniques.size(), 1);
            QCOMPARE(dirtyTechniques.first(), backendTechnique.peerId());

            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            Qt3DRender::QParameter parameter;

            {
                // WHEN
                technique.addParameter(&parameter);
                backendTechnique.syncFromFrontEnd(&technique, false);

                // THEN
                QCOMPARE(backendTechnique.parameters().size(), 1);
                QCOMPARE(backendTechnique.parameters().first(), parameter.id());
                QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
                renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            }
            {
                // WHEN
                technique.removeParameter(&parameter);
                backendTechnique.syncFromFrontEnd(&technique, false);

                // THEN
                QCOMPARE(backendTechnique.parameters().size(), 0);
                QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
                renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            }
        }
        {
            Qt3DRender::QFilterKey filterKey;

            {
                // WHEN
                technique.addFilterKey(&filterKey);
                backendTechnique.syncFromFrontEnd(&technique, false);

                // THEN
                QCOMPARE(backendTechnique.filterKeys().size(), 1);
                QCOMPARE(backendTechnique.filterKeys().first(), filterKey.id());
                QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
                renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            }
            {
                // WHEN
                technique.removeFilterKey(&filterKey);
                backendTechnique.syncFromFrontEnd(&technique, false);

                // THEN
                QCOMPARE(backendTechnique.filterKeys().size(), 0);
                QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
                renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            }
        }
        {
            Qt3DRender::QRenderPass pass;

            {
                // WHEN
                technique.addRenderPass(&pass);
                backendTechnique.syncFromFrontEnd(&technique, false);

                // THEN
                QCOMPARE(backendTechnique.renderPasses().size(), 1);
                QCOMPARE(backendTechnique.renderPasses().first(), pass.id());
                QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
                renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            }
            {
                // WHEN
                technique.removeRenderPass(&pass);
                backendTechnique.syncFromFrontEnd(&technique, false);

                // THEN
                QCOMPARE(backendTechnique.renderPasses().size(), 0);
                QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TechniquesDirty);
                renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            }
        }
    }

    void checkIsCompatibleWithFilters()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Technique backendTechnique;
        Qt3DRender::Render::NodeManagers nodeManagers;

        backendTechnique.setNodeManager(&nodeManagers);

        Qt3DRender::QFilterKey *filterKey1 = new Qt3DRender::QFilterKey();
        Qt3DRender::QFilterKey *filterKey2 = new Qt3DRender::QFilterKey();
        Qt3DRender::QFilterKey *filterKey3 = new Qt3DRender::QFilterKey();
        Qt3DRender::QFilterKey *filterKey4 = new Qt3DRender::QFilterKey();
        Qt3DRender::QFilterKey *filterKey5 = new Qt3DRender::QFilterKey();

        filterKey1->setName(QStringLiteral("displacement"));
        filterKey2->setName(QStringLiteral("diffRatio"));
        filterKey3->setName(QStringLiteral("oil"));
        filterKey4->setName(QStringLiteral("oil"));
        filterKey5->setName(QStringLiteral("heads"));

        filterKey1->setValue(QVariant(427.0f));
        filterKey2->setValue(QVariant(4.11f));
        filterKey3->setValue(QVariant(QStringLiteral("Valvoline-VR1")));
        filterKey4->setValue(QVariant(QStringLiteral("Mobil-1")));
        filterKey5->setName(QStringLiteral("AFR"));

        // Create backend nodes
        // WHEN
        Qt3DRender::Render::FilterKey *backendFilterKey1 = nodeManagers.filterKeyManager()->getOrCreateResource(filterKey1->id());
        Qt3DRender::Render::FilterKey *backendFilterKey2 = nodeManagers.filterKeyManager()->getOrCreateResource(filterKey2->id());
        Qt3DRender::Render::FilterKey *backendFilterKey3 = nodeManagers.filterKeyManager()->getOrCreateResource(filterKey3->id());
        Qt3DRender::Render::FilterKey *backendFilterKey4 = nodeManagers.filterKeyManager()->getOrCreateResource(filterKey4->id());
        Qt3DRender::Render::FilterKey *backendFilterKey5 = nodeManagers.filterKeyManager()->getOrCreateResource(filterKey5->id());

        backendFilterKey1->setRenderer(&renderer);
        backendFilterKey2->setRenderer(&renderer);
        backendFilterKey3->setRenderer(&renderer);
        backendFilterKey4->setRenderer(&renderer);
        backendFilterKey5->setRenderer(&renderer);

        simulateInitializationSync(filterKey1, backendFilterKey1);
        simulateInitializationSync(filterKey2, backendFilterKey2);
        simulateInitializationSync(filterKey3, backendFilterKey3);
        simulateInitializationSync(filterKey4, backendFilterKey4);
        simulateInitializationSync(filterKey5, backendFilterKey5);

        // THEN
        QCOMPARE(nodeManagers.filterKeyManager()->activeHandles().size(), 5);

        {
            // WHEN
            backendTechnique.appendFilterKey(filterKey1->id());
            backendTechnique.appendFilterKey(filterKey2->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 2);

            // WHEN
            Qt3DCore::QNodeIdVector techniqueFilters;
            techniqueFilters.push_back(filterKey1->id());
            techniqueFilters.push_back(filterKey2->id());
            techniqueFilters.push_back(filterKey3->id());
            techniqueFilters.push_back(filterKey5->id());

            // THEN -> incompatible technique doesn't have enough filters
            QCOMPARE(backendTechnique.isCompatibleWithFilters(techniqueFilters), false);

            // WHEN
            backendTechnique.removeFilterKey(filterKey1->id());
            backendTechnique.removeFilterKey(filterKey2->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 0);
        }

        {
            // WHEN
            backendTechnique.appendFilterKey(filterKey1->id());
            backendTechnique.appendFilterKey(filterKey2->id());
            backendTechnique.appendFilterKey(filterKey3->id());
            backendTechnique.appendFilterKey(filterKey5->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 4);

            // WHEN
            Qt3DCore::QNodeIdVector techniqueFilters;
            techniqueFilters.push_back(filterKey1->id());
            techniqueFilters.push_back(filterKey2->id());
            techniqueFilters.push_back(filterKey3->id());
            techniqueFilters.push_back(filterKey5->id());

            // THEN -> compatible same number
            QCOMPARE(backendTechnique.isCompatibleWithFilters(techniqueFilters), true);

            // WHEN
            backendTechnique.removeFilterKey(filterKey1->id());
            backendTechnique.removeFilterKey(filterKey2->id());
            backendTechnique.removeFilterKey(filterKey3->id());
            backendTechnique.removeFilterKey(filterKey5->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 0);
        }

        {
            // WHEN
            backendTechnique.appendFilterKey(filterKey1->id());
            backendTechnique.appendFilterKey(filterKey2->id());
            backendTechnique.appendFilterKey(filterKey3->id());
            backendTechnique.appendFilterKey(filterKey5->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 4);

            // WHEN
            Qt3DCore::QNodeIdVector techniqueFilters;
            techniqueFilters.push_back(filterKey1->id());
            techniqueFilters.push_back(filterKey2->id());
            techniqueFilters.push_back(filterKey4->id());
            techniqueFilters.push_back(filterKey5->id());

            // THEN -> compatible same number, one not matching
            QCOMPARE(backendTechnique.isCompatibleWithFilters(techniqueFilters), false);

            // WHEN
            backendTechnique.removeFilterKey(filterKey1->id());
            backendTechnique.removeFilterKey(filterKey2->id());
            backendTechnique.removeFilterKey(filterKey3->id());
            backendTechnique.removeFilterKey(filterKey5->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 0);
        }

        {
            // WHEN
            backendTechnique.appendFilterKey(filterKey1->id());
            backendTechnique.appendFilterKey(filterKey2->id());
            backendTechnique.appendFilterKey(filterKey3->id());
            backendTechnique.appendFilterKey(filterKey5->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 4);

            // WHEN
            Qt3DCore::QNodeIdVector techniqueFilters;
            techniqueFilters.push_back(filterKey1->id());
            techniqueFilters.push_back(filterKey2->id());
            techniqueFilters.push_back(filterKey3->id());

            // THEN -> technique has more than necessary filters
            QCOMPARE(backendTechnique.isCompatibleWithFilters(techniqueFilters), true);

            // WHEN
            backendTechnique.removeFilterKey(filterKey1->id());
            backendTechnique.removeFilterKey(filterKey2->id());
            backendTechnique.removeFilterKey(filterKey3->id());
            backendTechnique.removeFilterKey(filterKey5->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 0);
        }

        {
            // WHEN
            backendTechnique.appendFilterKey(filterKey1->id());
            backendTechnique.appendFilterKey(filterKey2->id());
            backendTechnique.appendFilterKey(filterKey3->id());
            backendTechnique.appendFilterKey(filterKey5->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 4);

            // WHEN
            Qt3DCore::QNodeIdVector techniqueFilters;
            techniqueFilters.push_back(filterKey1->id());
            techniqueFilters.push_back(filterKey2->id());
            techniqueFilters.push_back(filterKey4->id());

            // THEN -> technique has more than necessary filters
            // but one is not matching
            QCOMPARE(backendTechnique.isCompatibleWithFilters(techniqueFilters), false);

            // WHEN
            backendTechnique.removeFilterKey(filterKey1->id());
            backendTechnique.removeFilterKey(filterKey2->id());
            backendTechnique.removeFilterKey(filterKey3->id());
            backendTechnique.removeFilterKey(filterKey5->id());

            // THEN
            QCOMPARE(backendTechnique.filterKeys().size(), 0);
        }
    }
};

QTEST_MAIN(tst_Technique)

#include "tst_technique.moc"
