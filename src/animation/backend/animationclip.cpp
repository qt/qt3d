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

#include "animationclip_p.h"
#include <Qt3DAnimation/qanimationclip.h>
#include <Qt3DAnimation/private/qanimationclip_p.h>
#include <Qt3DAnimation/private/animationlogging_p.h>
#include <Qt3DRender/private/qurlhelper_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qpropertyupdatedchangebase_p.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qfile.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

AnimationClip::AnimationClip()
    : BackendNode(ReadWrite)
    , m_source()
    , m_name()
    , m_objectName()
    , m_channelGroups()
    , m_duration(0.0f)
{
}

void AnimationClip::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QAnimationClipData>>(change);
    const auto &data = typedChange->data;
    m_source = data.source;
    if (!m_source.isEmpty())
        setDirty(Handler::AnimationClipDirty);
}

void AnimationClip::cleanup()
{
    setEnabled(false);
    m_handler = nullptr;
    m_source.clear();
    m_channelGroups.clear();
    m_duration = 0.0f;

    clearData();
}

void AnimationClip::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    switch (e->type()) {
    case Qt3DCore::PropertyUpdated: {
        const auto change = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("source")) {
            m_source = change->value().toUrl();
            setDirty(Handler::AnimationClipDirty);
        }
        break;
    }

    default:
        break;
    }
    QBackendNode::sceneChangeEvent(e);
}

/*!
    \internal
    Called by LoadAnimationClipJob on the threadpool
 */
void AnimationClip::loadAnimation()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_source;
    clearData();

    // TODO: Handle remote files
    QString filePath = Qt3DRender::QUrlHelper::urlToLocalFileOrQrc(m_source);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not find animation clip:" << filePath;
        return;
    }

    QByteArray animationData = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(animationData);
    QJsonObject rootObject = document.object();

    // TODO: Allow loading of a named animation from a file containing many
    QJsonArray animationsArray = rootObject[QLatin1String("animations")].toArray();
    qCDebug(Jobs) << "Found" << animationsArray.size() << "animations:";
    for (int i = 0; i < animationsArray.size(); ++i) {
        QJsonObject animation = animationsArray.at(i).toObject();
        qCDebug(Jobs) << "\tName:" << animation[QLatin1String("action")].toString()
                      << "Object:" << animation[QLatin1String("object")].toString();
    }

    // For now just load the first animation
    QJsonObject animation = animationsArray.at(0).toObject();
    m_name = animation[QLatin1String("action")].toString();
    m_objectName = animation[QLatin1String("object")].toString();
    QJsonArray groupsArray = animation[QLatin1String("groups")].toArray();
    const int groupCount = groupsArray.size();
    m_channelGroups.resize(groupCount);
    for (int i = 0; i < groupCount; ++i) {
        const QJsonObject group = groupsArray.at(i).toObject();
        m_channelGroups[i].read(group);
    }

    const float t = findDuration();
    setDuration(t);

    m_channelCount = findChannelCount();

    qCDebug(Jobs) << "Loaded animation data:" << *this;
}

void AnimationClip::setDuration(float duration)
{
    if (qFuzzyCompare(duration, m_duration))
        return;

    m_duration = duration;

    // Send a change to the frontend
    auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
    e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
    Qt3DCore::QPropertyUpdatedChangeBasePrivate::get(e.data())->m_isFinal = true;
    e->setPropertyName("duration");
    e->setValue(m_duration);
    notifyObservers(e);
}

/*!
    \internal

    Given the index of a channel group, \a channelGroupIndex, calculates
    the base index of the first channel in this group. For example, if
    there are two channel groups each with 3 channels and you request
    the channelBaseIndex(1), the return value will be 3. Indices 0-2 are
    for the first group, so the first channel of the second group occurs
    at index 3.
 */
int AnimationClip::channelBaseIndex(int channelGroupIndex) const
{
    int index = 0;
    for (int i = 0; i < channelGroupIndex; ++i)
        index += m_channelGroups[i].channels.size();
    return index;
}

void AnimationClip::clearData()
{
    m_name.clear();
    m_objectName.clear();
    m_channelGroups.clear();
}

float AnimationClip::findDuration()
{
    // Iterate over the contained fcurves and find the longest one
    double tMax = 0.0;
    for (const ChannelGroup &channelGroup : qAsConst(m_channelGroups)) {
        for (const Channel &channel : qAsConst(channelGroup.channels)) {
            const float t = channel.fcurve.endTime();
            if (t > tMax)
                tMax = t;
        }
    }
    return tMax;
}

int AnimationClip::findChannelCount()
{
    int channelCount = 0;
    for (const ChannelGroup &channelGroup : qAsConst(m_channelGroups))
        channelCount += channelGroup.channels.size();
    return channelCount;
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
