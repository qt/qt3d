/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            mapping.setChannelName(newValue);

            // THEN
            QCOMPARE(mapping.channelName(), newValue);
            QCOMPARE(spy.count(), 0);
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
