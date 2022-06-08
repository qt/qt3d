// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QABSTRACTCHANNELMAPPING_P_H
#define QT3DANIMATION_QABSTRACTCHANNELMAPPING_P_H

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

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractChannelMapping;

class Q_AUTOTEST_EXPORT QAbstractChannelMappingPrivate : public Qt3DCore::QNodePrivate
{
public:
    QAbstractChannelMappingPrivate();

    static const QAbstractChannelMappingPrivate *get(const Qt3DAnimation::QAbstractChannelMapping *q);

    Q_DECLARE_PUBLIC(QAbstractChannelMapping)

    enum MappingType {
        ChannelMapping = 0,
        SkeletonMapping,
        CallbackMapping
    };

    MappingType m_mappingType;
};

struct QAbstractChannelMappingData
{
    // TODO: Add members that should be sent to the backend
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_QABSTRACTCHANNELMAPPING_P_H
