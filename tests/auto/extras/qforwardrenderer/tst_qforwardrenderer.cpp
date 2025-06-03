// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DCore/qentity.h>
#include <QObject>
#include <QSignalSpy>

class tst_QForwardRenderer : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DCore::QEntity*>();
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DExtras::QForwardRenderer forwardRenderer;

        // THEN
        QVERIFY(forwardRenderer.surface() == nullptr);
        QCOMPARE(forwardRenderer.viewportRect(), QRectF(0.0f, 0.0f, 1.0f, 1.0f));
        QCOMPARE(forwardRenderer.clearColor(), QColor(Qt::white));
        QVERIFY(forwardRenderer.camera() == nullptr);
        QCOMPARE(forwardRenderer.externalRenderTargetSize(), QSize());
        QVERIFY(forwardRenderer.isFrustumCullingEnabled());
        QCOMPARE(forwardRenderer.gamma(), 2.2f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DExtras::QForwardRenderer forwardRenderer;

        {
            // WHEN
            QSignalSpy spy(&forwardRenderer, SIGNAL(surfaceChanged(QObject*)));
            QWindow newValue;
            forwardRenderer.setSurface(&newValue);

            // THEN
            QCOMPARE(forwardRenderer.surface(), &newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            forwardRenderer.setSurface(&newValue);

            // THEN
            QCOMPARE(forwardRenderer.surface(), &newValue);
            QCOMPARE(spy.count(), 0);

            // WHEN
            forwardRenderer.setSurface(nullptr);

            // THEN
            QVERIFY(forwardRenderer.surface() == nullptr);
            QCOMPARE(spy.count(), 1);
        }
        {
            // WHEN
            QSignalSpy spy(&forwardRenderer, SIGNAL(viewportRectChanged(QRectF)));
            const QRectF newValue = QRectF(0.5, 0.5, 0.5, 0.5);
            forwardRenderer.setViewportRect(newValue);

            // THEN
            QCOMPARE(forwardRenderer.viewportRect(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            forwardRenderer.setViewportRect(newValue);

            // THEN
            QCOMPARE(forwardRenderer.viewportRect(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&forwardRenderer, SIGNAL(clearColorChanged(QColor)));
            const QColor newValue = QColor(Qt::red);
            forwardRenderer.setClearColor(newValue);

            // THEN
            QCOMPARE(forwardRenderer.clearColor(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            forwardRenderer.setClearColor(newValue);

            // THEN
            QCOMPARE(forwardRenderer.clearColor(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&forwardRenderer, SIGNAL(cameraChanged(Qt3DCore::QEntity*)));
            Qt3DCore::QEntity newValue;
            forwardRenderer.setCamera(&newValue);

            // THEN
            QCOMPARE(forwardRenderer.camera(), &newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            forwardRenderer.setCamera(&newValue);

            // THEN
            QCOMPARE(forwardRenderer.camera(), &newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&forwardRenderer, SIGNAL(externalRenderTargetSizeChanged(QSize)));
            const QSize newValue = QSize(454, 427);
            forwardRenderer.setExternalRenderTargetSize(newValue);

            // THEN
            QCOMPARE(forwardRenderer.externalRenderTargetSize(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            forwardRenderer.setExternalRenderTargetSize(newValue);

            // THEN
            QCOMPARE(forwardRenderer.externalRenderTargetSize(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&forwardRenderer, SIGNAL(frustumCullingEnabledChanged(bool)));
            forwardRenderer.setFrustumCullingEnabled(false);

            // THEN
            QVERIFY(!forwardRenderer.isFrustumCullingEnabled());
            QCOMPARE(spy.count(), 1);
            QVERIFY(!spy.takeFirst().takeFirst().toBool());

            // WHEN
            forwardRenderer.setFrustumCullingEnabled(false);

            // THEN
            QVERIFY(!forwardRenderer.isFrustumCullingEnabled());
            QCOMPARE(spy.count(), 0);

            // WHEN
            forwardRenderer.setFrustumCullingEnabled(true);

            // THEN
            QVERIFY(forwardRenderer.isFrustumCullingEnabled());
            QCOMPARE(spy.count(), 1);
            QVERIFY(spy.takeFirst().takeFirst().toBool());
        }
        {
            // WHEN
            QSignalSpy spy(&forwardRenderer, SIGNAL(gammaChanged(float)));
            const float newValue = 1.8f;
            forwardRenderer.setGamma(newValue);

            // THEN
            QCOMPARE(forwardRenderer.gamma(), newValue);
            QCOMPARE(spy.count(), 1);
            QCOMPARE(spy.takeFirst().first().toFloat(), 1.8f);

            // WHEN
            spy.clear();
            forwardRenderer.setClearColor(newValue);

            // THEN
            QCOMPARE(forwardRenderer.gamma(), newValue);
            QCOMPARE(spy.count(), 0);

        }
    }

};

QTEST_MAIN(tst_QForwardRenderer)

#include "tst_qforwardrenderer.moc"
