// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERSURFACENODE_H
#define QT3DRENDER_RENDER_RENDERSURFACENODE_H

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

#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3DRender {
namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderSurfaceSelector : public Qt3DRender::Render::FrameGraphNode
{
public:
    RenderSurfaceSelector();

    QSize renderTargetSize() const;
    void setRenderTargetSize(const QSize &size) { m_renderTargetSize = size; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    QSurface *surface() const { return m_surface; }
    inline int width() const noexcept { return m_width; }
    inline int height() const noexcept { return m_height; }
    inline float devicePixelRatio() const noexcept { return m_devicePixelRatio; }

private:
    QObject *m_surfaceObj;
    QSurface *m_surface;
    QSize m_renderTargetSize;
    int m_width;
    int m_height;
    float m_devicePixelRatio;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERSURFACENODE_H
