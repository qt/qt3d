/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rhibuffer_p.h"
#include <submissioncontext_p.h>
#include <QtGui/private/qrhi_p.h>
QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

namespace {
QRhiBuffer::UsageFlag bufferTypeToRhi(RHIBuffer::Type t)
{
    switch (t) {
    case RHIBuffer::Type::ArrayBuffer:
        return QRhiBuffer::VertexBuffer;
    case RHIBuffer::Type::IndexBuffer:
        return QRhiBuffer::IndexBuffer;
    case RHIBuffer::Type::UniformBuffer:
        return QRhiBuffer::UniformBuffer;
    default:
        RHI_UNIMPLEMENTED;
        return QRhiBuffer::StorageBuffer;
    }
}
}

// A UBO is created for each ShaderData Shader Pair
// That means a UBO is unique to a shader/shaderdata

RHIBuffer::RHIBuffer()
    : m_bufferId(Qt3DCore::QNodeId())
    , m_dynamic(true)
{ }

bool RHIBuffer::bind(SubmissionContext *ctx, Type t)
{
    assert(ctx->m_currentUpdates);
    if (this->m_datasToUpload.empty())
        return bool(m_rhiBuffer);

    const auto uploadMethod = m_dynamic ? &QRhiResourceUpdateBatch::updateDynamicBuffer
                                        : qOverload<QRhiBuffer *, int, int, const void *>(
                                                &QRhiResourceUpdateBatch::uploadStaticBuffer);
    if (!m_rhiBuffer) {
        if (m_allocSize <= 0)
            return false;

        const auto kind = m_dynamic ? QRhiBuffer::Dynamic : QRhiBuffer::Static;
        const auto usage = bufferTypeToRhi(t);

        m_rhiBuffer = ctx->rhi()->newBuffer(kind, usage, m_allocSize);
        if (!m_rhiBuffer->create())
            return false;
#if defined(QT_DEBUG)
        {
            // for debug: we set the buffer to zero
            auto ptr = new char[m_allocSize] {};
            (ctx->m_currentUpdates->*uploadMethod)(m_rhiBuffer, 0, m_allocSize, ptr);
            delete[] ptr;
        }
#endif
    }
    assert(m_rhiBuffer);

    for (const std::pair<QByteArray, int> &pair : this->m_datasToUpload) {
        const QByteArray &data = pair.first;
        int offset = pair.second;
        (ctx->m_currentUpdates->*uploadMethod)(m_rhiBuffer, offset, data.size(), data.constData());
    }

    m_datasToUpload.clear();
    return true;
}

void RHIBuffer::destroy()
{
    if (m_rhiBuffer) {
        m_rhiBuffer->destroy();
        delete m_rhiBuffer;
        m_rhiBuffer = nullptr;
    }
    m_allocSize = 0;
}

void RHIBuffer::orphan()
{
    m_datasToUpload.clear();
    destroy();
}

void RHIBuffer::allocate(const QByteArray &data, bool dynamic)
{
    // We orphan only if new size is larger than current size
    // Otherwise, we can just reuse current buffer
    if (data.size() > m_allocSize)
        orphan();

    m_datasToUpload.clear();
    m_datasToUpload.push_back({ data, 0 });
    m_allocSize = std::max(m_allocSize, data.size());
    m_dynamic = dynamic;
}

void RHIBuffer::update(const QByteArray &data, int offset)
{
    m_datasToUpload.push_back({ data, offset });
}

QByteArray RHIBuffer::download(SubmissionContext *ctx, uint size)
{
    Q_UNUSED(ctx);
    Q_UNUSED(size);
    RHI_UNIMPLEMENTED;
    //    char *gpu_ptr = ctx->mapBuffer(m_lastTarget, size);
    //    QByteArray data;
    //    if (gpu_ptr != nullptr) {
    //        data.resize(size);
    //        std::copy(gpu_ptr, gpu_ptr+size, data.data());
    //    }
    //    ctx->unmapBuffer(m_lastTarget);
    //    return data;
    return {};
}

void RHIBuffer::cleanup()
{
    m_bufferId = Qt3DCore::QNodeId();
    m_dynamic = true;
    destroy();
}

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
