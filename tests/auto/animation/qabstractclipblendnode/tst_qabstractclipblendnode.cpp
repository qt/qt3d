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
#include <Qt3DAnimation/qabstractclipblendnode.h>
#include <Qt3DAnimation/qanimationclip.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include "testpostmanarbiter.h"

namespace {

class TestClipBlendNode : public Qt3DAnimation::QAbstractClipBlendNode
{
public:
    TestClipBlendNode(Qt3DCore::QNode *parent = nullptr)
        : Qt3DAnimation::QAbstractClipBlendNode(parent)
    {}

    using Qt3DAnimation::QAbstractClipBlendNode::addClip;
    using Qt3DAnimation::QAbstractClipBlendNode::removeClip;
};

}

class tst_QAbstractClipBlendNode : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        TestClipBlendNode abstractClipBlendNode;

        // THEN
        QCOMPARE(abstractClipBlendNode.clips().size(), 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestClipBlendNode abstractClipBlendNode;

        {
            // WHEN
            Qt3DAnimation::QAnimationClip newValue;
            abstractClipBlendNode.addClip(&newValue);

            // THEN
            QCOMPARE(abstractClipBlendNode.clips().size(), 1);

            // WHEN
            abstractClipBlendNode.addClip(&newValue);

            // THEN
            QCOMPARE(abstractClipBlendNode.clips().size(), 1);

            // WHEN
            abstractClipBlendNode.removeClip(&newValue);

            // THEN
            QCOMPARE(abstractClipBlendNode.clips().size(), 0);
        }
    }

    void checkClipBookkeeping()
    {
        // GIVEN
        TestClipBlendNode abstractClipBlendNode;

        {
            // WHEN
            Qt3DAnimation::QAnimationClip clip;
            abstractClipBlendNode.addClip(&clip);

            QCOMPARE(abstractClipBlendNode.clips().size(), 1);
        }

        // THEN -> should not crash
        QCOMPARE(abstractClipBlendNode.clips().size(), 0);
    }

    void checkClipUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        TestClipBlendNode abstractClipBlendNode;
        Qt3DAnimation::QAnimationClip clip;
        arbiter.setArbiterOnNode(&abstractClipBlendNode);

        {
            // WHEN
            abstractClipBlendNode.addClip(&clip);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyNodeAddedChange>();
            QCOMPARE(change->propertyName(), "clip");
            QCOMPARE(change->addedNodeId(), clip.id());
            QCOMPARE(change->type(), Qt3DCore::PropertyValueAdded);

            arbiter.events.clear();
        }

        {
            // WHEN
            abstractClipBlendNode.removeClip(&clip);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyNodeRemovedChange>();
            QCOMPARE(change->propertyName(), "clip");
            QCOMPARE(change->removedNodeId(), clip.id());
            QCOMPARE(change->type(), Qt3DCore::PropertyValueRemoved);

            arbiter.events.clear();
        }

    }

    void checkParentClipBlendNode()
    {
        // GIVEN
        TestClipBlendNode clipNodeLevel0;
        TestClipBlendNode clipNodeLevel1_1(&clipNodeLevel0);
        TestClipBlendNode clipNodeLevel1_2(&clipNodeLevel0);
        TestClipBlendNode clipNodeLevel2_1(&clipNodeLevel1_1);
        Qt3DCore::QNode   fakeNodeLevel2_2(&clipNodeLevel1_2);
        TestClipBlendNode clipNodeLeve3_1(&fakeNodeLevel2_2);

        // THEN
        QVERIFY(clipNodeLevel0.parent() == nullptr);
        QCOMPARE(clipNodeLevel1_1.parent(), &clipNodeLevel0);
        QCOMPARE(clipNodeLevel1_2.parent(), &clipNodeLevel0);
        QCOMPARE(clipNodeLevel2_1.parent(), &clipNodeLevel1_1);
        QCOMPARE(fakeNodeLevel2_2.parent(), &clipNodeLevel1_2);
        QCOMPARE(clipNodeLeve3_1.parent(), &fakeNodeLevel2_2);

        QVERIFY(clipNodeLevel0.parentClipBlendNode() == nullptr);
        QCOMPARE(clipNodeLevel1_1.parentClipBlendNode(), &clipNodeLevel0);
        QCOMPARE(clipNodeLevel1_2.parentClipBlendNode(), &clipNodeLevel0);
        QCOMPARE(clipNodeLevel2_1.parentClipBlendNode(), &clipNodeLevel1_1);
        QCOMPARE(clipNodeLeve3_1.parentClipBlendNode(), &clipNodeLevel1_2);
    }
};

QTEST_MAIN(tst_QAbstractClipBlendNode)

#include "tst_qabstractclipblendnode.moc"
