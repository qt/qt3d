/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QT3DRENDER_QPICKINGPROXY_P_H
#define QT3DRENDER_QPICKINGPROXY_P_H

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

#include <Qt3DCore/private/qgeometryfactory_p.h>
#include <Qt3DCore/private/qboundingvolume_p.h>
#include <Qt3DRender/qpickingproxy.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <memory>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QPickingProxyPrivate : public Qt3DCore::QBoundingVolumePrivate
{
public:
    QPickingProxyPrivate();
    ~QPickingProxyPrivate();

    Q_DECLARE_PUBLIC(QPickingProxy)

    void setView(Qt3DCore::QGeometryView *view) override;
};

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_QPICKINGPROXY_P_H

