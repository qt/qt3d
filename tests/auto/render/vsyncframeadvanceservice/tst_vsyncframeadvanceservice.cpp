/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include <Qt3DRenderer/private/vsyncframeadvanceservice_p.h>

class FakeRenderThread Q_DECL_FINAL : public QThread
{
public:
    FakeRenderThread(Qt3D::Render::VSyncFrameAdvanceService *tickService)
        : m_tickService(tickService)
    {
    }

    // QThread interface
protected:
    void run() Q_DECL_FINAL
    {
        QThread::msleep(1000);
        m_tickService->proceedToNextFrame();
    }

private:
    Qt3D::Render::VSyncFrameAdvanceService *m_tickService;
};

class tst_VSyncFrameAdvanceService : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkSynchronisation()
    {
        // GIVEN
        Qt3D::Render::VSyncFrameAdvanceService tickService;
        FakeRenderThread renderThread(&tickService);
        QElapsedTimer t;

        // WHEN
        t.start();
        renderThread.start();
        tickService.waitForNextFrame();

        // THEN
        // we allow for a little margin by checking for 950
        // instead of 1000
        QVERIFY(t.elapsed() >= 950);
    }

};

QTEST_MAIN(tst_VSyncFrameAdvanceService)

#include "tst_vsyncframeadvanceservice.moc"
