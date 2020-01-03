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

#include "buffer_p.h"
#include <graphicscontext_p.h>
#include <QtGui/private/qrhi_p.h>
QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

// A UBO is created for each ShaderData Shader Pair
// That means a UBO is unique to a shader/shaderdata

RHIBuffer::RHIBuffer()
    : m_bufferId(0)
    , m_dynamic(true)
    , m_lastTarget(GL_ARRAY_BUFFER)
{
}

bool RHIBuffer::bind(GraphicsContext *ctx, Type t)
{
    assert(ctx->m_currentUpdates);
    if(this->m_datasToUpload.empty())
        return bool(m_rhiBuffer);

    if(!m_rhiBuffer)
    {
        const auto kind = m_dynamic ? QRhiBuffer::Dynamic : QRhiBuffer::Static;
        const auto usage = [&] {
            switch(t) {
            case Type::ArrayBuffer: return QRhiBuffer::VertexBuffer;
            case Type::IndexBuffer: return QRhiBuffer::IndexBuffer;
            case Type::UniformBuffer: return QRhiBuffer::UniformBuffer;
            default:
                RHI_UNIMPLEMENTED;
                return QRhiBuffer::StorageBuffer;
            }
        }();

        if(m_allocSize <= 0)
            return false;

        if(m_rhiBuffer)
        {
            // RHI does not seem to support using the same buffer with different types
            assert(m_rhiBuffer->usage() == usage);
        }

        if(!m_rhiBuffer)
        {
            m_rhiBuffer = ctx->m_rhi->newBuffer(kind, usage, m_allocSize);
        }
        assert(m_rhiBuffer);

        m_rhiBuffer->build();
        {
            // debug: set to zero
            char* ptr = (char*)alloca(m_allocSize);
            std::fill_n(ptr, m_allocSize, 0);
            if(m_dynamic)
            {
                ctx->m_currentUpdates->updateDynamicBuffer(m_rhiBuffer, 0, m_allocSize, ptr);
            }
            else
            {
                ctx->m_currentUpdates->uploadStaticBuffer(m_rhiBuffer, 0, m_allocSize, ptr);
            }
        }
    }

    if(m_dynamic)
    {
        for(const auto& [data, offset] : this->m_datasToUpload)
        {
            ctx->m_currentUpdates->updateDynamicBuffer(m_rhiBuffer, offset, data.size(), data.constData());
        }
    }
    else
    {
        for(const auto& [data, offset] : this->m_datasToUpload)
        {
            ctx->m_currentUpdates->uploadStaticBuffer(m_rhiBuffer, offset, data.size(), data.constData());
        }
    }
    this->m_datasToUpload.clear();
    return true;
}

bool RHIBuffer::release(GraphicsContext *ctx)
{
    if(m_rhiBuffer)
        m_rhiBuffer->release();
    return true;
}

bool RHIBuffer::create(GraphicsContext *ctx)
{
    return true;
}

void RHIBuffer::destroy(GraphicsContext *ctx)
{
    if(m_rhiBuffer)
    {
        m_rhiBuffer->releaseAndDestroyLater();
        m_rhiBuffer = nullptr;
    }
    m_allocSize = 0;
}

void RHIBuffer::orphan(GraphicsContext *)
{
    m_datasToUpload.clear();
    if(m_rhiBuffer)
    {
        m_rhiBuffer->releaseAndDestroyLater();
        m_rhiBuffer = nullptr;
    }
    m_allocSize = 0;
}

void RHIBuffer::allocate(GraphicsContext *ctx, const QByteArray& data, bool dynamic)
{
    m_datasToUpload.clear();
    m_datasToUpload.push_back({data, 0});
    m_allocSize = data.size();
    m_dynamic = dynamic;
}

void RHIBuffer::update(GraphicsContext *ctx, const QByteArray& data, int offset)
{
    m_datasToUpload.push_back({data, offset});
}

QByteArray RHIBuffer::download(GraphicsContext *ctx, uint size)
{
    RHI_UNIMPLEMENTED;
    return {};
//*     char *gpu_ptr = ctx->mapBuffer(m_lastTarget, size);
//*     QByteArray data;
//*     if (gpu_ptr != nullptr) {
//*         data.resize(size);
//*         std::copy(gpu_ptr, gpu_ptr+size, data.data());
//*     }
//*     ctx->unmapBuffer(m_lastTarget);
//*     return data;
}

void RHIBuffer::bindBufferBase(GraphicsContext *ctx, int bindingPoint, RHIBuffer::Type t)
{
    RHI_UNIMPLEMENTED;
//*     ctx->bindBufferBase(glBufferTypes[t], bindingPoint, m_bufferId);
}

void RHIBuffer::bindBufferBase(GraphicsContext *ctx, int bindingPoint)
{
    RHI_UNIMPLEMENTED;
//*     ctx->bindBufferBase(m_lastTarget, bindingPoint, m_bufferId);
}

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
