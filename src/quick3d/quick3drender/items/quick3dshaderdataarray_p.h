// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DSHADERDATAARRAY_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DSHADERDATAARRAY_P_H

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

#include <Qt3DCore/qnode.h>
#include <QtQml/QQmlListProperty>
#include <QtQml/qqmlregistration.h>

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>
#include <Qt3DQuickRender/private/quick3dshaderdata_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QShaderData;

namespace Render {
namespace Quick {

class Quick3DShaderDataArrayPrivate;
class Quick3DShaderData;

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DShaderDataArray : public Qt3DCore::QNode
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ShaderDataArray)
    QML_ADDED_IN_VERSION(2, 0);

    Q_PROPERTY(QQmlListProperty<Qt3DRender::QShaderData> values READ valuesList)
    Q_CLASSINFO("DefaultProperty", "values")

public:
    explicit Quick3DShaderDataArray(Qt3DCore::QNode *parent = nullptr);
    QQmlListProperty<QShaderData> valuesList();
    QList<QShaderData *> values() const;

private:
    Q_DECLARE_PRIVATE(Quick3DShaderDataArray)
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::Render::Quick::Quick3DShaderDataArray*) // LCOV_EXCL_LINE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DSHADERDATAARRAY_P_H
