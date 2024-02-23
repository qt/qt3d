// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/private/qeffect_p.h>
#include <Qt3DRender/private/effect_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"


class tst_Effect : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::Effect backendEffect;

        // THEN
        QCOMPARE(backendEffect.isEnabled(), false);
        QVERIFY(backendEffect.peerId().isNull());
        QVERIFY(backendEffect.techniques().empty());
    }

    void checkCleanupState()
    {
        // GIVEN
        Qt3DRender::Render::Effect backendEffect;

        // WHEN
        backendEffect.setEnabled(true);

        {
            Qt3DRender::QEffect effect;
            Qt3DRender::QTechnique technique;
            Qt3DRender::QParameter parameter;
            effect.addTechnique(&technique);
            effect.addParameter(&parameter);
            simulateInitializationSync(&effect, &backendEffect);
        }

        backendEffect.cleanup();

        // THEN
        QCOMPARE(backendEffect.isEnabled(), false);
        QCOMPARE(backendEffect.techniques().size(), 0);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DRender::QEffect effect;
        Qt3DRender::QTechnique technique;
        Qt3DRender::QParameter parameter;

        effect.addTechnique(&technique);
        effect.addParameter(&parameter);

        {
            // WHEN
            Qt3DRender::Render::Effect backendEffect;
            simulateInitializationSync(&effect, &backendEffect);

            // THEN
            QCOMPARE(backendEffect.isEnabled(), true);
            QCOMPARE(backendEffect.peerId(), effect.id());
            QCOMPARE(backendEffect.techniques().size(), 1);
            QCOMPARE(backendEffect.techniques().first(), technique.id());
            QCOMPARE(backendEffect.parameters().size(), 1);
            QCOMPARE(backendEffect.parameters().first(), parameter.id());
        }
        {
            // WHEN
            Qt3DRender::Render::Effect backendEffect;
            effect.setEnabled(false);
            simulateInitializationSync(&effect, &backendEffect);

            // THEN
            QCOMPARE(backendEffect.peerId(), effect.id());
            QCOMPARE(backendEffect.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::QEffect effect;
        Qt3DRender::Render::Effect backendEffect;
        TestRenderer renderer;
        backendEffect.setRenderer(&renderer);
        simulateInitializationSync(&effect, &backendEffect);

        {
            // WHEN
            const bool newValue = false;
            effect.setEnabled(newValue);
            backendEffect.syncFromFrontEnd(&effect, false);

            // THEN
            QCOMPARE(backendEffect.isEnabled(), newValue);
        }
        {
            Qt3DRender::QTechnique technique;
            {
                // WHEN
                effect.addTechnique(&technique);
                backendEffect.syncFromFrontEnd(&effect, false);

                // THEN
                QCOMPARE(backendEffect.techniques().size(), 1);
                QCOMPARE(backendEffect.techniques().first(), technique.id());
            }
            {
                // WHEN
                effect.removeTechnique(&technique);
                backendEffect.syncFromFrontEnd(&effect, false);

                // THEN
                QCOMPARE(backendEffect.techniques().size(), 0);
            }
        }
        {
            Qt3DRender::QParameter parameter;

            {
                // WHEN
                effect.addParameter(&parameter);
                backendEffect.syncFromFrontEnd(&effect, false);

                // THEN
                QCOMPARE(backendEffect.parameters().size(), 1);
                QCOMPARE(backendEffect.parameters().first(), parameter.id());
            }
            {
                // WHEN
                effect.removeParameter(&parameter);
                backendEffect.syncFromFrontEnd(&effect, false);

                // THEN
                QCOMPARE(backendEffect.parameters().size(), 0);
            }
        }
    }

};

QTEST_MAIN(tst_Effect)

#include "tst_effect.moc"
