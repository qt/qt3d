// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
        ArrayBuffer         = 1 << 0,
        UniformBuffer       = 1 << 1,
        IndexBuffer         = 1 << 2,
        ShaderStorageBuffer = 1 << 3,
        PixelPackBuffer     = 1 << 4,
        PixelUnpackBuffer   = 1 << 5,
        DrawIndirectBuffer  = 1 << 6
    };

    bool bind(SubmissionContext *ctx, Type t);
    void destroy();
    void destroyOrphaned();
    void allocate(const QByteArray &data, bool dynamic = true);
    void update(const QByteArray &data, int offset = 0);
    QByteArray download(SubmissionContext *ctx, uint size);
    void cleanup();

    qsizetype size() const { return m_allocSize; }
    QRhiBuffer *rhiBuffer() const noexcept { return m_rhiBuffer; }

private:
    void orphan();

    uint m_bufferId;
    bool m_dynamic;
    qsizetype m_allocSize {};

    QRhiBuffer *m_rhiBuffer {};

    std::vector<QRhiBuffer *> m_buffersToCleanup;

    std::vector<std::pair<QByteArray /*data*/, int /*offset*/>> m_datasToUpload;
};

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RHIBUFFER_P_H
