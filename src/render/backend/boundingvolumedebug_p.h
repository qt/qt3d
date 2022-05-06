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
