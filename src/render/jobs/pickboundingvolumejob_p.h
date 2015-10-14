/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H
#define QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H

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

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qboundingvolumeprovider.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <QSharedPointer>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QNodeId;
}

namespace Qt3DRender {

namespace Render {

class Entity;
class Renderer;

class PickBoundingVolumeJob : public Qt3DCore::QAspectJob, public Qt3DCore::QBoundingVolumeProvider
{
public:
    PickBoundingVolumeJob(Renderer *renderer);

    void setRoot(Entity *root);
    void setMouseEvents(const QList<QMouseEvent> &mouseEvents);
    QVector<Qt3DCore::QBoundingVolume *> boundingVolumes() const Q_DECL_FINAL;

protected:
    void run() Q_DECL_FINAL;

private:
    Renderer *m_renderer;
    Entity *m_node;
    QList<QMouseEvent> m_mouseEvents;

    QVector<Qt3DCore::QBoundingVolume *> gatherBoundingVolumes(Entity *entity) const;
    void viewMatrixForCamera(const Qt3DCore::QNodeId &cameraId,
                             QMatrix4x4 &viewMatrix,
                             QMatrix4x4 &projectionMatrix) const;
    QRect windowViewport(const QRectF &relativeViewport);
    HObjectPicker m_currentPicker;
};

typedef QSharedPointer<PickBoundingVolumeJob> PickBoundingVolumeJobPtr;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEJOB_H
