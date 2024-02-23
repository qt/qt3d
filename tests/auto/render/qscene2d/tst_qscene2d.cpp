// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DQuickScene2D/qscene2d.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <private/qscene2d_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

using namespace Qt3DRender::Quick;

class tst_QScene2D : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::Quick::QScene2D::RenderPolicy>(
                    "QScene2D::RenderPolicy");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::Quick::QScene2D scene2d;

        // THEN
        QCOMPARE(scene2d.output(), nullptr);
        QCOMPARE(scene2d.renderPolicy(), QScene2D::Continuous);
        QCOMPARE(scene2d.item(), nullptr);
        QCOMPARE(scene2d.isMouseEnabled(), true);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::Quick::QScene2D scene2d;
        QScopedPointer<Qt3DRender::QRenderTargetOutput> output(new Qt3DRender::QRenderTargetOutput());
        QScopedPointer<QQuickItem> item(new QQuickItem());

        {
            // WHEN
            QSignalSpy spy(&scene2d, SIGNAL(outputChanged(Qt3DRender::QRenderTargetOutput*)));
            Qt3DRender::QRenderTargetOutput *newValue = output.data();
            scene2d.setOutput(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(scene2d.output(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            scene2d.setOutput(newValue);

            // THEN
            QCOMPARE(scene2d.output(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&scene2d, SIGNAL(renderPolicyChanged(QScene2D::RenderPolicy)));
            const QScene2D::RenderPolicy newValue = QScene2D::SingleShot;
            scene2d.setRenderPolicy(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(scene2d.renderPolicy(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            scene2d.setRenderPolicy(newValue);

            // THEN
            QCOMPARE(scene2d.renderPolicy(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&scene2d, SIGNAL(itemChanged(QQuickItem*)));
            QQuickItem *newValue = item.data();
            scene2d.setItem(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(scene2d.item(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            scene2d.setItem(newValue);

            // THEN
            QCOMPARE(scene2d.item(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&scene2d, SIGNAL(mouseEnabledChanged(bool)));
            bool newValue = false;
            scene2d.setMouseEnabled(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(scene2d.isMouseEnabled(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            scene2d.setMouseEnabled(newValue);

            // THEN
            QCOMPARE(scene2d.isMouseEnabled(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkOutputUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::Quick::QScene2D scene2d;
        arbiter.setArbiterOnNode(&scene2d);
        QScopedPointer<Qt3DRender::QRenderTargetOutput> output(new Qt3DRender::QRenderTargetOutput());

        {
            // WHEN
            scene2d.setOutput(output.data());
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &scene2d);

            arbiter.clear();
        }

        {
            // WHEN
            scene2d.setOutput(output.data());
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkRenderPolicyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::Quick::QScene2D scene2d;
        arbiter.setArbiterOnNode(&scene2d);

        {
            // WHEN
            scene2d.setRenderPolicy(QScene2D::SingleShot);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &scene2d);

            arbiter.clear();
        }

        {
            // WHEN
            scene2d.setRenderPolicy(QScene2D::SingleShot);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkMouseEnabledUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::Quick::QScene2D scene2d;
        arbiter.setArbiterOnNode(&scene2d);

        {
            // WHEN
            scene2d.setMouseEnabled(false);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &scene2d);

            arbiter.clear();
        }

        {
            // WHEN
            scene2d.setMouseEnabled(false);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QScene2D)

#include "tst_qscene2d.moc"
