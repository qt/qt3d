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
#include <Qt3DAnimation/qclipblendvalue.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qclipblendvalue_p.h>
#include <Qt3DAnimation/private/clipblendvalue_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "qbackendnodetester.h"

class tst_ClipBlendValue : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkInitialState()
    {
        // GIVEN
        Qt3DAnimation::Animation::ClipBlendValue backendClipBlendValue;

        // THEN
        QCOMPARE(backendClipBlendValue.isEnabled(), false);
        QVERIFY(backendClipBlendValue.peerId().isNull());
        QCOMPARE(backendClipBlendValue.clipId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendValue.blendType(), Qt3DAnimation::Animation::ClipBlendNode::ValueType);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QClipBlendValue clipBlendValue;
        Qt3DAnimation::QAnimationClipLoader clip;
        clipBlendValue.setClip(&clip);

        {
            // WHEN
            Qt3DAnimation::Animation::ClipBlendValue backendClipBlendValue;
            simulateInitialization(&clipBlendValue, &backendClipBlendValue);

            // THEN
            QCOMPARE(backendClipBlendValue.isEnabled(), true);
            QCOMPARE(backendClipBlendValue.peerId(), clipBlendValue.id());
            QCOMPARE(backendClipBlendValue.clipId(), clip.id());
        }
        {
            // WHEN
            Qt3DAnimation::Animation::ClipBlendValue backendClipBlendValue;
            clipBlendValue.setEnabled(false);
            simulateInitialization(&clipBlendValue, &backendClipBlendValue);

            // THEN
            QCOMPARE(backendClipBlendValue.peerId(), clipBlendValue.id());
            QCOMPARE(backendClipBlendValue.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DAnimation::Animation::ClipBlendValue backendClipBlendValue;
        {
            // WHEN
            const bool newValue = false;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("enabled");
            change->setValue(newValue);
            backendClipBlendValue.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendClipBlendValue.isEnabled(), newValue);
        }
        {
            // WHEN
            const Qt3DAnimation::QAnimationClipLoader newValue;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("clip");
            change->setValue(QVariant::fromValue(newValue.id()));
            backendClipBlendValue.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendClipBlendValue.clipId(), newValue.id());
        }
    }
};

QTEST_MAIN(tst_ClipBlendValue)

#include "tst_clipblendvalue.moc"
