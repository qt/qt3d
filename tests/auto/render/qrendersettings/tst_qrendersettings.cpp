// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qrendersettings.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/private/qrendersettings_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QRenderSettings : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QRenderSettings::RenderPolicy >("RenderPolicy");
        qRegisterMetaType<Qt3DRender::QPickingSettings::PickMethod>("QPickingSettings::PickMethod");
        qRegisterMetaType<Qt3DRender::QPickingSettings::PickResultMode>("QPickingSettings::PickResultMode");
        qRegisterMetaType<Qt3DRender::QPickingSettings::FaceOrientationPickingMode>("QPickingSettings::FaceOrientationPickingMode");
        qRegisterMetaType<Qt3DRender::QFrameGraphNode*>("QFrameGraphNode *");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QRenderSettings renderSettings;

        // THEN
        QVERIFY(renderSettings.pickingSettings() != nullptr);
        QCOMPARE(renderSettings.renderPolicy(),  Qt3DRender::QRenderSettings::Always);
        QVERIFY(renderSettings.activeFrameGraph() == nullptr);
        QCOMPARE(renderSettings.pickingSettings()->pickMethod(), Qt3DRender::QPickingSettings::BoundingVolumePicking);
        QCOMPARE(renderSettings.pickingSettings()->pickResultMode(), Qt3DRender::QPickingSettings::NearestPick);
        QCOMPARE(renderSettings.pickingSettings()->faceOrientationPickingMode(), Qt3DRender::QPickingSettings::FrontFace);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QRenderSettings renderSettings;
        Qt3DRender::QPickingSettings *pickingSettings = renderSettings.pickingSettings();

        {
            // WHEN
            QSignalSpy spy(&renderSettings, SIGNAL(renderPolicyChanged(RenderPolicy)));
            const Qt3DRender::QRenderSettings::RenderPolicy newValue = Qt3DRender::QRenderSettings::OnDemand;
            renderSettings.setRenderPolicy(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderSettings.renderPolicy(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderSettings.setRenderPolicy(newValue);

            // THEN
            QCOMPARE(renderSettings.renderPolicy(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&renderSettings, SIGNAL(activeFrameGraphChanged(QFrameGraphNode*)));

            Qt3DRender::QViewport newValue;
            renderSettings.setActiveFrameGraph(&newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderSettings.activeFrameGraph(), &newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderSettings.setActiveFrameGraph(&newValue);

            // THEN
            QCOMPARE(renderSettings.activeFrameGraph(), &newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(pickingSettings, SIGNAL(pickMethodChanged(QPickingSettings::PickMethod)));
            const Qt3DRender::QPickingSettings::PickMethod newValue = Qt3DRender::QPickingSettings::TrianglePicking;
            pickingSettings->setPickMethod(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(pickingSettings->pickMethod(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            pickingSettings->setPickMethod(newValue);

            // THEN
            QCOMPARE(pickingSettings->pickMethod(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(pickingSettings, SIGNAL(pickResultModeChanged(QPickingSettings::PickResultMode)));
            const Qt3DRender::QPickingSettings::PickResultMode newValue = Qt3DRender::QPickingSettings::AllPicks;
            pickingSettings->setPickResultMode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(pickingSettings->pickResultMode(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            pickingSettings->setPickResultMode(newValue);

            // THEN
            QCOMPARE(pickingSettings->pickResultMode(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(pickingSettings, SIGNAL(faceOrientationPickingModeChanged(QPickingSettings::FaceOrientationPickingMode)));
            const Qt3DRender::QPickingSettings::FaceOrientationPickingMode newValue = Qt3DRender::QPickingSettings::FrontAndBackFace;
            pickingSettings->setFaceOrientationPickingMode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(pickingSettings->faceOrientationPickingMode(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            pickingSettings->setFaceOrientationPickingMode(newValue);

            // THEN
            QCOMPARE(pickingSettings->faceOrientationPickingMode(), newValue);
            QCOMPARE(spy.size(), 0);
        }

    }

    void checkRenderPolicyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderSettings renderSettings;
        arbiter.setArbiterOnNode(&renderSettings);

        {
            // WHEN
            renderSettings.setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSettings);

            arbiter.clear();
        }

        {
            // WHEN
            renderSettings.setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkActiveFrameGraphUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderSettings renderSettings;
        arbiter.setArbiterOnNode(&renderSettings);
        Qt3DRender::QViewport viewport;

        {
            // WHEN
            renderSettings.setActiveFrameGraph(&viewport);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSettings);

            arbiter.clear();
        }

        {
            // WHEN
            renderSettings.setActiveFrameGraph(&viewport);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkPickMethodUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderSettings renderSettings;
        Qt3DRender::QPickingSettings *pickingSettings = renderSettings.pickingSettings();

        arbiter.setArbiterOnNode(&renderSettings);

        {
            // WHEN
            pickingSettings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSettings);

            arbiter.clear();
        }

        {
            // WHEN
            pickingSettings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkPickResultModeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderSettings renderSettings;
        Qt3DRender::QPickingSettings *pickingSettings = renderSettings.pickingSettings();

        arbiter.setArbiterOnNode(&renderSettings);

        {
            // WHEN
            pickingSettings->setPickResultMode(Qt3DRender::QPickingSettings::AllPicks);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSettings);

            arbiter.clear();
        }

        {
            // WHEN
            pickingSettings->setPickResultMode(Qt3DRender::QPickingSettings::AllPicks);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkFaceOrientationPickingModeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderSettings renderSettings;
        Qt3DRender::QPickingSettings *pickingSettings = renderSettings.pickingSettings();

        arbiter.setArbiterOnNode(&renderSettings);

        {
            // WHEN
            pickingSettings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontAndBackFace);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSettings);

            arbiter.clear();
        }

        {
            // WHEN
            pickingSettings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontAndBackFace);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkWorldSpaceToleranceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderSettings renderSettings;
        Qt3DRender::QPickingSettings *pickingSettings = renderSettings.pickingSettings();

        arbiter.setArbiterOnNode(&renderSettings);

        {
            // WHEN
            pickingSettings->setWorldSpaceTolerance(5.f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSettings);

            arbiter.clear();
        }

        {
            // WHEN
            pickingSettings->setWorldSpaceTolerance(5.f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QRenderSettings)

#include "tst_qrendersettings.moc"
