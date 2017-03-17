/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "clipblendnode_p.h"
#include <Qt3DAnimation/qclipblendnodecreatedchange.h>
#include <Qt3DAnimation/qabstractanimationclip.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

namespace Animation {

ClipBlendNode::ClipBlendNode(BlendType blendType)
    : BackendNode(ReadOnly) // Makes sense for now at least
    , m_manager(nullptr)
    , m_blendType(blendType)
{
}

ClipBlendNode::~ClipBlendNode()
{
}

Qt3DCore::QNodeId ClipBlendNode::parentId() const
{
    return m_parentId;
}

Qt3DCore::QNodeIdVector ClipBlendNode::childrenIds() const
{
    return m_childrenIds;
}

Qt3DCore::QNodeIdVector ClipBlendNode::clipIds() const
{
    return m_clipIds;
}

void ClipBlendNode::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    switch (e->type()) {

    case Qt3DCore::PropertyValueAdded: {
       Qt3DCore::QPropertyNodeAddedChangePtr change = qSharedPointerCast<Qt3DCore::QPropertyNodeAddedChange>(e);
        if (change->metaObject()->inherits(&QAbstractClipBlendNode::staticMetaObject))
            addChildId(change->addedNodeId());
        else if (change->metaObject()->inherits(&QAbstractAnimationClip::staticMetaObject))
            m_clipIds.push_back(change->addedNodeId());
        break;
    }

    case Qt3DCore::PropertyValueRemoved: {
        Qt3DCore::QPropertyNodeRemovedChangePtr change = qSharedPointerCast<Qt3DCore::QPropertyNodeRemovedChange>(e);
        if (change->metaObject()->inherits(&QAbstractClipBlendNode::staticMetaObject))
            removeChildId(change->removedNodeId());
        else if (change->metaObject()->inherits(&QAbstractAnimationClip::staticMetaObject))
            m_clipIds.removeOne(change->removedNodeId());
        break;
    }

    default:
        break;
    }

    Qt3DCore::QBackendNode::sceneChangeEvent(e);
}

void ClipBlendNode::setClipBlendNodeManager(ClipBlendNodeManager *manager)
{
    m_manager = manager;
}



void ClipBlendNode::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto creationChange = qSharedPointerCast<QClipBlendNodeCreatedChangeBase>(change);
    setParentId(creationChange->parentClipBlendNodeId());
    m_clipIds = creationChange->clips();
}

void ClipBlendNode::setParentId(Qt3DCore::QNodeId parentId)
{
    if (parentId != m_parentId) {
        // We already had a parent, tell it to abandon us
        if (!m_parentId.isNull()) {
            ClipBlendNode *parent = m_manager->lookupNode(m_parentId);
            if (parent != nullptr)
                parent->m_childrenIds.removeAll(peerId());
        }
        m_parentId = parentId;
        ClipBlendNode *parent = m_manager->lookupNode(m_parentId);
        if (parent != nullptr && !parent->m_childrenIds.contains(peerId()))
            parent->m_childrenIds.append(peerId());
    }
}

void ClipBlendNode::addChildId(Qt3DCore::QNodeId childId)
{
    if (!m_childrenIds.contains(childId)) {
        ClipBlendNode *child = m_manager->lookupNode(childId);
        if (child != nullptr) {
            m_childrenIds.push_back(childId);
            child->m_parentId = peerId();
        }
    }
}

void ClipBlendNode::removeChildId(Qt3DCore::QNodeId childId)
{
    if (m_childrenIds.contains(childId)) {
        ClipBlendNode *child = m_manager->lookupNode(childId);
        if (child != nullptr)
            child->m_parentId = Qt3DCore::QNodeId();
        m_childrenIds.removeAll(childId);
    }
}

ClipBlendNode::BlendType Animation::ClipBlendNode::blendType() const
{
    return m_blendType;
}

void ClipBlendNode::setClipResults(Qt3DCore::QNodeId animatorId, const ClipResults &clipResults)
{
    // Do we already have an entry for this animator?
    const int animatorIndex = m_animatorIds.indexOf(animatorId);
    if (animatorIndex == -1) {
        // Nope, add it
        m_animatorIds.push_back(animatorId);
        m_clipResults.push_back(clipResults);
    } else {
        m_clipResults[animatorIndex] = clipResults;
    }
}

ClipResults ClipBlendNode::clipResults(Qt3DCore::QNodeId animatorId) const
{
    const int animatorIndex = m_animatorIds.indexOf(animatorId);
    if (animatorIndex != -1)
        return m_clipResults[animatorIndex];
    return ClipResults();
}

/*!
    \fn QVector<Qt3DCore::QNodeId> ClipBlendNode::dependencyIds() const
    \internal

    Each subclass of ClipBlendNode must implement this function such that it
    returns a vector of the ids of ClipBlendNodes upon which is it dependent
    in order to be able to evaluate given its current internal state.

    For example, a subclass implementing a simple lerp blend between two
    other nodes, would always return the ids of the nodes between which it
    is lerping.

    A more generalised lerp node that is capable of lerping between a
    series of nodes would return the ids of the two nodes that correspond
    to the blend values which sandwich the currently set blend value.

    The animation handler will submit a job that uses this function to
    build a list of clips that must be evaluated in order to later
    evaluate the entire blend tree. In this way, the clips can all be
    evaluated in one pass, and the tree in a subsequent pass.
*/

/*!
    \internal

    Fetches the ClipResults from the nodes listed in the dependencyIds
    and passes them to the doBlend() virtual function which should be
    implemented in subclasses to perform the actual blend operation.
    The results are then inserted into the clip results for this blend
    node indexed by the \a animatorId.
*/
void ClipBlendNode::performBlend(Qt3DCore::QNodeId animatorId)
{
    // Obtain the clip results from each of the dependencies
    const QVector<Qt3DCore::QNodeId> dependencyNodeIds = dependencyIds();
    const int dependencyCount = dependencyNodeIds.size();
    QVector<ClipResults> blendData;
    blendData.reserve(dependencyCount);
    for (const auto dependencyId : dependencyNodeIds) {
        ClipBlendNode *dependencyNode = clipBlendNodeManager()->lookupNode(dependencyId);
        ClipResults blendDataElement = dependencyNode->clipResults(animatorId);
        blendData.push_back(blendDataElement);
    }

    // Ask the blend node to perform the actual blend operation on the data
    // from the dependencies
    ClipResults blendedResults = doBlend(blendData);
    setClipResults(animatorId, blendedResults);
}

} // Animation

} // Qt3DAnimation

QT_END_NAMESPACE
