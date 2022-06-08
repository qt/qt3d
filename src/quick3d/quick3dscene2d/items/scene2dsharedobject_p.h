// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QUICK3DRENDER_SCENE2DSHAREDOBJECT_P_H
#define QT3DRENDER_QUICK3DRENDER_SCENE2DSHAREDOBJECT_P_H

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

#include <Qt3DQuickScene2D/qt3dquickscene2d_global.h>
#include <Qt3DQuickScene2D/qscene2d.h>

#include <QtQuick/QQuickWindow>
#include <QtQuick/QQuickRenderControl>
#include <QtGui/QOffscreenSurface>
#include <QtCore/QCoreApplication>
#include <QtCore/QWaitCondition>
#include <QtCore/QThread>

#include <private/qnode_p.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Quick {

class Scene2DManager;

class Q_3DQUICKSCENE2DSHARED_EXPORT Scene2DSharedObject
{
public:
    Scene2DSharedObject(Scene2DManager *manager);
    ~Scene2DSharedObject();

    QQuickRenderControl *m_renderControl;
    QQuickWindow *m_quickWindow;
    Scene2DManager *m_renderManager;
    QOffscreenSurface *m_surface;

    QThread *m_renderThread;
    QObject *m_renderObject;

    QWaitCondition m_cond;
    QMutex m_mutex;

    bool isInitialized() const;
    void setInitialized();

    void requestQuit();
    bool isQuit() const;

    void requestRender(bool sync);

    bool isSyncRequested() const;
    void clearSyncRequest();

    void wait();
    void wake();

    bool isPrepared() const;
    void setPrepared();

    void disallowRender();
    bool canRender() const;

    void cleanup();

private:

    bool m_disallowed;
    bool m_quit;
    bool m_requestSync;
    bool m_prepared;
    bool m_initialized;
};

typedef QSharedPointer<Scene2DSharedObject> Scene2DSharedObjectPtr;

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::Quick::Scene2DSharedObjectPtr)

#endif // QT3DRENDER_QUICK3DRENDER_SCENE2DSHAREDOBJECT_P_H
