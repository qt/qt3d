// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/private/qparameter_p.h>
#include <Qt3DRender/private/parameter_p.h>
#include <Qt3DRender/private/uniform_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_Parameter : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::Parameter backendParameter;

        // THEN
        QCOMPARE(backendParameter.isEnabled(), false);
        QVERIFY(backendParameter.peerId().isNull());
        QCOMPARE(backendParameter.name(), QString());
        QCOMPARE(backendParameter.uniformValue(), Qt3DRender::Render::UniformValue());
        QCOMPARE(backendParameter.nameId(), -1);
        QCOMPARE(backendParameter.backendValue(), QVariant());
    }

    void checkCleanupState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Parameter backendParameter;
        Qt3DRender::QParameter parameter;
        parameter.setName(QStringLiteral("Cutlass"));
        parameter.setValue(QVariant(QColor(Qt::blue)));

        // WHEN
        backendParameter.setRenderer(&renderer);
        simulateInitializationSync(&parameter, &backendParameter);
        backendParameter.cleanup();

        // THEN
        QCOMPARE(backendParameter.isEnabled(), false);
        QCOMPARE(backendParameter.name(), QString());
        QCOMPARE(backendParameter.uniformValue(), Qt3DRender::Render::UniformValue());
        QCOMPARE(backendParameter.nameId(), -1);
        QCOMPARE(backendParameter.backendValue(), QVariant());
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QParameter parameter;

        parameter.setName(QStringLiteral("Chevelle"));
        parameter.setValue(QVariant(383.0f));

        {
            // WHEN
            Qt3DRender::Render::Parameter backendParameter;
            backendParameter.setRenderer(&renderer);
            simulateInitializationSync(&parameter, &backendParameter);

            // THEN
            QCOMPARE(backendParameter.isEnabled(), true);
            QCOMPARE(backendParameter.peerId(), parameter.id());
            QCOMPARE(backendParameter.name(), QStringLiteral("Chevelle"));
            QCOMPARE(backendParameter.uniformValue(), Qt3DRender::Render::UniformValue::fromVariant(parameter.value()));
            QCOMPARE(backendParameter.nameId(), Qt3DRender::Render::StringToInt::lookupId(QStringLiteral("Chevelle")));
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ParameterDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::Parameter backendParameter;
            backendParameter.setRenderer(&renderer);
            parameter.setEnabled(false);
            simulateInitializationSync(&parameter, &backendParameter);

            // THEN
            QCOMPARE(backendParameter.peerId(), parameter.id());
            QCOMPARE(backendParameter.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ParameterDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::Parameter backendParameter;
        TestRenderer renderer;
        backendParameter.setRenderer(&renderer);

        Qt3DRender::QParameter parameter;
        simulateInitializationSync(&parameter, &backendParameter);

        {
            // WHEN
            const bool newValue = false;
            parameter.setEnabled(newValue);
            backendParameter.syncFromFrontEnd(&parameter, false);

            // THEN
            QCOMPARE(backendParameter.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ParameterDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const QString newValue = QStringLiteral("C7");
            parameter.setName(newValue);
            backendParameter.syncFromFrontEnd(&parameter, false);

            // THEN
            QCOMPARE(backendParameter.name(), newValue);
            QCOMPARE(backendParameter.nameId(), Qt3DRender::Render::StringToInt::lookupId(newValue));
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ParameterDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const QVariant value = QVariant::fromValue(QVector3D(350.0f, 427.0f, 454.0f));
            const Qt3DRender::Render::UniformValue newValue = Qt3DRender::Render::UniformValue::fromVariant(value);
            parameter.setValue(value);
            backendParameter.syncFromFrontEnd(&parameter, false);

            // THEN
            QCOMPARE(backendParameter.uniformValue(), newValue);
            QCOMPARE(backendParameter.backendValue(), value);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ParameterDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }

};

QTEST_MAIN(tst_Parameter)

#include "tst_parameter.moc"
