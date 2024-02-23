// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qanimationcliploader_p.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

class tst_QAnimationClipLoader : public Qt3DAnimation::QAnimationClipLoader
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QAnimationClipLoader clip;

        // THEN
        QCOMPARE(clip.source(), QUrl());
        QCOMPARE(clip.duration(), 0.0f);
        QCOMPARE(clip.status(), Qt3DAnimation::QAnimationClipLoader::NotReady);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QAnimationClipLoader clip;

        {
            // WHEN
            QSignalSpy spy(&clip, SIGNAL(sourceChanged(QUrl)));
            const QUrl newValue(QStringLiteral("qrc:/walk.qlip"));
            clip.setSource(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(clip.source(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            clip.setSource(newValue);

            // THEN
            QCOMPARE(clip.source(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkSourceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QAnimationClipLoader clip;
        arbiter.setArbiterOnNode(&clip);

        {
            // WHEN
            clip.setSource(QUrl(QStringLiteral("qrc:/toyplane.qlip")));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &clip);

            arbiter.clear();
        }

        {
            // WHEN
            clip.setSource(QStringLiteral("qrc:/toyplane.qlip"));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }
};

QTEST_MAIN(tst_QAnimationClipLoader)

#include "tst_qanimationcliploader.moc"
