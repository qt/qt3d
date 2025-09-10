// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OFFSCREENSURFACEHELPER_H
#define QT3DRENDER_RENDER_OFFSCREENSURFACEHELPER_H

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

#include <QObject>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

class QOffscreenSurface;

namespace Qt3DRender {
namespace Render {

class AbstractRenderer;

class Q_3DRENDERSHARED_PRIVATE_EXPORT OffscreenSurfaceHelper : public QObject
{
    Q_OBJECT
public:
    OffscreenSurfaceHelper(AbstractRenderer *renderer,
                           QObject *parent = nullptr);
    inline QOffscreenSurface *offscreenSurface() const { return m_offscreenSurface; }

public slots:
    void createOffscreenSurface();

private:
    Render::AbstractRenderer *m_renderer;
    QOffscreenSurface *m_offscreenSurface;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OFFSCREENSURFACEHELPER_H
