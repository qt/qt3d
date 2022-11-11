// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

    for (auto targetData : std::as_const(m_record.targetChanges)) {
        Qt3DCore::QNode *node = manager->lookupNode(targetData.targetId);
        if (node)
            node->setProperty(targetData.propertyName, targetData.value);
    }

    for (auto skeletonData : std::as_const(m_record.skeletonChanges)) {
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

    for (const AnimationCallbackAndValue &callback: std::as_const(m_callbacks)) {
        if (callback.callback)
            callback.callback->valueChanged(callback.value);
    }

    m_record = {};
}

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
