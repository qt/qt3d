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
****************************************************************************/

#ifndef QT3DRENDER_QRENDERSETTINGS_H
#define QT3DRENDER_QRENDERSETTINGS_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qpickingsettings.h>
#include <QtGui/qtguiglobal.h>

QT_BEGIN_NAMESPACE
namespace Qt3DRender {

class QFrameGraphNode;
class QRenderCapabilities;
class QRenderSettingsPrivate;

class Q_3DRENDERSHARED_EXPORT QRenderSettings : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QRenderCapabilities* renderCapabilities READ renderCapabilities CONSTANT REVISION 15)
    Q_PROPERTY(Qt3DRender::QPickingSettings* pickingSettings READ pickingSettings CONSTANT)
    Q_PROPERTY(RenderPolicy renderPolicy READ renderPolicy WRITE setRenderPolicy NOTIFY renderPolicyChanged)
    Q_PROPERTY(Qt3DRender::QFrameGraphNode *activeFrameGraph READ activeFrameGraph WRITE setActiveFrameGraph NOTIFY activeFrameGraphChanged)
    Q_CLASSINFO("DefaultProperty", "activeFrameGraph")

public:
    explicit QRenderSettings(Qt3DCore::QNode *parent = nullptr);
    ~QRenderSettings();

    enum RenderPolicy {
        OnDemand,
        Always
    };
    Q_ENUM(RenderPolicy) // LCOV_EXCL_LINE

    QRenderCapabilities* renderCapabilities();
    QPickingSettings* pickingSettings();
    QFrameGraphNode *activeFrameGraph() const;
    RenderPolicy renderPolicy() const;

public Q_SLOTS:
    void setActiveFrameGraph(QFrameGraphNode *activeFrameGraph);
    void setRenderPolicy(RenderPolicy renderPolicy);

Q_SIGNALS:
    void activeFrameGraphChanged(QFrameGraphNode *activeFrameGraph);
    void renderPolicyChanged(RenderPolicy renderPolicy);

protected:
    Q_DECLARE_PRIVATE(QRenderSettings)
    explicit QRenderSettings(QRenderSettingsPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_onPickingMethodChanged(QPickingSettings::PickMethod))
    Q_PRIVATE_SLOT(d_func(), void _q_onPickResultModeChanged(QPickingSettings::PickResultMode))
    Q_PRIVATE_SLOT(d_func(), void _q_onFaceOrientationPickingModeChanged(QPickingSettings::FaceOrientationPickingMode))
    Q_PRIVATE_SLOT(d_func(), void _q_onWorldSpaceToleranceChanged(float))
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // namespace Qt3Drender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERSETTINGS_H
