// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/private/qrendercapture_p.h>

#include <QPointer>

#include "testarbiter.h"

class MyRenderCapture : public Qt3DRender::QRenderCapture
{
    Q_OBJECT
public:
    MyRenderCapture(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QRenderCapture(parent)
    {}

private:
    friend class tst_QRenderCapture;
};

class tst_QRenderCapture : public Qt3DCore::QNode
{
    Q_OBJECT

private Q_SLOTS:

    void checkCaptureRequest()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QRenderCapture> renderCapture(new Qt3DRender::QRenderCapture());
        arbiter.setArbiterOnNode(renderCapture.data());

        // WHEN
        QScopedPointer<Qt3DRender::QRenderCaptureReply> reply(renderCapture->requestCapture(QRect(10, 15, 20, 50)));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), renderCapture.data());

        arbiter.clear();
    }

    void crashOnRenderCaptureDeletion()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRenderCapture> renderCapture(new Qt3DRender::QRenderCapture());
        QPointer<Qt3DRender::QRenderCaptureReply> renderCaptureReply(renderCapture->requestCapture());

        // THEN
        QVERIFY(renderCaptureReply);

        // WHEN
        renderCapture.reset();

        // THEN (Should not crash and delete reply)
        QVERIFY(!renderCaptureReply);
    }

};

QTEST_MAIN(tst_QRenderCapture)

#include "tst_qrendercapture.moc"
