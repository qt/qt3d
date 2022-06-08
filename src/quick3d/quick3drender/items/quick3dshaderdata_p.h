// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DSHADERDATA_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DSHADERDATA_P_H

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

#include <QtQml/QJSValue>
#include <QtQml/QJSValueIterator>

#include <Qt3DRender/qshaderdata.h>
#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DShaderData : public QShaderData
{
    Q_OBJECT
public:
    explicit Quick3DShaderData(QNode *parent = 0);
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::Render::Quick::Quick3DShaderData*) // LCOV_EXCL_LINE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DSHADERDATA_P_H
