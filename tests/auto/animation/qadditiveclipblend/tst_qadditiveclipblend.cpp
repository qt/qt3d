/****************************************************************************
**
** Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
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
#include <Qt3DAnimation/qadditiveclipblend.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qadditiveclipblend_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DAnimation/qclipblendnodecreatedchange.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include "testpostmanarbiter.h"

class tst_QAdditiveClipBlend : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QAdditiveClipBlend addBlend;

        // THEN
        QCOMPARE(addBlend.additiveFactor(), 0.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QAdditiveClipBlend addBlend;

        {
            // WHEN
            QSignalSpy spy(&addBlend, SIGNAL(additiveFactorChanged(float)));
            const float newValue = 0.5f;
            addBlend.setAdditiveFactor(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(addBlend.additiveFactor(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            addBlend.setAdditiveFactor(newValue);

            // THEN
            QCOMPARE(addBlend.additiveFactor(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DAnimation::QAdditiveClipBlend addBlend;
        Qt3DAnimation::QAnimationClipLoader clip1;
        Qt3DAnimation::QAnimationClipLoader clip2;

        addBlend.addClip(&clip1);
        addBlend.addClip(&clip2);
        addBlend.setAdditiveFactor(0.8f);


        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&addBlend);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 3); // 1 + 2 clips

            const auto creationChangeData = qSharedPointerCast<Qt3DAnimation::QClipBlendNodeCreatedChange<Qt3DAnimation::QAdditiveClipBlendData>>(creationChanges.first());
            const Qt3DAnimation::QAdditiveClipBlendData cloneData = creationChangeData->data;

            QCOMPARE(addBlend.additiveFactor(), cloneData.additiveFactor);
            QCOMPARE(addBlend.id(), creationChangeData->subjectId());
            QCOMPARE(addBlend.isEnabled(), true);
            QCOMPARE(addBlend.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(addBlend.metaObject(), creationChangeData->metaObject());
            QCOMPARE(creationChangeData->clips().size(), 2);
            QCOMPARE(creationChangeData->clips().first(), clip1.id());
            QCOMPARE(creationChangeData->clips().last(), clip2.id());
            QCOMPARE(creationChangeData->parentClipBlendNodeId(), Qt3DCore::QNodeId());
        }

        // WHEN
        addBlend.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&addBlend);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 3); // 1 + 2 clips

            const auto creationChangeData = qSharedPointerCast<Qt3DAnimation::QClipBlendNodeCreatedChange<Qt3DAnimation::QAdditiveClipBlendData>>(creationChanges.first());
            const Qt3DAnimation::QAdditiveClipBlendData cloneData = creationChangeData->data;

            QCOMPARE(addBlend.additiveFactor(), cloneData.additiveFactor);
            QCOMPARE(addBlend.id(), creationChangeData->subjectId());
            QCOMPARE(addBlend.isEnabled(), false);
            QCOMPARE(addBlend.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(addBlend.metaObject(), creationChangeData->metaObject());
            QCOMPARE(creationChangeData->clips().size(), 2);
            QCOMPARE(creationChangeData->clips().first(), clip1.id());
            QCOMPARE(creationChangeData->clips().last(), clip2.id());
            QCOMPARE(creationChangeData->parentClipBlendNodeId(), Qt3DCore::QNodeId());
        }
    }

    void checkBlendFactorUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QAdditiveClipBlend addBlend;
        arbiter.setArbiterOnNode(&addBlend);

        {
            // WHEN
            addBlend.setAdditiveFactor(0.4f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "additiveFactor");
            QCOMPARE(change->value().value<float>(), addBlend.additiveFactor());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            addBlend.setAdditiveFactor(0.4f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

};

QTEST_MAIN(tst_QAdditiveClipBlend)

#include "tst_qadditiveclipblend.moc"
