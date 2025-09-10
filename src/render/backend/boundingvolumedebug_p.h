// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_BOUNDINGVOLUMEDEBUG_H
#define QT3DRENDER_RENDER_BOUNDINGVOLUMEDEBUG_H

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

#if 0

#include <Qt3DRender/private/backendnode_p.h>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_AUTOTEST_EXPORT BoundingVolumeDebug : public BackendNode
{
public:
    BoundingVolumeDebug();
    ~BoundingVolumeDebug();

    void cleanup();
    void updateFromPeer(Qt3DCore::QNode *peer) final;
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) final;

    inline bool isRecursive() const { return m_recursive; }
    inline float radius() const { return m_radius; }
    inline QVector3D center() const { return m_center; }

    void setRadius(float radius);
    void setCenter(const QVector3D &center);

private:
    bool m_recursive;
    float m_radius;
    QVector3D m_center;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif

#endif // QT3DRENDER_RENDER_BOUNDINGVOLUMEDEBUG_H
