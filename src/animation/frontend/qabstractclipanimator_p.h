// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QABSTRACTCLIPANIMATOR_P_H
#define QT3DANIMATION_QABSTRACTCLIPANIMATOR_P_H

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

#include <Qt3DCore/private/qcomponent_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QChannelMapper;
class QClock;

class QAbstractClipAnimatorPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QAbstractClipAnimatorPrivate();

    virtual bool canPlay() const;

    Q_DECLARE_PUBLIC(QAbstractClipAnimator)

    Qt3DAnimation::QChannelMapper *m_mapper;
    Qt3DAnimation::QClock *m_clock;
    bool m_running;
    int m_loops;
    float m_normalizedTime;
};

struct QAbstractClipAnimatorData
{
    Qt3DCore::QNodeId mapperId;
    Qt3DCore::QNodeId clockId;
    bool running;
    float normalizedTime;
    int loops;
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_QABSTRACTCLIPANIMATOR_P_H
