// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_PLATFORMSURFACEFILTER_H
#define QT3DRENDER_RENDER_PLATFORMSURFACEFILTER_H

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

#include <private/qt3drender_global_p.h>

#include <QtCore/qmutex.h>
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtGui/qsurface.h>

QT_BEGIN_NAMESPACE

class QOffscreenSurface;
class QWindow;

namespace Qt3DRender {
namespace Render {

class AbstractRenderer;

class Q_3DRENDERSHARED_PRIVATE_EXPORT PlatformSurfaceFilter : public QObject
{
    Q_OBJECT

public:
    explicit PlatformSurfaceFilter(QObject *parent = 0);
    ~PlatformSurfaceFilter();

    bool eventFilter(QObject *obj, QEvent *e) override;

    static void lockSurface();
    static void releaseSurface();
    static bool isSurfaceValid(QSurface *surface);

    template<class T>
    void setSurface(T *surface)
    {
        if (m_obj == surface)
            return;

        if (m_obj)
            m_obj->removeEventFilter(this);

        // Surface is offset from QWindow/QOffscreenSurface due to multiple inheritance
        m_surface = static_cast<QSurface *>(surface);
        m_obj = surface;

        if (m_obj) {
            m_obj->installEventFilter(this);
            markSurfaceAsValid();
        }
    }
private:
    QPointer<QObject> m_obj;
    QSurface *m_surface;

    static QBasicMutex m_surfacesMutex;
    static QHash<QSurface *, bool> m_surfacesValidity;
    void markSurfaceAsValid();
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT SurfaceLocker
{
public:
    explicit SurfaceLocker(QSurface *surface);
    ~SurfaceLocker();
    bool isSurfaceValid() const;

private:
    QSurface *m_surface;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PLATFORMSURFACEFILTER_H
