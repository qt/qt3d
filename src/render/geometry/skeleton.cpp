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

#include "skeleton_p.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>

#include <Qt3DCore/qjoint.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DCore/private/qskeletoncreatedchange_p.h>
#include <Qt3DCore/private/qskeleton_p.h>
#include <Qt3DCore/private/qskeletonloader_p.h>
#include <Qt3DCore/private/qmath3d_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Skeleton::Skeleton()
    : BackendNode(Qt3DCore::QBackendNode::ReadWrite)
    , m_status(Qt3DCore::QSkeletonLoader::NotReady)
    , m_createJoints(false)
    , m_dataType(Unknown)
    , m_skeletonManager(nullptr)
    , m_jointManager(nullptr)
{
}

void Skeleton::cleanup()
{
    m_source.clear();
    m_status = Qt3DCore::QSkeletonLoader::NotReady;
    m_createJoints = false;
    m_rootJointId = Qt3DCore::QNodeId();
    clearData();
    setEnabled(false);
}

void Skeleton::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAbstractSkeleton *node = qobject_cast<const QAbstractSkeleton *>(frontEnd);
    if (!node)
        return;
    const QSkeleton *skeletonNode = qobject_cast<const QSkeleton *>(frontEnd);
    const QSkeletonLoader *loaderNode = qobject_cast<const QSkeletonLoader *>(frontEnd);

    if (firstTime) {
        m_skeletonHandle = m_skeletonManager->lookupHandle(peerId());

        if (skeletonNode) {
            m_dataType = Data;
            m_rootJointId = skeletonNode->rootJoint()->id();
            if (!m_rootJointId.isNull()) {
                markDirty(AbstractRenderer::SkeletonDataDirty);
                m_skeletonManager->addDirtySkeleton(SkeletonManager::SkeletonDataDirty, m_skeletonHandle);
            }
        }

        if (loaderNode) {
            m_dataType = File;
            m_source = loaderNode->source();
            if (!m_source.isEmpty()) {
                markDirty(AbstractRenderer::SkeletonDataDirty);
                m_skeletonManager->addDirtySkeleton(SkeletonManager::SkeletonDataDirty, m_skeletonHandle);
            }
        }
    }

    if (loaderNode) {
        if (loaderNode->source() != m_source) {
            m_source = loaderNode->source();
            markDirty(AbstractRenderer::SkeletonDataDirty);
            m_skeletonManager->addDirtySkeleton(SkeletonManager::SkeletonDataDirty, m_skeletonHandle);
        }
        m_createJoints = loaderNode->isCreateJointsEnabled();

        auto newJointId = Qt3DCore::qIdForNode(loaderNode->rootJoint());
        if (newJointId != m_rootJointId) {
            m_rootJointId = newJointId;

            // If using a QSkeletonLoader to create frontend QJoints, when those joints are
            // set on the skeleton, we end up here. In order to allow the subsequent call
            // to loadSkeleton(), see below, to build the internal data from the frontend
            // joints rather than from the source url again, we need to change the data type
            // to Data.
            m_dataType = Data;

            // If the joint changes, we need to rebuild our internal SkeletonData and
            // the relationships between joints and skeleton. Mark the skeleton data as
            // dirty so that we get a loadSkeletonJob executed to process this skeleton.
            if (!m_rootJointId.isNull()) {
                markDirty(AbstractRenderer::SkeletonDataDirty);
                m_skeletonManager->addDirtySkeleton(SkeletonManager::SkeletonDataDirty, m_skeletonHandle);
            }
        }
    }

    auto d = Qt3DCore::QAbstractSkeletonPrivate::get(node);
    m_skeletonData.localPoses = d->m_localPoses;
}

void Skeleton::setStatus(QSkeletonLoader::Status status)
{
    if (status != m_status)
        m_status = status;
}

void Skeleton::clearData()
{
    m_name.clear();
    m_skeletonData.joints.clear();
    m_skeletonData.localPoses.clear();
    m_skeletonData.jointNames.clear();
    m_skeletonData.jointIndices.clear();
}

void Skeleton::setSkeletonData(const SkeletonData &data)
{
    m_skeletonData = data;
    m_skinningPalette.resize(m_skeletonData.joints.size());
}

// Called from UpdateSkinningPaletteJob
void Skeleton::setLocalPose(HJoint jointHandle, const Qt3DCore::Sqt &localPose)
{
    // Find the corresponding index into the JointInfo vector
    // and set the local pose
    const int jointIndex = m_skeletonData.jointIndices.value(jointHandle, -1);
    Q_ASSERT(jointIndex != -1);
    m_skeletonData.localPoses[jointIndex] = localPose;
}

QVector<QMatrix4x4> Skeleton::calculateSkinningMatrixPalette()
{
    const QVector<Sqt> &localPoses = m_skeletonData.localPoses;
    QVector<JointInfo> &joints = m_skeletonData.joints;
    for (int i = 0; i < m_skeletonData.joints.size(); ++i) {
        // Calculate the global pose of this joint
        JointInfo &joint = joints[i];
        if (joint.parentIndex == -1) {
            joint.globalPose = localPoses[i].toMatrix();
        } else {
            JointInfo &parentJoint = joints[joint.parentIndex];
            joint.globalPose = parentJoint.globalPose * localPoses[i].toMatrix();
        }

        m_skinningPalette[i] = joint.globalPose * joint.inverseBindPose;
    }
    return m_skinningPalette;
}


SkeletonFunctor::SkeletonFunctor(AbstractRenderer *renderer,
                                 SkeletonManager *skeletonManager,
                                 JointManager *jointManager)
    : m_renderer(renderer)
    , m_skeletonManager(skeletonManager)
    , m_jointManager(jointManager)
{
}

Qt3DCore::QBackendNode *SkeletonFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    Skeleton *backend = m_skeletonManager->getOrCreateResource(change->subjectId());
    backend->setRenderer(m_renderer);
    backend->setSkeletonManager(m_skeletonManager);
    backend->setJointManager(m_jointManager);
    return backend;
}

Qt3DCore::QBackendNode *SkeletonFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_skeletonManager->lookupResource(id);
}

void SkeletonFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_skeletonManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
