// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_ADDITIVECLIPBLEND_P_H
#define QT3DANIMATION_ANIMATION_ADDITIVECLIPBLEND_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DAnimation/private/clipblendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

namespace Animation {

class Q_AUTOTEST_EXPORT AdditiveClipBlend : public ClipBlendNode
{
public:
    AdditiveClipBlend();
    ~AdditiveClipBlend();

    inline Qt3DCore::QNodeId baseClipId() const { return m_baseClipId; }
    void setBaseClipId(Qt3DCore::QNodeId baseClipId) { m_baseClipId = baseClipId; } // For unit tests

    inline Qt3DCore::QNodeId additiveClipId() const { return m_additiveClipId; }
    void setAdditiveClipId(Qt3DCore::QNodeId additiveClipId) { m_additiveClipId = additiveClipId; } // For unit tests

    inline float additiveFactor() const { return m_additiveFactor; }
    void setAdditiveFactor(float additiveFactor) { m_additiveFactor = additiveFactor; } // For unit tests

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) final;

    inline QList<Qt3DCore::QNodeId> allDependencyIds() const override
    {
        return currentDependencyIds();
    }

    inline QList<Qt3DCore::QNodeId> currentDependencyIds() const override
    {
        return { m_baseClipId, m_additiveClipId };
    }

    inline double duration() const override
    {
        ClipBlendNode *node = clipBlendNodeManager()->lookupNode(m_baseClipId);
        Q_ASSERT(node);
        return node->duration();
    }

protected:
    ClipResults doBlend(const QList<ClipResults> &blendData) const final;

private:
    Qt3DCore::QNodeId m_baseClipId;
    Qt3DCore::QNodeId m_additiveClipId;
    float m_additiveFactor;
};

} // Animation

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_ADDITIVECLIPBLEND_P_H
