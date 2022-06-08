// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DVIEWPORT_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DVIEWPORT_P_H

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

#include <Qt3DRender/qviewport.h>

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>
#include <Qt3DQuick/private/quick3dnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

// TO DO : Check if this is required or if this might as well be removed

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DViewport : public QObject
{
    Q_OBJECT
public:
    explicit Quick3DViewport(QObject *parent = 0);
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DVIEWPORT_P_H

