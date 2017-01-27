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
#include <Qt3DAnimation/qlerpblend.h>
#include <Qt3DAnimation/qanimationclip.h>
#include <Qt3DAnimation/private/qabstractclipblendnode_p.h>
#include <Qt3DAnimation/private/clipblendnode_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include "qbackendnodetester.h"

namespace {

class TestClipBlendNode : public Qt3DAnimation::Animation::ClipBlendNode
{
public:
    TestClipBlendNode()
        : Qt3DAnimation::Animation::ClipBlendNode(Qt3DAnimation::Animation::ClipBlendNode::LerpBlendType)
    {}
};

} // anonymous

class tst_ClipBlendNode : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        TestClipBlendNode backendClipBlendNode;

        // THEN
        QCOMPARE(backendClipBlendNode.isEnabled(), false);
        QVERIFY(backendClipBlendNode.peerId().isNull());
        QCOMPARE(backendClipBlendNode.clipIds(), Qt3DCore::QNodeIdVector());
        QCOMPARE(backendClipBlendNode.parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendNode.childrenIds(), Qt3DCore::QNodeIdVector());
        QVERIFY(backendClipBlendNode.clipBlendNodeManager() == nullptr);
        QCOMPARE(backendClipBlendNode.blendType(), Qt3DAnimation::Animation::ClipBlendNode::LerpBlendType);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QLerpBlend clipBlendNode;
        Qt3DAnimation::QAnimationClip clip;
        clipBlendNode.addClip(&clip);

        QCoreApplication::processEvents();

        {
            // WHEN
            Qt3DAnimation::Animation::ClipBlendNodeManager manager;
            TestClipBlendNode backendClipBlendNode;
            backendClipBlendNode.setClipBlendNodeManager(&manager);
            simulateInitialization(&clipBlendNode, &backendClipBlendNode);

            // THEN
            QCOMPARE(backendClipBlendNode.isEnabled(), true);
            QCOMPARE(backendClipBlendNode.peerId(), clipBlendNode.id());
            QCOMPARE(backendClipBlendNode.clipIds().size(), 1);
            QCOMPARE(backendClipBlendNode.clipIds().first(), clip.id());
            QCOMPARE(backendClipBlendNode.parentId(), Qt3DCore::QNodeId());
            QCOMPARE(backendClipBlendNode.childrenIds().size(), 0);
            QCOMPARE(backendClipBlendNode.clipBlendNodeManager(), &manager);
            QCOMPARE(backendClipBlendNode.blendType(), Qt3DAnimation::Animation::ClipBlendNode::LerpBlendType);
        }
        {
            // WHEN
            Qt3DAnimation::Animation::ClipBlendNodeManager manager;
            TestClipBlendNode backendClipBlendNode;
            clipBlendNode.setEnabled(false);
            backendClipBlendNode.setClipBlendNodeManager(&manager);
            simulateInitialization(&clipBlendNode, &backendClipBlendNode);

            // THEN
            QCOMPARE(backendClipBlendNode.peerId(), clipBlendNode.id());
            QCOMPARE(backendClipBlendNode.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        TestClipBlendNode backendClipBlendNode;
        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        backendClipBlendNode.setClipBlendNodeManager(&manager);
        {
            // WHEN
            const bool newValue = false;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("enabled");
            change->setValue(newValue);
            backendClipBlendNode.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendClipBlendNode.isEnabled(), newValue);
        }
        {
            // WHEN
            Qt3DAnimation::QAnimationClip clip;
            // To geneate the type_info in the QNodePrivate of clip
            Qt3DCore::QNodeCreatedChangeGenerator generator(&clip);

            const auto addedChange = Qt3DCore::QPropertyNodeAddedChangePtr::create(Qt3DCore::QNodeId(), &clip);
            backendClipBlendNode.sceneChangeEvent(addedChange);

            // THEN
            QCOMPARE(backendClipBlendNode.clipIds().size(), 1);
            QCOMPARE(backendClipBlendNode.clipIds().first(), clip.id());

            // WHEN
            const auto removedChange = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &clip);
            backendClipBlendNode.sceneChangeEvent(removedChange);

            // THEN
            QCOMPARE(backendClipBlendNode.clipIds().size(), 0);
        }
        {
            // WHEN
            Qt3DAnimation::QLerpBlend clipBlendChild;
            // Will be destroyed when manager is destroyed
            TestClipBlendNode *backenChildClipBlendNode = new TestClipBlendNode();
            backendClipBlendNode.setClipBlendNodeManager(&manager);
            manager.appendNode(clipBlendChild.id(), backenChildClipBlendNode);

            // To geneate the type_info in the QNodePrivate of clipBlendChild
            Qt3DCore::QNodeCreatedChangeGenerator generator(&clipBlendChild);
            const auto addChange = Qt3DCore::QPropertyNodeAddedChangePtr::create(Qt3DCore::QNodeId(), &clipBlendChild);
            backendClipBlendNode.sceneChangeEvent(addChange);

            // THEN
            QCOMPARE(backendClipBlendNode.childrenIds().size(), 1);
            QCOMPARE(backendClipBlendNode.childrenIds().first(), clipBlendChild.id());

            // WHEN
            const auto removedChange = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &clipBlendChild);
            backendClipBlendNode.sceneChangeEvent(removedChange);

            // THEN
            QCOMPARE(backendClipBlendNode.childrenIds().size(), 0);
        }
    }

    void checkParentInitialization()
    {
        // GIVEN
        TestClipBlendNode *backendClipBlendNode = new TestClipBlendNode();
        TestClipBlendNode *backendChildClipBlendNode = new TestClipBlendNode();
        Qt3DAnimation::QLerpBlend clipBlendParent;
        Qt3DAnimation::QLerpBlend childClipBlend(&clipBlendParent);
        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        backendClipBlendNode->setClipBlendNodeManager(&manager);
        backendChildClipBlendNode->setClipBlendNodeManager(&manager);

        // THEN
        QCOMPARE(backendClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendNode->childrenIds().size(), 0);
        QCOMPARE(backendChildClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendChildClipBlendNode->childrenIds().size(), 0);

        // WHEN
        manager.appendNode(clipBlendParent.id(), backendClipBlendNode);
        manager.appendNode(childClipBlend.id(), backendChildClipBlendNode);
        simulateInitialization(&clipBlendParent, backendClipBlendNode);
        simulateInitialization(&childClipBlend, backendChildClipBlendNode);

        // THEN
        QCOMPARE(backendClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendNode->childrenIds().size(), 1);
        QCOMPARE(backendClipBlendNode->childrenIds().first(), childClipBlend.id());
        QCOMPARE(backendChildClipBlendNode->parentId(), clipBlendParent.id());
        QCOMPARE(backendChildClipBlendNode->childrenIds().size(), 0);

        // WHEN
        // To geneate the type_info in the QNodePrivate of clipBlendChild
        Qt3DCore::QNodeCreatedChangeGenerator generator(&childClipBlend);
        const auto removedChange = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &childClipBlend);
        backendClipBlendNode->sceneChangeEvent(removedChange);

        // THEN
        QCOMPARE(backendClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendNode->childrenIds().size(), 0);
        QCOMPARE(backendChildClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendChildClipBlendNode->childrenIds().size(), 0);
    }

};

QTEST_MAIN(tst_ClipBlendNode)

#include "tst_clipblendnode.moc"
