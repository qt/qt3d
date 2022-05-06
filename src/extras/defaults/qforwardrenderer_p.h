/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DEXTRAS_QFORWARDRENDERER_P_H
#define QT3DEXTRAS_QFORWARDRENDERER_P_H

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

#include <Qt3DRender/private/qtechniquefilter_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QForwardRenderer;
class QViewport;
class QClearBuffers;
class QCameraSelector;
class QFrustumCulling;
class QRenderSurfaceSelector;
class QDebugOverlay;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QForwardRendererPrivate: public Qt3DRender::QTechniqueFilterPrivate
{
public:
    QForwardRendererPrivate();

    Qt3DRender::QRenderSurfaceSelector *m_surfaceSelector;
    Qt3DRender::QViewport *m_viewport;
    Qt3DRender::QCameraSelector *m_cameraSelector;
    Qt3DRender::QClearBuffers *m_clearBuffer;
    Qt3DRender::QFrustumCulling *m_frustumCulling;
    Qt3DRender::QDebugOverlay *m_debugOverlay;

    void init();

    Q_DECLARE_PUBLIC(QForwardRenderer)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QFORWARDRENDERER_P_H

