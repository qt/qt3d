/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QRENDERSURFACESELECTOR_P_H
#define QRENDERSURFACESELECTOR_P_H

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

#include <Qt3DRender/private/qframegraphnode_p.h>
#include <Qt3DRender/private/platformsurfacefilter_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <QtGui/qsurface.h>
#include <QtGui/qwindow.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderSurfaceSelector;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QRenderSurfaceSelectorPrivate : public Qt3DRender::QFrameGraphNodePrivate
{
public:
    QRenderSurfaceSelectorPrivate();
    ~QRenderSurfaceSelectorPrivate();

    // TODO: Qt 5.8, make it public
    static QRenderSurfaceSelector *find(QObject *rootObject);

    void setExternalRenderTargetSize(const QSize &size);
    QSize externalRenderTargetSize() const { return m_externalRenderTargetSize; }

    QSurface *m_surface;
    QSize m_externalRenderTargetSize;
    QScopedPointer<Qt3DRender::Render::PlatformSurfaceFilter> m_surfaceEventFilter;
    float m_surfacePixelRatio;
    QMetaObject::Connection m_heightConn;
    QMetaObject::Connection m_widthConn;
    QMetaObject::Connection m_screenConn;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QRENDERSURFACESELECTOR_P_H
