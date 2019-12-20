/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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
