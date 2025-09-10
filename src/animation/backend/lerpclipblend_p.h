// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_LERPCLIPBLEND_P_H
#define QT3DANIMATION_ANIMATION_LERPCLIPBLEND_P_H

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

class Q_AUTOTEST_EXPORT LerpClipBlend : public ClipBlendNode
{
public:
    LerpClipBlend();
    ~LerpClipBlend();

    inline float blendFactor() const { return m_blendFactor; }
    void setBlendFactor(float blendFactor) { m_blendFactor = blendFactor; } // For unit tests

    inline Qt3DCore::QNodeId startClipId() const { return m_startClipId; }
    void setStartClipId(Qt3DCore::QNodeId startClipId) { m_startClipId = startClipId; }  // For unit tests

    inline Qt3DCore::QNodeId endClipId() const { return m_endClipId; }
    void setEndClipId(Qt3DCore::QNodeId endClipId) { m_endClipId = endClipId; }  // For unit tests

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) final;

    inline QList<Qt3DCore::QNodeId> allDependencyIds() const override
    {
        return currentDependencyIds();
    }

    inline QList<Qt3DCore::QNodeId> currentDependencyIds() const override
    {
        return { m_startClipId, m_endClipId };
    }

    double duration() const override;

protected:
    ClipResults doBlend(const QList<ClipResults> &blendData) const final;

private:
    Qt3DCore::QNodeId m_startClipId;
    Qt3DCore::QNodeId m_endClipId;
    float m_blendFactor;
};

} // Animation

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_LERPCLIPBLEND_P_H
