// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_CHANNELMAPPER_P_H
#define QT3DANIMATION_ANIMATION_CHANNELMAPPER_P_H

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

#include <Qt3DAnimation/private/backendnode_p.h>
#include <Qt3DAnimation/private/fcurve_p.h>

#include <Qt3DCore/qnodeid.h>

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Handler;

class Q_AUTOTEST_EXPORT ChannelMapper : public BackendNode
{
public:
    ChannelMapper();

    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    void setMappingIds(const Qt3DCore::QNodeIdVector &mappingIds) { m_mappingIds = mappingIds; }
    Qt3DCore::QNodeIdVector mappingIds() const { return m_mappingIds; }

    QVector<ChannelMapping*> mappings() const
    {
        if (m_isDirty)
            updateMappings();
        return m_mappings;
    }

private:
    void updateMappings() const;

    Qt3DCore::QNodeIdVector m_mappingIds;

    // Cached data
    mutable QVector<ChannelMapping*> m_mappings;
    mutable bool m_isDirty;
};

} // namespace Animation
} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_CHANNELMAPPER_P_H
