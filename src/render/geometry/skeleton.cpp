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

#include <Qt3DCore/qpropertyupdatedchange.h>

#include <QFile>
#include <QFileInfo>

#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/gltfskeletonloader_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/qurlhelper_p.h>
#include <Qt3DCore/private/qskeletoncreatedchange_p.h>
#include <Qt3DCore/private/qskeletonloader_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Skeleton::Skeleton()
    : BackendNode(Qt3DCore::QBackendNode::ReadWrite)
    , m_status(Qt3DCore::QSkeletonLoader::NotReady)
{
}

void Skeleton::cleanup()
{
    m_source.clear();
    m_status = Qt3DCore::QSkeletonLoader::NotReady;
    clearData();
    setEnabled(false);
}

void Skeleton::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    m_skeletonManager = m_renderer->nodeManagers()->skeletonManager();
    const auto skeletonCreatedChange = qSharedPointerCast<QSkeletonCreatedChangeBase>(change);
    switch (skeletonCreatedChange->type()) {
    case QSkeletonCreatedChangeBase::SkeletonLoader: {
        const auto loaderTypedChange = qSharedPointerCast<QSkeletonCreatedChange<QSkeletonLoaderData>>(change);
        const auto &data = loaderTypedChange->data;
        m_dataType = File;
        m_source = data.source;
        if (!m_source.isEmpty()) {
            markDirty(AbstractRenderer::SkeletonDataDirty);
            m_skeletonManager->addDirtySkeleton(SkeletonManager::SkeletonDataDirty, peerId());
        }
        break;
    }

    case QSkeletonCreatedChangeBase::Skeleton:
        // TODO: Handle QSkeleton case (non loaded)
        break;
    }
}

void Skeleton::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    switch (e->type()) {
    case Qt3DCore::PropertyUpdated: {
        const auto change = qSharedPointerCast<QPropertyUpdatedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("source")) {
            Q_ASSERT(m_dataType == File);
            const auto source = change->value().toUrl();
            if (source != m_source) {
                m_source = source;
                markDirty(AbstractRenderer::SkeletonDataDirty);
                m_skeletonManager->addDirtySkeleton(SkeletonManager::SkeletonDataDirty, peerId());
            }
        }

        // TODO: Handle QSkeleton case (non loaded)

        break;
    }

    default:
        break;
    }
    QBackendNode::sceneChangeEvent(e);
}

void Skeleton::setStatus(QSkeletonLoader::Status status)
{
    if (status != m_status) {
        m_status = status;
        Qt3DCore::QPropertyUpdatedChangePtr e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName("status");
        e->setValue(QVariant::fromValue(m_status));
        notifyObservers(e);
    }
}

void Skeleton::notifyJointCount()
{
    Qt3DCore::QPropertyUpdatedChangePtr e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
    e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
    e->setPropertyName("jointCount");
    e->setValue(jointCount());
    notifyObservers(e);
}

void Skeleton::loadSkeleton()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_source;
    clearData();

    // Load the data
    switch (m_dataType) {
    case File:
        loadSkeletonFromUrl();
        break;

    case Data:
        loadSkeletonFromData();
        break;

    default:
        Q_UNREACHABLE();
    }

    // If using a loader inform the frontend of the status change
    if (m_dataType == File) {
        if (jointCount() == 0)
            setStatus(QSkeletonLoader::Error);
        else
            setStatus(QSkeletonLoader::Ready);
    }
    notifyJointCount();

    qCDebug(Jobs) << "Loaded skeleton data:" << *this;
}

void Skeleton::loadSkeletonFromUrl()
{
    // TODO: Handle remote files
    QString filePath = Qt3DRender::QUrlHelper::urlToLocalFileOrQrc(m_source);
    QFileInfo info(filePath);
    if (!info.exists()) {
        qWarning() << "Could not open skeleton file:" << filePath;
        setStatus(Qt3DCore::QSkeletonLoader::Error);
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open skeleton file:" << filePath;
        setStatus(Qt3DCore::QSkeletonLoader::Error);
        return;
    }

    // TODO: Make plugin based for more file type support. For now gltf or native
    const QString ext = info.suffix();
    if (ext == QLatin1String("gltf")) {
        GLTFSkeletonLoader loader;
        loader.load(&file);
        m_skeletonData = loader.createSkeleton(m_name);
    } else if (ext == QLatin1String("json")) {
        // TODO: Support native skeleton type
    } else {
        qWarning() << "Unknown skeleton file type:" << ext;
        setStatus(Qt3DCore::QSkeletonLoader::Error);
        return;
    }
}

void Skeleton::loadSkeletonFromData()
{

}

void Skeleton::clearData()
{
    m_name.clear();
    m_skeletonData.joints.clear();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
