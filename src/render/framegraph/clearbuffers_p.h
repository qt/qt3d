// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_CLEARBUFFERS_P_H
#define QT3DRENDER_RENDER_CLEARBUFFERS_P_H

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
#include <Qt3DRender/qclearbuffers.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class FrameGraphManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT ClearBuffers : public FrameGraphNode
{
public:
    ClearBuffers();

    QClearBuffers::BufferType type() const;
    float clearDepthValue() const;
    int clearStencilValue() const;
    Qt3DCore::QNodeId bufferId() const;

    // in the backend we store this as a QVector4D, as the clearBuffer
    // functions do support all float values (also those not clamped to [0,1])
    // (for non-clamped float or int buffer types)
    // we don't support this in the frontend yet, but let's keep our options for now
    // and avoid QColor->QVector4D conversion every frame.
    QVector4D clearColor() const;

    // some clear operations only accept clamped values
    QColor clearColorAsColor() const;

    bool clearsAllColorBuffers() const;
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QClearBuffers::BufferType m_type;
    QColor m_clearColorAsColor;
    QVector4D m_clearColor;
    float m_clearDepthValue;
    int m_clearStencilValue;
    Qt3DCore::QNodeId m_colorBufferId;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_CLEARBUFFERS_P_H
