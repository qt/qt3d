// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_VIEWPORTNODE_H
#define QT3DRENDER_RENDER_VIEWPORTNODE_H

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
#include <QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QViewport;

namespace Render {

class Renderer;

class Q_3DRENDERSHARED_PRIVATE_EXPORT ViewportNode : public FrameGraphNode
{
public:
    ViewportNode();

    float xMin() const;
    void setXMin(float xMin);

    float yMin() const;
    void setYMin(float yMin);

    float xMax() const;
    void setXMax(float xMax);

    float yMax() const;
    void setYMax(float yMax);

    float gamma() const;
    void setGamma(float gamma);

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    static QRectF computeViewport(const QRectF &childViewport, const ViewportNode *parentViewport);

private:
    float m_xMin;
    float m_yMin;
    float m_xMax;
    float m_yMax;
    float m_gamma;
};


} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_VIEWPORTNODE_H
