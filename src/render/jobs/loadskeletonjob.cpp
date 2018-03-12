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

#include "loadskeletonjob_p.h"
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qabstractskeleton.h>
#include <Qt3DCore/qskeletonloader.h>
#include <Qt3DCore/private/qabstractskeleton_p.h>
#include <Qt3DCore/private/qabstractnodefactory_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qskeletonloader_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/qurlhelper_p.h>
#include <Qt3DRender/private/gltfskeletonloader_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class LoadSkeletonJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    LoadSkeletonJobPrivate() : m_backendSkeleton(nullptr), m_loadedRootJoint(nullptr) { }
    ~LoadSkeletonJobPrivate() override { }

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    Skeleton *m_backendSkeleton;
    Qt3DCore::QJoint* m_loadedRootJoint;
};

LoadSkeletonJob::LoadSkeletonJob(const HSkeleton &handle)
    : QAspectJob(*new LoadSkeletonJobPrivate)
    , m_handle(handle)
    , m_nodeManagers(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::LoadSkeleton, 0)
}

void LoadSkeletonJob::run()
{
    Q_D(LoadSkeletonJob);
    d->m_backendSkeleton = nullptr;

    Skeleton *skeleton = m_nodeManagers->skeletonManager()->data(m_handle);
    if (skeleton != nullptr) {
        d->m_backendSkeleton = skeleton;
        loadSkeleton(skeleton);
    }
}

void LoadSkeletonJob::loadSkeleton(Skeleton *skeleton)
{
    qCDebug(Jobs) << Q_FUNC_INFO << skeleton->source();
    skeleton->clearData();

    // Load the data
    switch (skeleton->dataType()) {
    case Skeleton::File:
        loadSkeletonFromUrl(skeleton);
        break;

    case Skeleton::Data:
        loadSkeletonFromData(skeleton);
        break;

    default:
        Q_UNREACHABLE();
    }

    // If using a loader inform the frontend of the status change.
    // Don't bother if asked to create frontend joints though. When
    // the backend gets notified of those joints we'll update the
    // status at that point.
    if (skeleton->dataType() == Skeleton::File && !skeleton->createJoints()) {
        if (skeleton->jointCount() == 0)
            skeleton->setStatus(Qt3DCore::QSkeletonLoader::Error);
        else
            skeleton->setStatus(Qt3DCore::QSkeletonLoader::Ready);
    }

    qCDebug(Jobs) << "Loaded skeleton data:" << *skeleton;
}

void LoadSkeletonJob::loadSkeletonFromUrl(Skeleton *skeleton)
{
    Q_D(LoadSkeletonJob);

    using namespace Qt3DCore;

    // TODO: Handle remote files
    QString filePath = Qt3DRender::QUrlHelper::urlToLocalFileOrQrc(skeleton->source());
    QFileInfo info(filePath);
    if (!info.exists()) {
        qWarning() << "Could not open skeleton file:" << filePath;
        skeleton->setStatus(Qt3DCore::QSkeletonLoader::Error);
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open skeleton file:" << filePath;
        skeleton->setStatus(QSkeletonLoader::Error);
        return;
    }

    // TODO: Make plugin based for more file type support. For now gltf or native
    const QString ext = info.suffix();
    SkeletonData skeletonData;
    if (ext == QLatin1String("gltf")) {
        GLTFSkeletonLoader loader;
        loader.load(&file);
        skeletonData = loader.createSkeleton(skeleton->name());

        // If the user has requested it, create the frontend nodes for the joints
        // and send them to the (soon to be owning) QSkeletonLoader.
        if (skeleton->createJoints()) {
            QJoint *rootJoint = createFrontendJoints(skeletonData);
            if (!rootJoint) {
                qWarning() << "Failed to create frontend joints";
                skeleton->setStatus(QSkeletonLoader::Error);
                return;
            }

            // Move the QJoint tree to the main thread and notify the
            // corresponding QSkeletonLoader
            const auto appThread = QCoreApplication::instance()->thread();
            rootJoint->moveToThread(appThread);

            d->m_loadedRootJoint = rootJoint;

            // Clear the skeleton data. It will be recreated from the
            // frontend joints. A little bit inefficient but ensures
            // that joints created this way and via QSkeleton go through
            // the same code path.
            skeletonData = SkeletonData();
        }
    } else if (ext == QLatin1String("json")) {
        // TODO: Support native skeleton type
    } else {
        qWarning() << "Unknown skeleton file type:" << ext;
        skeleton->setStatus(QSkeletonLoader::Error);
        return;
    }

    skeleton->setSkeletonData(skeletonData);
}

void LoadSkeletonJob::loadSkeletonFromData(Skeleton *skeleton)
{
    // Recurse down through the joint hierarchy and process it into
    // the vector of joints used within SkeletonData. The recursion
    // ensures that a parent always appears before its children in
    // the vector of JointInfo objects.
    //
    // In addition, we set up a mapping from the joint ids to the
    // index of the corresponding JointInfo object in the vector.
    // This will allow us to easily update entries in the vector of
    // JointInfos when a Joint node marks itself as dirty.
    const int rootParentIndex = -1;
    auto skeletonData = skeleton->skeletonData();
    processJointHierarchy(skeleton->rootJointId(), rootParentIndex, skeletonData);
    skeleton->setSkeletonData(skeletonData);
}

Qt3DCore::QJoint *LoadSkeletonJob::createFrontendJoints(const SkeletonData &skeletonData) const
{
    if (skeletonData.joints.isEmpty())
        return nullptr;

    // Create frontend joints from the joint info objects
    QVector<Qt3DCore::QJoint *> frontendJoints;
    const int jointCount = skeletonData.joints.size();
    frontendJoints.reserve(jointCount);
    for (int i = 0; i < jointCount; ++i) {
        const QMatrix4x4 &inverseBindMatrix = skeletonData.joints[i].inverseBindPose;
        const QString &jointName = skeletonData.jointNames[i];
        const Qt3DCore::Sqt &localPose = skeletonData.localPoses[i];
        frontendJoints.push_back(createFrontendJoint(jointName, localPose, inverseBindMatrix));
    }

    // Now go through and resolve the parent for each joint
    for (int i = 0; i < frontendJoints.size(); ++i) {
        const auto parentIndex = skeletonData.joints[i].parentIndex;
        if (parentIndex == -1)
            continue;

        // It's not enough to just set up the QObject parent-child relationship.
        // We need to explicitly add the child to the parent's list of joints so
        // that information is then propagated to the backend.
        frontendJoints[parentIndex]->addChildJoint(frontendJoints[i]);
    }

    return frontendJoints[0];
}

Qt3DCore::QJoint *LoadSkeletonJob::createFrontendJoint(const QString &jointName,
                                                       const Qt3DCore::Sqt &localPose,
                                                       const QMatrix4x4 &inverseBindMatrix) const
{
    auto joint = Qt3DCore::QAbstractNodeFactory::createNode<Qt3DCore::QJoint>("QJoint");
    joint->setTranslation(localPose.translation);
    joint->setRotation(localPose.rotation);
    joint->setScale(localPose.scale);
    joint->setInverseBindMatrix(inverseBindMatrix);
    joint->setName(jointName);
    return joint;
}

void LoadSkeletonJob::processJointHierarchy(Qt3DCore::QNodeId jointId,
                                            int parentJointIndex,
                                            SkeletonData &skeletonData)
{
    // Lookup the joint, create a JointInfo, and add an entry to the index map
    Joint *joint = m_nodeManagers->jointManager()->lookupResource(jointId);
    Q_ASSERT(joint);
    joint->setOwningSkeleton(m_handle);
    const JointInfo jointInfo(joint, parentJointIndex);
    skeletonData.joints.push_back(jointInfo);
    skeletonData.localPoses.push_back(joint->localPose());
    skeletonData.jointNames.push_back(joint->name());

    const int jointIndex = skeletonData.joints.size() - 1;
    const HJoint jointHandle = m_nodeManagers->jointManager()->lookupHandle(jointId);
    skeletonData.jointIndices.insert(jointHandle, jointIndex);

    // Recurse to the children
    const auto childIds = joint->childJointIds();
    for (const auto &childJointId : childIds)
        processJointHierarchy(childJointId, jointIndex, skeletonData);
}

void LoadSkeletonJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    if (!m_backendSkeleton)
        return;

    using namespace Qt3DCore;
    QAbstractSkeleton *node = qobject_cast<QAbstractSkeleton *>(manager->lookupNode(m_backendSkeleton->peerId()));
    if (!node)
        return;

    QAbstractSkeletonPrivate *dnode = QAbstractSkeletonPrivate::get(node);
    dnode->m_jointCount = m_backendSkeleton->jointCount();
    dnode->m_jointNames = m_backendSkeleton->jointNames();
    dnode->m_localPoses = m_backendSkeleton->localPoses();
    dnode->update();

    QSkeletonLoader *loaderNode = qobject_cast<QSkeletonLoader *>(node);
    if (loaderNode) {
        QSkeletonLoaderPrivate *dloaderNode = static_cast<QSkeletonLoaderPrivate *>(QSkeletonLoaderPrivate::get(loaderNode));
        dloaderNode->setStatus(m_backendSkeleton->status());

        if (m_loadedRootJoint) {
            dloaderNode->setRootJoint(m_loadedRootJoint);
            m_loadedRootJoint = nullptr;
        }
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
