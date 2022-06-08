// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DRENDERPASS_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DRENDERPASS_P_H

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

#include <Qt3DRender/qrenderpass.h>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DRenderPass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QFilterKey> filterKeys READ filterKeyList)
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QRenderState> renderStates READ renderStateList)
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QParameter> parameters READ parameterList)
public:
    explicit Quick3DRenderPass(QObject *parent = 0);

    QQmlListProperty<QFilterKey> filterKeyList();
    QQmlListProperty<QRenderState> renderStateList();
    QQmlListProperty<QParameter> parameterList();

    inline QRenderPass *parentRenderPass() const { return qobject_cast<QRenderPass *>(parent()); }
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DRENDERPASS_P_H
