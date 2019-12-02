/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

#include "abstractevaluateclipanimatorjob_p.h"
#include <Qt3DCore/private/qaspectjob_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qskeleton_p.h>
#include <Qt3DAnimation/qabstractclipanimator.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class AbstractEvaluateClipAnimatorJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    AbstractEvaluateClipAnimatorJobPrivate() { }
    ~AbstractEvaluateClipAnimatorJobPrivate() override { }

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    AnimationRecord m_record;
    QVector<AnimationCallbackAndValue> m_callbacks;
};

AbstractEvaluateClipAnimatorJob::AbstractEvaluateClipAnimatorJob()
    : Qt3DCore::QAspectJob(*new AbstractEvaluateClipAnimatorJobPrivate)
{
}

void AbstractEvaluateClipAnimatorJob::setPostFrameData(const AnimationRecord &record, const QVector<AnimationCallbackAndValue> &callbacks)
{
    auto mainThreadCB = callbacks;
    mainThreadCB.erase(std::remove_if(mainThreadCB.begin(), mainThreadCB.end(), [](const AnimationCallbackAndValue &callback) {
                        if (callback.flags.testFlag(QAnimationCallback::OnThreadPool)) {
                            // call these now and remove them from the list
                            callback.callback->valueChanged(callback.value);
                            return true;
                        }
                        return false;
                    }), mainThreadCB.end());
    // Should now only have callbacks to be called on main thread

    Q_D(AbstractEvaluateClipAnimatorJob);
    d->m_record = record;
    d->m_callbacks = mainThreadCB;
}

void AbstractEvaluateClipAnimatorJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    if (m_record.animatorId.isNull())
        return;

    for (auto targetData : qAsConst(m_record.targetChanges)) {
        Qt3DCore::QNode *node = manager->lookupNode(targetData.targetId);
        if (node)
            node->setProperty(targetData.propertyName, targetData.value);
    }

    for (auto skeletonData : qAsConst(m_record.skeletonChanges)) {
        Qt3DCore::QAbstractSkeleton *node = qobject_cast<Qt3DCore::QAbstractSkeleton *>(manager->lookupNode(skeletonData.first));
        if (node) {
            auto d = Qt3DCore::QAbstractSkeletonPrivate::get(node);
            d->m_localPoses = skeletonData.second;
            d->update();
        }
    }

    QAbstractClipAnimator *animator = qobject_cast<QAbstractClipAnimator *>(manager->lookupNode(m_record.animatorId));
    if (animator) {
        if (isValidNormalizedTime(m_record.normalizedTime))
            animator->setNormalizedTime(m_record.normalizedTime);
        if (m_record.finalFrame)
            animator->setRunning(false);
    }

    for (const AnimationCallbackAndValue &callback: qAsConst(m_callbacks)) {
        if (callback.callback)
            callback.callback->valueChanged(callback.value);
    }

    m_record = {};
}

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
