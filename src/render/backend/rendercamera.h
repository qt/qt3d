/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_RENDERCAMERA_H
#define QT3D_RENDER_RENDERCAMERA_H

#include <qchangearbiter.h>

#include <QMatrix4x4>
#include <QRectF>

namespace Qt3D
{

class Camera;
class RendererAspect;

namespace Render {

class RenderCamera : public QObserverInterface
{
public:
    RenderCamera(RendererAspect *rendererAspect);

    void setPeer(Camera *peer);

    // manually sync state with the peer, for the moment
    void sync();

    QVector4D clearColor() const { return m_clearColor; }

    unsigned int clearMask() const;

    QRectF viewport() const { return m_viewport; }

    void setProjection(const QMatrix4x4 &projection);
    QMatrix4x4 projection() const;

    QMatrix4x4 view() const;

    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;

private:
    RendererAspect *m_rendererAspect;
    Camera *m_peer;

    // clip plane data
    // ? m_zNear, m_zFar;


    QVector4D m_clearColor;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QRectF m_viewport;
};

} // Render

} // Qt3D

#endif // QT3D_RENDER_RENDERCAMERA_H
