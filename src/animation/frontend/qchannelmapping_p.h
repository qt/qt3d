// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QCHANNELMAPPING_P_H
#define QT3DANIMATION_QCHANNELMAPPING_P_H

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

#include <Qt3DAnimation/private/qabstractchannelmapping_p.h>
#include <Qt3DAnimation/qanimationcallback.h>
#include <QList>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class Q_AUTOTEST_EXPORT QChannelMappingPrivate : public QAbstractChannelMappingPrivate
{
public:
    QChannelMappingPrivate();

    Q_DECLARE_PUBLIC(QChannelMapping)

    void updatePropertyNameTypeAndComponentCount();

    QString m_channelName;
    Qt3DCore::QNode *m_target;
    QString m_property;
    const char *m_propertyName;
    int m_type;
    int m_componentCount;
};

struct QChannelMappingData
{
    QString channelName;
    Qt3DCore::QNodeId targetId;
    int type;
    int componentCount;
    const char *propertyName;
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

// Used to define the meta type id
Q_DECLARE_METATYPE(QList<float>) // LCOV_EXCL_LINE

#endif // QT3DANIMATION_QCHANNELMAPPING_P_H
