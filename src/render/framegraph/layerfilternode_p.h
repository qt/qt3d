// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LAYERFILTERNODE_H
#define QT3DRENDER_RENDER_LAYERFILTERNODE_H

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
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QLayer>
#include <QStringList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Renderer;

class Q_3DRENDERSHARED_PRIVATE_EXPORT LayerFilterNode : public FrameGraphNode
{
public:
    LayerFilterNode();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    Qt3DCore::QNodeIdVector layerIds() const;
    void setLayerIds(const Qt3DCore::QNodeIdVector &list);

    QLayerFilter::FilterMode filterMode() const;

private:
    Qt3DCore::QNodeIdVector m_layerIds;
    QLayerFilter::FilterMode m_filterMode;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_LAYERFILTERNODE_H
