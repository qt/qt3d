// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QANIMATIONCLIPLOADER_P_H
#define QT3DANIMATION_QANIMATIONCLIPLOADER_P_H

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

#include <Qt3DAnimation/private/qabstractanimationclip_p.h>
#include "qanimationcliploader.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationClipLoaderPrivate : public QAbstractAnimationClipPrivate
{
public:
    QAnimationClipLoaderPrivate();

    void setStatus(QAnimationClipLoader::Status status);

    Q_DECLARE_PUBLIC(QAnimationClipLoader)

    QUrl m_source;
    QAnimationClipLoader::Status m_status;
};

struct QAnimationClipLoaderData
{
    QUrl source;
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCLIPLOADER_P_H
