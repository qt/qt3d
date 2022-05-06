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

#ifndef QT3DRENDER_RENDER_RENDERSETTINGS_H
#define QT3DRENDER_RENDER_RENDERSETTINGS_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qrendersettings.h>
#include <Qt3DRender/qpickingsettings.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class AbstractRenderer;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderSettings : public BackendNode
{
public:
    RenderSettings();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    Qt3DCore::QNodeId activeFrameGraphID() const { return m_activeFrameGraph; }
    QRenderSettings::RenderPolicy renderPolicy() const { return m_renderPolicy; }
    QPickingSettings::PickMethod pickMethod() const { return m_pickMethod; }
    QPickingSettings::PickResultMode pickResultMode() const { return m_pickResultMode; }
    QPickingSettings::FaceOrientationPickingMode faceOrientationPickingMode() const { return m_faceOrientationPickingMode; }
    float pickWorldSpaceTolerance() const { return m_pickWorldSpaceTolerance; }
    QString capabilities() const { return m_capabilities; }

    // For unit test purposes
    void setActiveFrameGraphId(Qt3DCore::QNodeId frameGraphNodeId) { m_activeFrameGraph = frameGraphNodeId; }

private:
    QRenderSettings::RenderPolicy m_renderPolicy;
    QPickingSettings::PickMethod m_pickMethod;
    QPickingSettings::PickResultMode m_pickResultMode;
    QPickingSettings::FaceOrientationPickingMode m_faceOrientationPickingMode;
    float m_pickWorldSpaceTolerance;
    Qt3DCore::QNodeId m_activeFrameGraph;
    QString m_capabilities;
};

class RenderSettingsFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit RenderSettingsFunctor(AbstractRenderer *renderer);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    AbstractRenderer *m_renderer;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERSETTINGS_H
