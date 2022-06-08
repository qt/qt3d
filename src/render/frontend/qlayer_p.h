// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QLAYER_P_H
#define QT3DRENDER_QLAYER_P_H

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

#include <private/qcomponent_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QLayerPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QLayerPrivate();

    bool m_recursive;

    Q_DECLARE_PUBLIC(QLayer)
};

struct QLayerData {
    bool m_recursive;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QLAYER_P_H
