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
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/private/qfilterkey_p.h>
#include <Qt3DRender/private/filterkey_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_FilterKey : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::FilterKey backendFilterKey;

        // THEN
        QCOMPARE(backendFilterKey.isEnabled(), false);
        QVERIFY(backendFilterKey.peerId().isNull());
        QCOMPARE(backendFilterKey.value(), QVariant());
        QCOMPARE(backendFilterKey.name(), QString());
    }

    void checkCleanupState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::FilterKey backendFilterKey;

        // WHEN
        backendFilterKey.setRenderer(&renderer);
        backendFilterKey.setEnabled(true);

        {
            Qt3DRender::QFilterKey filterKey;
            filterKey.setName(QStringLiteral("Tim"));
            filterKey.setValue(QVariant(QStringLiteral("McGraw")));
            simulateInitializationSync(&filterKey, &backendFilterKey);
        }

        backendFilterKey.cleanup();

        // THEN
        QCOMPARE(backendFilterKey.isEnabled(), false);
        QCOMPARE(backendFilterKey.value(), QVariant());
        QCOMPARE(backendFilterKey.name(), QString());
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QFilterKey filterKey;
        filterKey.setName(QStringLiteral("Dallas"));
        filterKey.setValue(QVariant(QStringLiteral("Smith")));

        {
            // WHEN
            Qt3DRender::Render::FilterKey backendFilterKey;
            backendFilterKey.setRenderer(&renderer);
            simulateInitializationSync(&filterKey, &backendFilterKey);

            // THEN
            QCOMPARE(backendFilterKey.isEnabled(), true);
            QCOMPARE(backendFilterKey.peerId(), filterKey.id());
            QCOMPARE(backendFilterKey.value(), QVariant(QStringLiteral("Smith")));
            QCOMPARE(backendFilterKey.name(), QStringLiteral("Dallas"));
        }
        {
            // WHEN
            Qt3DRender::Render::FilterKey backendFilterKey;
            backendFilterKey.setRenderer(&renderer);
            filterKey.setEnabled(false);
            simulateInitializationSync(&filterKey, &backendFilterKey);

            // THEN
            QCOMPARE(backendFilterKey.peerId(), filterKey.id());
            QCOMPARE(backendFilterKey.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::FilterKey backendFilterKey;
        Qt3DRender::QFilterKey frontend;
        TestRenderer renderer;
        backendFilterKey.setRenderer(&renderer);
        simulateInitializationSync(&frontend, &backendFilterKey);

        {
            // WHEN
            const bool newValue = false;
            frontend.setEnabled(newValue);
            backendFilterKey.syncFromFrontEnd(&frontend, false);

            // THEN
            QCOMPARE(backendFilterKey.isEnabled(), newValue);
        }
        {
            // WHEN
            const QVariant newValue(383.0f);
            frontend.setValue(newValue);
            backendFilterKey.syncFromFrontEnd(&frontend, false);

            // THEN
            QCOMPARE(backendFilterKey.value(), newValue);
        }
        {
            // WHEN
            const QString newValue = QStringLiteral("Alan");
            frontend.setName(newValue);
            backendFilterKey.syncFromFrontEnd(&frontend, false);

            // THEN
            QCOMPARE(backendFilterKey.name(), newValue);
        }
    }

    void checkComparison()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::FilterKey backendFilterKey1;
        Qt3DRender::Render::FilterKey backendFilterKey2;
        Qt3DRender::Render::FilterKey backendFilterKey3;
        Qt3DRender::Render::FilterKey backendFilterKey4;
        backendFilterKey1.setRenderer(&renderer);
        backendFilterKey2.setRenderer(&renderer);
        backendFilterKey3.setRenderer(&renderer);
        backendFilterKey4.setRenderer(&renderer);

        // WHEN
        {
            Qt3DRender::QFilterKey filterKey1;
            filterKey1.setName(QStringLiteral("Dallas"));
            filterKey1.setValue(QVariant(QStringLiteral("Smith")));

            simulateInitializationSync(&filterKey1, &backendFilterKey1);
            simulateInitializationSync(&filterKey1, &backendFilterKey4);

            Qt3DRender::QFilterKey filterKey2;
            filterKey2.setName(QStringLiteral("Tim"));
            filterKey2.setValue(QVariant(QStringLiteral("Smith")));

            simulateInitializationSync(&filterKey2, &backendFilterKey2);

            Qt3DRender::QFilterKey filterKey3;
            filterKey3.setName(QStringLiteral("Dallas"));
            filterKey3.setValue(QVariant(QStringLiteral("McGraw")));

            simulateInitializationSync(&filterKey3, &backendFilterKey3);
        }

        // THEN
        QVERIFY(backendFilterKey1 == backendFilterKey4);
        QVERIFY(backendFilterKey4 == backendFilterKey1);
        QVERIFY(backendFilterKey1 != backendFilterKey2);
        QVERIFY(backendFilterKey2 != backendFilterKey1);
        QVERIFY(backendFilterKey1 != backendFilterKey3);
        QVERIFY(backendFilterKey3 != backendFilterKey2);
    }

};

QTEST_MAIN(tst_FilterKey)

#include "tst_filterkey.moc"
