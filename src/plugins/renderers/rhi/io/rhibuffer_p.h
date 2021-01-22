/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_RHI_RHIBUFFER_P_H
#define QT3DRENDER_RENDER_RHI_RHIBUFFER_P_H

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

#include <Qt3DCore/qnodeid.h>
#include <qbytearray.h>

QT_BEGIN_NAMESPACE
class QRhiBuffer;
namespace Qt3DRender {

namespace Render {

namespace Rhi {

class SubmissionContext;

class RHIBuffer
{
public:
    RHIBuffer();

    enum Type {
        ArrayBuffer = 0,
        UniformBuffer,
        IndexBuffer,
        ShaderStorageBuffer,
        PixelPackBuffer,
        PixelUnpackBuffer,
        DrawIndirectBuffer
    };

    bool bind(SubmissionContext *ctx, Type t);
    bool release(SubmissionContext *ctx);
    bool create(SubmissionContext *ctx);
    void destroy(SubmissionContext *ctx);
    void orphan(SubmissionContext *ctx);
    void allocate(SubmissionContext *ctx, const QByteArray &data, bool dynamic = true);
    void update(SubmissionContext *ctx, const QByteArray &data, int offset = 0);
    QByteArray download(SubmissionContext *ctx, uint size);
    void bindBufferBase(SubmissionContext *ctx, int bindingPoint, Type t);
    void bindBufferBase(SubmissionContext *ctx, int bindingPoint);

    void cleanup();

    QRhiBuffer *rhiBuffer() const noexcept { return m_rhiBuffer; }

private:
    uint m_bufferId;
    bool m_dynamic;
    int m_allocSize {};
    int m_lastTarget;

    QRhiBuffer *m_rhiBuffer {};

    std::vector<std::pair<QByteArray /*data*/, int /*offset*/>> m_datasToUpload;
};

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RHIBUFFER_P_H
