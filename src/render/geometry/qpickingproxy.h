// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QPICKINGPROXY_H
#define QT3DRENDER_QPICKINGPROXY_H

#include <Qt3DCore/qboundingvolume.h>
#include <Qt3DCore/qgeometry.h>
#include <Qt3DCore/qgeometryview.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPickingProxyPrivate;

class Q_3DRENDERSHARED_EXPORT QPickingProxy : public Qt3DCore::QBoundingVolume
{
    Q_OBJECT
public:
    explicit QPickingProxy(Qt3DCore::QNode *parent = nullptr);
    ~QPickingProxy();

protected:
    explicit QPickingProxy(QPickingProxyPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QPickingProxy)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPICKINGPROXY_H
