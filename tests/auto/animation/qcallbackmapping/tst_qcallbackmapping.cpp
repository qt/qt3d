// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DAnimation/qcallbackmapping.h>
#include <Qt3DAnimation/private/qcallbackmapping_p.h>
#include <Qt3DCore/qentity.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

class DummyCallback : public Qt3DAnimation::QAnimationCallback
{
public:
    void valueChanged(const QVariant &) override { }
};

class tst_QCallbackMapping : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QCallbackMapping mapping;

        // THEN
        QCOMPARE(mapping.channelName(), QString());
        QCOMPARE(mapping.callback(), static_cast<Qt3DAnimation::QAnimationCallback *>(nullptr));
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QCallbackMapping mapping;

        {
            // WHEN
            QSignalSpy spy(&mapping, SIGNAL(channelNameChanged(QString)));
            const QString newValue(QStringLiteral("Rotation"));
            mapping.setChannelName(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mapping.channelName(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mapping.setChannelName(newValue);

            // THEN
            QCOMPARE(mapping.channelName(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            auto newValue = new DummyCallback();
            mapping.setCallback(QMetaType::QQuaternion, newValue);

            // THEN - no signals for callback
            QCOMPARE(mapping.callback(), newValue);
        }
    }

    void checkPropertyUpdateChanges()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QCallbackMapping mapping;
        arbiter.setArbiterOnNode(&mapping);

        {
            // WHEN
            mapping.setChannelName(QStringLiteral("Scale"));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mapping);

            arbiter.clear();

            // WHEN
            mapping.setChannelName(QStringLiteral("Scale"));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        {
            // WHEN
            auto callback = new DummyCallback();
            mapping.setCallback(QMetaType::QVector3D, callback, Qt3DAnimation::QAnimationCallback::OnThreadPool);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mapping);

            arbiter.clear();

            // WHEN
            mapping.setCallback(QMetaType::QVector3D, callback, Qt3DAnimation::QAnimationCallback::OnThreadPool);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }
};

QTEST_MAIN(tst_QCallbackMapping)

#include "tst_qcallbackmapping.moc"
