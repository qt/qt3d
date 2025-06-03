// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/private/qrenderpass_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QRenderPass : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QShaderProgram *>("QShaderProgram *");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QRenderPass renderPass;

        // THEN
        QVERIFY(renderPass.shaderProgram() == nullptr);
        QCOMPARE(renderPass.filterKeys().size(), 0);
        QCOMPARE(renderPass.renderStates().size(), 0);
        QCOMPARE(renderPass.parameters().size(), 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QRenderPass renderPass;

        {
            // WHEN
            QSignalSpy spy(&renderPass, SIGNAL(shaderProgramChanged(QShaderProgram*)));
            Qt3DRender::QShaderProgram newValue;
            renderPass.setShaderProgram(&newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderPass.shaderProgram(), &newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderPass.setShaderProgram(&newValue);

            // THEN
            QCOMPARE(renderPass.shaderProgram(), &newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            Qt3DRender::QFilterKey newValue;
            renderPass.addFilterKey(&newValue);

            // THEN
            QCOMPARE(renderPass.filterKeys().size(), 1);

            // WHEN
            renderPass.addFilterKey(&newValue);

            // THEN
            QCOMPARE(renderPass.filterKeys().size(), 1);

            // WHEN
            renderPass.removeFilterKey(&newValue);

            // THEN
            QCOMPARE(renderPass.filterKeys().size(), 0);
        }
        {
            // WHEN
            Qt3DRender::QDepthTest newValue;
            renderPass.addRenderState(&newValue);

            // THEN
            QCOMPARE(renderPass.renderStates().size(), 1);

            // WHEN
            renderPass.addRenderState(&newValue);

            // THEN
            QCOMPARE(renderPass.renderStates().size(), 1);

            // WHEN
            renderPass.removeRenderState(&newValue);

            // THEN
            QCOMPARE(renderPass.renderStates().size(), 0);
        }
        {
            // WHEN
            Qt3DRender::QParameter newValue;
            renderPass.addParameter(&newValue);

            // THEN
            QCOMPARE(renderPass.parameters().size(), 1);

            // WHEN
            renderPass.addParameter(&newValue);

            // THEN
            QCOMPARE(renderPass.parameters().size(), 1);

            // WHEN
            renderPass.removeParameter(&newValue);

            // THEN
            QCOMPARE(renderPass.parameters().size(), 0);
        }
    }

    void checkShaderProgramBookkeeping()
    {
        // GIVEN
        Qt3DRender::QRenderPass renderPass;

        {
            // WHEN
            Qt3DRender::QShaderProgram shaderProgram;
            renderPass.setShaderProgram(&shaderProgram);

            QCOMPARE(renderPass.shaderProgram(), &shaderProgram);
        }

        // THEN -> should not crash
        QVERIFY(renderPass.shaderProgram() == nullptr);
    }

    void checkFilterKeyBookkeeping()
    {
        // GIVEN
        Qt3DRender::QRenderPass renderPass;

        {
            // WHEN
            Qt3DRender::QFilterKey filterKey;
            renderPass.addFilterKey(&filterKey);

            QCOMPARE(renderPass.filterKeys().size(), 1);
        }

        // THEN -> should not crash
        QCOMPARE(renderPass.filterKeys().size(), 0);
    }

    void checkRenderStateBookkeeping()
    {
        // GIVEN
        Qt3DRender::QRenderPass renderPass;

        {
            // WHEN
            Qt3DRender::QDepthTest renderState;
            renderPass.addRenderState(&renderState);

            QCOMPARE(renderPass.renderStates().size(), 1);
        }

        // THEN -> should not crash
        QCOMPARE(renderPass.renderStates().size(), 0);
    }

    void checkParameterBookkeeping()
    {
        // GIVEN
        Qt3DRender::QRenderPass renderPass;

        {
            // WHEN
            Qt3DRender::QParameter parameter;
            renderPass.addParameter(&parameter);

            QCOMPARE(renderPass.parameters().size(), 1);
        }

        // THEN -> should not crash
        QCOMPARE(renderPass.parameters().size(), 0);
    }

    void checkShaderProgramUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderPass renderPass;
        Qt3DRender::QShaderProgram shaderProgram;
        arbiter.setArbiterOnNode(&renderPass);

        {
            // WHEN
            renderPass.setShaderProgram(&shaderProgram);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderPass);

            arbiter.clear();
        }

        {
            // WHEN
            renderPass.setShaderProgram(&shaderProgram);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkFilterKeyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderPass renderPass;
        Qt3DRender::QFilterKey filterKey;
        arbiter.setArbiterOnNode(&renderPass);

        {
            // WHEN
            renderPass.addFilterKey(&filterKey);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderPass);

            arbiter.clear();
        }

        {
            // WHEN
            renderPass.removeFilterKey(&filterKey);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderPass);

            arbiter.clear();
        }

    }

    void checkRenderStateUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderPass renderPass;
        Qt3DRender::QDepthTest renderState;
        arbiter.setArbiterOnNode(&renderPass);

        {
            // WHEN
            renderPass.addRenderState(&renderState);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderPass);

            arbiter.clear();
        }

        {
            // WHEN
            renderPass.removeRenderState(&renderState);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderPass);

            arbiter.clear();
        }

    }

    void checkParameterUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderPass renderPass;
        Qt3DRender::QParameter parameter;
        arbiter.setArbiterOnNode(&renderPass);

        {
            // WHEN
            renderPass.addParameter(&parameter);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderPass);

            arbiter.clear();
        }

        {
            // WHEN
            renderPass.removeParameter(&parameter);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderPass);

            arbiter.clear();
        }

    }

};

QTEST_MAIN(tst_QRenderPass)

#include "tst_qrenderpass.moc"
