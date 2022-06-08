// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_CLIPBLENDVALUE_H
#define QT3DANIMATION_ANIMATION_CLIPBLENDVALUE_H

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

class Q_AUTOTEST_EXPORT ClipBlendValue : public ClipBlendNode
{
public:
    ClipBlendValue();
    ~ClipBlendValue();

    inline Qt3DCore::QNodeId clipId() const { return m_clipId; }
    void setClipId(Qt3DCore::QNodeId clipId) { m_clipId = clipId; } // For unit tests

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    inline QList<Qt3DCore::QNodeId> allDependencyIds() const override
    {
        return currentDependencyIds();
    }

    inline QList<Qt3DCore::QNodeId> currentDependencyIds() const override
    {
        return {};
    }

    double duration() const override;

    void setClipFormat(Qt3DCore::QNodeId animatorId, const ClipFormat &formatIndices);
    ClipFormat &clipFormat(Qt3DCore::QNodeId animatorId);
    const ClipFormat &clipFormat(Qt3DCore::QNodeId animatorId) const;

protected:
    ClipResults doBlend(const QList<ClipResults> &blendData) const override;

private:
    Qt3DCore::QNodeId m_clipId;

    QList<Qt3DCore::QNodeId> m_animatorIds;
    QList<ClipFormat> m_clipFormats;
};

} // namespace Animation
} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_CLIPBLENDVALUE_H
