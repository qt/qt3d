// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QCALLBACKMAPPING_P_H
#define QT3DANIMATION_QCALLBACKMAPPING_P_H

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
#include <Qt3DAnimation/qcallbackmapping.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QCallbackMappingPrivate : public QAbstractChannelMappingPrivate
{
public:
    QCallbackMappingPrivate();

    Q_DECLARE_PUBLIC(QCallbackMapping)

    QString m_channelName;
    int m_type;
    QAnimationCallback *m_callback;
    QAnimationCallback::Flags m_callbackFlags;
};

struct QCallbackMappingData
{
    QString channelName;
    int type;
    QAnimationCallback *callback;
    QAnimationCallback::Flags callbackFlags;
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_QCALLBACKMAPPING_P_H
