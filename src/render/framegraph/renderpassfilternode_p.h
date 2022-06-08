// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERPASSFILTER_H
#define QT3DRENDER_RENDER_RENDERPASSFILTER_H

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
#include <Qt3DRender/private/parameterpack_p.h>
#include <QList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFilterKey;
class QRenderPassFilter;

template <typename T>
class QHandle;

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderPassFilter : public Render::FrameGraphNode
{
public:
    RenderPassFilter();

    QList<Qt3DCore::QNodeId> filters() const;
    QList<Qt3DCore::QNodeId> parameters() const;
    void appendFilter(Qt3DCore::QNodeId criterionId);
    void removeFilter(Qt3DCore::QNodeId criterionId);
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QList<Qt3DCore::QNodeId> m_filters;
    ParameterPack m_parameterPack;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERPASSFILTER_H
