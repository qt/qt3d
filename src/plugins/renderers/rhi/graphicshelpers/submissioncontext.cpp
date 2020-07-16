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

#include "submissioncontext_p.h"

#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qfrontface.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/qstenciloperationarguments.h>
#include <Qt3DRender/qstenciltestarguments.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DRender/private/material_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/renderstates_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <Qt3DRender/private/rendertarget_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/attachmentpack_p.h>
#include <Qt3DRender/private/qbuffer_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DRender/private/vulkaninstance_p.h>
#include <QGuiApplication>
#include <texture_p.h>
#include <rendercommand_p.h>
#include <renderer_p.h>
#include <rhiresourcemanagers_p.h>
#include <renderbuffer_p.h>
#include <rhishader_p.h>
#include <QOpenGLShaderProgram>

#include <private/qdebug_p.h>
#include <QSurface>
#include <QWindow>
#include <QShaderBaker>

#ifdef Q_OS_WIN
#include <QtGui/private/qrhid3d11_p.h>
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
#include <QtGui/private/qrhimetal_p.h>
#endif

#ifndef QT_NO_OPENGL
#include <QtGui/private/qrhigles2_p.h>
#endif

#if QT_CONFIG(vulkan)
#include <QtGui/private/qrhivulkan_p.h>
#endif
#include <bitset>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Rhi {

static QHash<unsigned int, SubmissionContext *> static_contexts;

unsigned int nextFreeContextId() noexcept
{
    for (unsigned int i = 0; i < 0xffff; ++i) {
        if (!static_contexts.contains(i))
            return i;
    }

    qFatal("Couldn't find free context ID");
    return 0;
}

namespace {

RHIBuffer::Type attributeTypeToGLBufferType(QAttribute::AttributeType type) noexcept
{
    switch (type) {
    case QAttribute::VertexAttribute:
        return RHIBuffer::ArrayBuffer;
    case QAttribute::IndexAttribute:
        return RHIBuffer::IndexBuffer;
    case QAttribute::DrawIndirectAttribute:
        return RHIBuffer::DrawIndirectBuffer;
    default:
        Q_UNREACHABLE();
    }
}

void copyGLFramebufferDataToImage(QImage &img, const uchar *srcData, uint stride, uint width,
                                  uint height, QAbstractTexture::TextureFormat format) noexcept
{
    switch (format) {
    case QAbstractTexture::RGBA32F: {
        uchar *srcScanline = const_cast<uchar *>(srcData) + stride * (height - 1);
        for (uint i = 0; i < height; ++i) {
            uchar *dstScanline = img.scanLine(i);
            float *pSrc = reinterpret_cast<float *>(srcScanline);
            for (uint j = 0; j < width; j++) {
                *dstScanline++ = (uchar)(255.0f * qBound(0.0f, pSrc[4 * j + 2], 1.0f));
                *dstScanline++ = (uchar)(255.0f * qBound(0.0f, pSrc[4 * j + 1], 1.0f));
                *dstScanline++ = (uchar)(255.0f * qBound(0.0f, pSrc[4 * j + 0], 1.0f));
                *dstScanline++ = (uchar)(255.0f * qBound(0.0f, pSrc[4 * j + 3], 1.0f));
            }
            srcScanline -= stride;
        }
    } break;
    default: {
        uchar *srcScanline = (uchar *)srcData + stride * (height - 1);
        for (uint i = 0; i < height; ++i) {
            memcpy(img.scanLine(i), srcScanline, stride);
            srcScanline -= stride;
        }
    } break;
    }
}

// Render States Helpers

template<typename GenericState>
void applyStateHelper(const GenericState *state, QRhiGraphicsPipeline *gp) noexcept
{
    Q_UNUSED(state);
    Q_UNUSED(gp);
    qWarning() << "RHI Unhandled render state" << typeid(GenericState).name();
}

void applyStateHelper(const BlendEquationArguments *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();

    // We assume a single color attachment
    QRhiGraphicsPipeline::TargetBlend targetBlend {};

    const bool hasTargetBlend = gp->cbeginTargetBlends() != gp->cendTargetBlends();
    if (hasTargetBlend)
        targetBlend = *(gp->cbeginTargetBlends());

    auto getRHIBlendFactor = [](int arg) {
        switch (arg) {
        case QBlendEquationArguments::Zero:
            return QRhiGraphicsPipeline::Zero;
        case QBlendEquationArguments::One:
            return QRhiGraphicsPipeline::One;
        case QBlendEquationArguments::SourceColor:
            return QRhiGraphicsPipeline::SrcColor;
        case QBlendEquationArguments::SourceAlpha:
            return QRhiGraphicsPipeline::SrcAlpha;
            // ### Qt 6 Fix values
            //        case QBlendEquationArguments::Source1Alpha:
            //            return QRhiGraphicsPipeline::Src1Alpha;
            //        case QBlendEquationArguments::Source1Color:
            //            return QRhiGraphicsPipeline::Src1Color;
        case QBlendEquationArguments::DestinationColor:
            return QRhiGraphicsPipeline::DstColor;
        case QBlendEquationArguments::DestinationAlpha:
            return QRhiGraphicsPipeline::DstAlpha;
        case QBlendEquationArguments::SourceAlphaSaturate:
            return QRhiGraphicsPipeline::SrcAlphaSaturate;
        case QBlendEquationArguments::ConstantColor:
            return QRhiGraphicsPipeline::ConstantColor;
        case QBlendEquationArguments::ConstantAlpha:
            return QRhiGraphicsPipeline::ConstantAlpha;
        case QBlendEquationArguments::OneMinusSourceColor:
            return QRhiGraphicsPipeline::OneMinusSrcColor;
        case QBlendEquationArguments::OneMinusSourceAlpha:
            return QRhiGraphicsPipeline::OneMinusSrcAlpha;
        case QBlendEquationArguments::OneMinusDestinationAlpha:
            return QRhiGraphicsPipeline::OneMinusDstAlpha;
        case QBlendEquationArguments::OneMinusDestinationColor:
            return QRhiGraphicsPipeline::OneMinusDstColor;
        case QBlendEquationArguments::OneMinusConstantColor:
            return QRhiGraphicsPipeline::OneMinusConstantColor;
        case QBlendEquationArguments::OneMinusConstantAlpha:
            return QRhiGraphicsPipeline::OneMinusConstantAlpha;
        case QBlendEquationArguments::OneMinusSource1Alpha:
            return QRhiGraphicsPipeline::OneMinusSrc1Alpha;
        case QBlendEquationArguments::OneMinusSource1Color:
            return QRhiGraphicsPipeline::OneMinusSrc1Color;
        default:
            qDebug() << "Unhandled blend equation argument" << arg;
            return QRhiGraphicsPipeline::Zero;
        }
    };

    targetBlend.srcAlpha = getRHIBlendFactor(std::get<2>(values));
    targetBlend.dstAlpha = getRHIBlendFactor(std::get<3>(values));
    targetBlend.srcColor = getRHIBlendFactor(std::get<0>(values));
    targetBlend.dstColor = getRHIBlendFactor(std::get<1>(values));
    gp->setTargetBlends({ targetBlend });
}

void applyStateHelper(const BlendEquation *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();
    const QBlendEquation::BlendFunction equation =
            static_cast<QBlendEquation::BlendFunction>(std::get<0>(values));

    // We assume a single color attachment
    QRhiGraphicsPipeline::TargetBlend targetBlend;

    const bool hasTargetBlend = gp->cbeginTargetBlends() != gp->cendTargetBlends();
    if (hasTargetBlend)
        targetBlend = *(gp->cbeginTargetBlends());

    auto getRHIBlendOp = [](QBlendEquation::BlendFunction equation) {
        switch (equation) {
        case QBlendEquation::Add:
            return QRhiGraphicsPipeline::Add;
        case QBlendEquation::Subtract:
            return QRhiGraphicsPipeline::Subtract;
        case QBlendEquation::ReverseSubtract:
            return QRhiGraphicsPipeline::ReverseSubtract;
        case QBlendEquation::Min:
            return QRhiGraphicsPipeline::Min;
        case QBlendEquation::Max:
            return QRhiGraphicsPipeline::Max;
        }
    };

    targetBlend.enable = true;
    targetBlend.opAlpha = getRHIBlendOp(equation);
    gp->setTargetBlends({ targetBlend });
}

void applyStateHelper(const MSAAEnabled *state, QRhiGraphicsPipeline *gp,
                      const QSurfaceFormat &format) noexcept
{
    gp->setSampleCount(format.samples());
}

void applyStateHelper(const DepthTest *state, QRhiGraphicsPipeline *gp) noexcept
{
    gp->setDepthTest(true);
    const QDepthTest::DepthFunction depthFunc =
            static_cast<QDepthTest::DepthFunction>(std::get<0>(state->values()));
    switch (depthFunc) {
    case QDepthTest::Never:
        gp->setDepthOp(QRhiGraphicsPipeline::Never);
        break;
    case QDepthTest::Always:
        gp->setDepthOp(QRhiGraphicsPipeline::Always);
        break;
    case QDepthTest::Less:
        gp->setDepthOp(QRhiGraphicsPipeline::Less);
        break;
    case QDepthTest::LessOrEqual:
        gp->setDepthOp(QRhiGraphicsPipeline::LessOrEqual);
        break;
    case QDepthTest::Equal:
        gp->setDepthOp(QRhiGraphicsPipeline::Equal);
        break;
    case QDepthTest::GreaterOrEqual:
        gp->setDepthOp(QRhiGraphicsPipeline::GreaterOrEqual);
        break;
    case QDepthTest::Greater:
        gp->setDepthOp(QRhiGraphicsPipeline::Greater);
        break;
    case QDepthTest::NotEqual:
        gp->setDepthOp(QRhiGraphicsPipeline::NotEqual);
        break;
    }
}

void applyStateHelper(const NoDepthMask *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();
    gp->setDepthWrite(std::get<0>(values));
}

void applyStateHelper(const CullFace *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();
    const QCullFace::CullingMode cullingMode =
            static_cast<QCullFace::CullingMode>(std::get<0>(values));
    switch (cullingMode) {
    case QCullFace::NoCulling:
        gp->setCullMode(QRhiGraphicsPipeline::None);
        break;
    case QCullFace::Front:
        gp->setCullMode(QRhiGraphicsPipeline::Front);
        break;
    case QCullFace::Back:
        gp->setCullMode(QRhiGraphicsPipeline::Back);
        break;
    case QCullFace::FrontAndBack:
        qWarning() << "RHI doesn't handle FrontAndBack CullFace";
        break;
    }
}

void applyStateHelper(const FrontFace *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();
    const QFrontFace::WindingDirection cullingMode =
            static_cast<QFrontFace::WindingDirection>(std::get<0>(values));

    switch (cullingMode) {
    case QFrontFace::ClockWise:
        gp->setFrontFace(QRhiGraphicsPipeline::CW);
        break;
    case QFrontFace::CounterClockWise:
        gp->setFrontFace(QRhiGraphicsPipeline::CCW);
        break;
    }
}

void applyStateHelper(const StencilTest *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();
    gp->setStencilTest(true);

    auto getCompareOp = [](int compareOp) {
        switch (compareOp) {
        case QStencilTestArguments::Never:
            return QRhiGraphicsPipeline::Never;
        case QStencilTestArguments::Always:
            return QRhiGraphicsPipeline::Always;
        case QStencilTestArguments::Less:
            return QRhiGraphicsPipeline::Less;
        case QStencilTestArguments::LessOrEqual:
            return QRhiGraphicsPipeline::LessOrEqual;
        case QStencilTestArguments::Equal:
            return QRhiGraphicsPipeline::Equal;
        case QStencilTestArguments::GreaterOrEqual:
            return QRhiGraphicsPipeline::GreaterOrEqual;
        case QStencilTestArguments::Greater:
            return QRhiGraphicsPipeline::Greater;
        case QStencilTestArguments::NotEqual:
            return QRhiGraphicsPipeline::NotEqual;
        default:
            qDebug() << "Unhandled stencil test argument";
            return QRhiGraphicsPipeline::Never;
        }
    };

    QRhiGraphicsPipeline::StencilOpState frontCompare = gp->stencilFront();
    frontCompare.compareOp = getCompareOp(std::get<0>(values));
    gp->setStencilFront(frontCompare);

    QRhiGraphicsPipeline::StencilOpState backCompare = gp->stencilBack();
    backCompare.compareOp = getCompareOp(std::get<3>(values));
    gp->setStencilBack(backCompare);
}

void applyStateHelper(const ColorMask *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();

    // We assume a single color attachment
    QRhiGraphicsPipeline::TargetBlend targetBlend;

    const bool hasTargetBlend = gp->cbeginTargetBlends() != gp->cendTargetBlends();
    if (hasTargetBlend)
        targetBlend = *(gp->cbeginTargetBlends());

    const bool redEnabled = std::get<0>(values);
    const bool greenEnabled = std::get<1>(values);
    const bool blueEnabled = std::get<2>(values);
    const bool alphaEnabled = std::get<3>(values);

    QRhiGraphicsPipeline::ColorMask colorMask;
    if (redEnabled)
        colorMask |= QRhiGraphicsPipeline::R;
    if (greenEnabled)
        colorMask |= QRhiGraphicsPipeline::G;
    if (blueEnabled)
        colorMask |= QRhiGraphicsPipeline::B;
    if (alphaEnabled)
        colorMask |= QRhiGraphicsPipeline::A;

    targetBlend.colorWrite = colorMask;
    gp->setTargetBlends({ targetBlend });
}

void applyStateHelper(const StencilOp *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();
    auto getRHIStencilOp = [](int op) {
        switch (op) {
        case QStencilOperationArguments::Zero:
            return QRhiGraphicsPipeline::StencilZero;
        case QStencilOperationArguments::Keep:
            return QRhiGraphicsPipeline::Keep;
        case QStencilOperationArguments::Replace:
            return QRhiGraphicsPipeline::Replace;
        case QStencilOperationArguments::Increment:
            return QRhiGraphicsPipeline::IncrementAndClamp;
        case QStencilOperationArguments::Decrement:
            return QRhiGraphicsPipeline::DecrementAndClamp;
        case QStencilOperationArguments::IncrementWrap:
            return QRhiGraphicsPipeline::IncrementAndWrap;
        case QStencilOperationArguments::DecrementWrap:
            return QRhiGraphicsPipeline::DecrementAndWrap;
        case QStencilOperationArguments::Invert:
            return QRhiGraphicsPipeline::Invert;
        default:
            qDebug() << "Unhandled stencil operation argument";
            return QRhiGraphicsPipeline::StencilZero;
        }
    };

    QRhiGraphicsPipeline::StencilOpState frontCompare = gp->stencilFront();
    frontCompare.depthFailOp = getRHIStencilOp(std::get<1>(values));
    frontCompare.failOp = getRHIStencilOp(std::get<0>(values));
    frontCompare.passOp = getRHIStencilOp(std::get<2>(values));
    gp->setStencilFront(frontCompare);

    QRhiGraphicsPipeline::StencilOpState backCompare = gp->stencilBack();
    backCompare.depthFailOp = getRHIStencilOp(std::get<4>(values));
    backCompare.failOp = getRHIStencilOp(std::get<3>(values));
    backCompare.passOp = getRHIStencilOp(std::get<5>(values));
    gp->setStencilBack(backCompare);
}

void applyStateHelper(const StencilMask *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();
    gp->setStencilWriteMask(std::get<0>(values));
    gp->setStencilReadMask(std::get<1>(values));
}

static QShader::Stage rhiShaderStage(QShaderProgram::ShaderType type) noexcept
{
    switch (type) {
    case QShaderProgram::Vertex:
        return QShader::VertexStage;
    case QShaderProgram::Fragment:
        return QShader::FragmentStage;
    case QShaderProgram::TessellationControl:
        return QShader::TessellationControlStage;
    case QShaderProgram::TessellationEvaluation:
        return QShader::TessellationEvaluationStage;
    case QShaderProgram::Geometry:
        return QShader::GeometryStage;
    case QShaderProgram::Compute:
        return QShader::ComputeStage;
    default:
        std::abort();
    }
}

} // anonymous

SubmissionContext::SubmissionContext()
    : m_ownCurrent(true),
      m_id(nextFreeContextId()),
      m_surface(nullptr),
      m_activeShader(nullptr),
      m_renderTargetFormat(QAbstractTexture::NoFormat),
      m_material(nullptr),
      m_activeFBO(0),
      m_renderer(nullptr),
      m_uboTempArray(QByteArray(1024, 0)),
      m_initialized(false),
      m_maxTextureUnits(0),
      m_defaultFBO(0),
      m_rhi(nullptr),
      m_currentSwapChain(nullptr),
      m_currentRenderPassDescriptor(nullptr)
#ifndef QT_NO_OPENGL
      ,
      m_fallbackSurface(nullptr)
#endif
{
    static_contexts[m_id] = this;
    m_contextInfo.m_api = QGraphicsApiFilter::RHI;

    // We set those version numbers because QShaderGenerator wants major > 0
    m_contextInfo.m_major = 1;
    m_contextInfo.m_minor = 0;
}

SubmissionContext::~SubmissionContext()
{
    releaseResources();

    Q_ASSERT(static_contexts[m_id] == this);
    static_contexts.remove(m_id);
}

void SubmissionContext::initialize()
{
    m_initialized = true;
    // m_textureContext.initialize(this);

    Qt3DRender::API requestedApi = Qt3DRender::API::OpenGL;
    const auto userRequestedApi = qgetenv("QT3D_RHI_DEFAULT_API").toLower();
    if (!userRequestedApi.isEmpty()) {
        if (userRequestedApi == QByteArrayLiteral("opengl")) {
            requestedApi = Qt3DRender::API::OpenGL;
        } else if (userRequestedApi == QByteArrayLiteral("vulkan")) {
            requestedApi = Qt3DRender::API::Vulkan;
        } else if (userRequestedApi == QByteArrayLiteral("metal")) {
            requestedApi = Qt3DRender::API::Metal;
        } else if (userRequestedApi == QByteArrayLiteral("d3d11")) {
            requestedApi = Qt3DRender::API::DirectX;
        } else if (userRequestedApi == QByteArrayLiteral("null")) {
            requestedApi = Qt3DRender::API::Null;
        }
    }

    QRhi::Flags rhiFlags = QRhi::EnableDebugMarkers;

#if QT_CONFIG(vulkan)
    if (requestedApi == Qt3DRender::API::Vulkan) {
        QRhiVulkanInitParams params;
        params.inst = &Qt3DRender::staticVulkanInstance();
        m_rhi = QRhi::create(QRhi::Vulkan, &params, rhiFlags);
    }
#endif

#ifdef Q_OS_WIN
    if (requestedApi == Qt3DRender::API::DirectX) {
        QRhiD3D11InitParams params;
        params.enableDebugLayer = true;
        m_rhi = QRhi::create(QRhi::D3D11, &params, rhiFlags);
    }
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
    if (requestedApi == Qt3DRender::API::Metal) {
        QRhiMetalInitParams params;
        m_rhi = QRhi::create(QRhi::Metal, &params, rhiFlags);
    }
#endif
    if (requestedApi == Qt3DRender::API::Null) {
        QRhiInitParams params;
        m_rhi = QRhi::create(QRhi::Null, &params, rhiFlags);
    }

    if (requestedApi != Qt3DRender::API::OpenGL && m_rhi == nullptr) {
        qWarning() << "RHI: Unable to use requested RHI Api, trying to fall back on OpenGL";
        requestedApi = Qt3DRender::API::OpenGL;
    }

    if (requestedApi == Qt3DRender::API::OpenGL) {
#ifndef QT_NO_OPENGL
        m_fallbackSurface = QRhiGles2InitParams::newFallbackSurface();
        QRhiGles2InitParams params;
        params.format = QSurfaceFormat::defaultFormat();
        params.fallbackSurface = m_fallbackSurface;
        m_rhi = QRhi::create(QRhi::OpenGLES2, &params, rhiFlags);
#else
        qWarning() << "RHI: OpenGL not supported";
#endif
    }

    Q_ASSERT(m_rhi != nullptr);
}

void SubmissionContext::resolveRenderTargetFormat()
{
    RHI_UNIMPLEMENTED;

    //*     const QSurfaceFormat format = m_gl->format();
    //*     const uint a = (format.alphaBufferSize() == -1) ? 0 : format.alphaBufferSize();
    //*     const uint r = format.redBufferSize();
    //*     const uint g = format.greenBufferSize();
    //*     const uint b = format.blueBufferSize();
    //*
    //* #define RGBA_BITS(r,g,b,a) (r | (g << 6) | (b << 12) | (a << 18))
    //*
    //*     const uint bits = RGBA_BITS(r,g,b,a);
    //*     switch (bits) {
    //*     case RGBA_BITS(8,8,8,8):
    //*         m_renderTargetFormat = QAbstractTexture::RGBA8_UNorm;
    //*         break;
    //*     case RGBA_BITS(8,8,8,0):
    //*         m_renderTargetFormat = QAbstractTexture::RGB8_UNorm;
    //*         break;
    //*     case RGBA_BITS(5,6,5,0):
    //*         m_renderTargetFormat = QAbstractTexture::R5G6B5;
    //*         break;
    //*     }
    //* #undef RGBA_BITS
}

bool SubmissionContext::beginDrawing(QSurface *surface)
{
    Q_ASSERT(surface);

    m_surface = surface;

    // TO DO: Find a way to make to pause work if the window is not exposed
    //    if (m_surface && m_surface->surfaceClass() == QSurface::Window) {
    //        qDebug() << Q_FUNC_INFO << 1;
    //        if (!static_cast<QWindow *>(m_surface)->isExposed())
    //            return false;
    //        qDebug() << Q_FUNC_INFO << 2;
    //    }

    // Makes the surface current on the OpenGLContext
    // and sets the right glHelper
    // m_ownCurrent = !(m_gl->surface() == m_surface);
    // if (m_ownCurrent && !makeCurrent(m_surface))
    //     return false;

    // TODO: cache surface format somewhere rather than doing this every time render surface changes
    resolveRenderTargetFormat();

#if defined(QT3D_RENDER_ASPECT_RHI_DEBUG)
    GLint err = m_gl->functions()->glGetError();
    if (err != 0) {
        qCWarning(Backend) << Q_FUNC_INFO << "glGetError:" << err;
    }
#endif

    Q_ASSERT(isInitialized());

    // need to reset these values every frame, may get overwritten elsewhere
    RHI_UNIMPLEMENTED;

    if (m_activeShader) {
        m_activeShader = nullptr;
    }

    // Check if we have a swapchain for the Window, if not create one
    SwapChainInfo *swapChainInfo = swapChainForSurface(surface);
    QRhiSwapChain *swapChain = swapChainInfo->swapChain;

    // TO DO: Check if that's required all the time
    {
        // Rebuild RenderPassDescriptor
        // TODO -> this is not necessary, swapChain->buildOrResize already does it
        // swapChainInfo->renderBuffer->setPixelSize(surface->size());
        // swapChainInfo->renderBuffer->build();

        // Resize swapchain if needed
        if (m_surface->size() != swapChain->surfacePixelSize()) {
            bool couldRebuild = swapChain->buildOrResize();
            if (!couldRebuild)
                return false;
        }
    }

    m_currentSwapChain = swapChain;
    m_currentRenderPassDescriptor = swapChainInfo->renderPassDescriptor;

    // Begin Frame
    const auto success = m_rhi->beginFrame(m_currentSwapChain);

    return success == QRhi::FrameOpSuccess;
}

void SubmissionContext::endDrawing(bool swapBuffers)
{
    m_rhi->endFrame(m_currentSwapChain, {});

    RHI_UNIMPLEMENTED;
    //* if (swapBuffers)
    //*     m_gl->swapBuffers(m_surface);
    //* if (m_ownCurrent)
    //*     m_gl->doneCurrent();
    // m_textureContext.endDrawing();
    //*    static int i = 0;
    //*    if (i++ == 10)
    //*        std::exit(0);
}

void SubmissionContext::activateRenderTarget(Qt3DCore::QNodeId renderTargetNodeId,
                                             const AttachmentPack &attachments, GLuint defaultFboId)
{
    RHI_UNIMPLEMENTED;
    GLuint fboId = defaultFboId; // Default FBO
    if (renderTargetNodeId) {
        // New RenderTarget
        if (!m_renderTargets.contains(renderTargetNodeId)) {
            if (m_defaultFBO && fboId == m_defaultFBO) {
                // this is the default fbo that some platforms create (iOS), we just register it
                // Insert FBO into hash
                m_renderTargets.insert(renderTargetNodeId, fboId);
            } else {
                RHI_UNIMPLEMENTED;
                fboId = createRenderTarget(renderTargetNodeId, attachments);
            }
        } else {
            RHI_UNIMPLEMENTED;
            fboId = updateRenderTarget(renderTargetNodeId, attachments, true);
        }
    }
    m_activeFBO = fboId;
    //*    m_glHelper->bindFrameBufferObject(m_activeFBO, GraphicsHelperInterface::FBODraw);
    // Set active drawBuffers
    activateDrawBuffers(attachments);
}

GLuint SubmissionContext::createRenderTarget(Qt3DCore::QNodeId renderTargetNodeId,
                                             const AttachmentPack &attachments)
{
    RHI_UNIMPLEMENTED;
    return 0;
    //*    const GLuint fboId = m_glHelper->createFrameBufferObject();
    //*    if (fboId) {
    //*        // The FBO is created and its attachments are set once
    //*        // Insert FBO into hash
    //*        m_renderTargets.insert(renderTargetNodeId, fboId);
    //*        // Bind FBO
    //*        m_glHelper->bindFrameBufferObject(fboId, GraphicsHelperInterface::FBODraw);
    //*        bindFrameBufferAttachmentHelper(fboId, attachments);
    //*    } else {
    //*        qCritical("Failed to create FBO");
    //*    }
    //*    return fboId;
}

GLuint SubmissionContext::updateRenderTarget(Qt3DCore::QNodeId renderTargetNodeId,
                                             const AttachmentPack &attachments,
                                             bool isActiveRenderTarget)
{
    RHI_UNIMPLEMENTED;
    return 0;
    //*    const GLuint fboId = m_renderTargets.value(renderTargetNodeId);
    //*
    //*    // We need to check if  one of the attachment was resized
    //*    bool needsResize = !m_renderTargetsSize.contains(fboId);    // not even initialized yet?
    //*    if (!needsResize) {
    //*        // render target exists, has attachment been resized?
    //*        RHITextureManager *rhiTextureManager =
    //m_renderer->rhiResourceManagers()->rhiTextureManager();
    //*        const QSize s = m_renderTargetsSize[fboId];
    //*        const auto attachments_ = attachments.attachments();
    //*        for (const Attachment &attachment : attachments_) {
    //*            RHITexture *rTex = rhiTextureManager->lookupResource(attachment.m_textureUuid);
    //*            // ### TODO QTBUG-64757 this check is insufficient since the
    //*            // texture may have changed to another one with the same size. That
    //*            // case is not handled atm.
    //*            if (rTex) {
    //*                needsResize |= rTex->size() != s;
    //*                if (isActiveRenderTarget && attachment.m_point ==
    //QRenderTargetOutput::Color0)
    //*                    m_renderTargetFormat = rTex->properties().format;
    //*            }
    //*        }
    //*    }
    //*
    //*    if (needsResize) {
    //*        m_glHelper->bindFrameBufferObject(fboId, GraphicsHelperInterface::FBODraw);
    //*        bindFrameBufferAttachmentHelper(fboId, attachments);
    //*    }
    //*
    //*    return fboId;
}

QSize SubmissionContext::renderTargetSize(const QSize &surfaceSize) const
{
    RHI_UNIMPLEMENTED;
    return surfaceSize;
    //*    QSize renderTargetSize{};
    //*    if (m_activeFBO != m_defaultFBO) {
    //*        // For external FBOs we may not have a m_renderTargets entry.
    //*        if (m_renderTargetsSize.contains(m_activeFBO)) {
    //*            renderTargetSize = m_renderTargetsSize[m_activeFBO];
    //*        } else if (surfaceSize.isValid()) {
    //*            renderTargetSize = surfaceSize;
    //*        } else {
    //*            // External FBO (when used with QtQuick2 Scene3D)
    //*
    //*            // Query FBO color attachment 0 size
    //*            GLint attachmentObjectType = GL_NONE;
    //*            GLint attachment0Name = 0;
    //*            m_gl->functions()->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
    //*                                                                     GL_COLOR_ATTACHMENT0,
    //* GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
    //*                                                                     &attachmentObjectType);
    //*            m_gl->functions()->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
    //*                                                                     GL_COLOR_ATTACHMENT0,
    //* GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
    //*                                                                     &attachment0Name);
    //*
    //*            if (attachmentObjectType == GL_RENDERBUFFER &&
    //m_glHelper->supportsFeature(GraphicsHelperInterface::RenderBufferDimensionRetrieval))
    //*                renderTargetSize = m_glHelper->getRenderBufferDimensions(attachment0Name);
    //*            else if (attachmentObjectType == GL_TEXTURE &&
    //m_glHelper->supportsFeature(GraphicsHelperInterface::TextureDimensionRetrieval))
    //*                // Assumes texture level 0 and GL_TEXTURE_2D target
    //*                renderTargetSize = m_glHelper->getTextureDimensions(attachment0Name,
    //GL_TEXTURE_2D);
    //*            else
    //*                return renderTargetSize;
    //*        }
    //*    } else {
    //*        renderTargetSize = m_surface->size();
    //*        if (m_surface->surfaceClass() == QSurface::Window) {
    //*            const float dpr = static_cast<QWindow *>(m_surface)->devicePixelRatio();
    //*            renderTargetSize *= dpr;
    //*        }
    //*    }
    //*    return renderTargetSize;
}

QImage SubmissionContext::readFramebuffer(const QRect &rect)
{
    RHI_UNIMPLEMENTED;
    return {};
    //*    QImage img;
    //*    const unsigned int area = rect.width() * rect.height();
    //*    unsigned int bytes;
    //*    GLenum format, type;
    //*    QImage::Format imageFormat;
    //*    uint stride;
    //*
    //*    /* format value should match GL internalFormat */
    //*    GLenum internalFormat = m_renderTargetFormat;
    //*
    //*    switch (m_renderTargetFormat) {
    //*    case QAbstractTexture::RGBAFormat:
    //*    case QAbstractTexture::RGBA8_SNorm:
    //*    case QAbstractTexture::RGBA8_UNorm:
    //*    case QAbstractTexture::RGBA8U:
    //*    case QAbstractTexture::SRGB8_Alpha8:
    //*#ifdef QT_OPENGL_ES_2
    //*        format = GL_RGBA;
    //*        imageFormat = QImage::Format_RGBA8888_Premultiplied;
    //*#else
    //*        format = GL_BGRA;
    //*        imageFormat = QImage::Format_ARGB32_Premultiplied;
    //*        internalFormat = GL_RGBA8;
    //*#endif
    //*        type = GL_UNSIGNED_BYTE;
    //*        bytes = area * 4;
    //*        stride = rect.width() * 4;
    //*        break;
    //*    case QAbstractTexture::SRGB8:
    //*    case QAbstractTexture::RGBFormat:
    //*    case QAbstractTexture::RGB8U:
    //*    case QAbstractTexture::RGB8_UNorm:
    //*#ifdef QT_OPENGL_ES_2
    //*        format = GL_RGBA;
    //*        imageFormat = QImage::Format_RGBX8888;
    //*#else
    //*        format = GL_BGRA;
    //*        imageFormat = QImage::Format_RGB32;
    //*        internalFormat = GL_RGB8;
    //*#endif
    //*        type = GL_UNSIGNED_BYTE;
    //*        bytes = area * 4;
    //*        stride = rect.width() * 4;
    //*        break;
    //*#ifndef QT_OPENGL_ES_2
    //*    case QAbstractTexture::RG11B10F:
    //*        bytes = area * 4;
    //*        format = GL_RGB;
    //*        type = GL_UNSIGNED_INT_10F_11F_11F_REV;
    //*        imageFormat = QImage::Format_RGB30;
    //*        stride = rect.width() * 4;
    //*        break;
    //*    case QAbstractTexture::RGB10A2:
    //*        bytes = area * 4;
    //*        format = GL_RGBA;
    //*        type = GL_UNSIGNED_INT_2_10_10_10_REV;
    //*        imageFormat = QImage::Format_A2BGR30_Premultiplied;
    //*        stride = rect.width() * 4;
    //*        break;
    //*    case QAbstractTexture::R5G6B5:
    //*        bytes = area * 2;
    //*        format = GL_RGB;
    //*        type = GL_UNSIGNED_SHORT;
    //*        internalFormat = GL_UNSIGNED_SHORT_5_6_5_REV;
    //*        imageFormat = QImage::Format_RGB16;
    //*        stride = rect.width() * 2;
    //*        break;
    //*    case QAbstractTexture::RGBA16F:
    //*    case QAbstractTexture::RGBA16U:
    //*    case QAbstractTexture::RGBA32F:
    //*    case QAbstractTexture::RGBA32U:
    //*        bytes = area * 16;
    //*        format = GL_RGBA;
    //*        type = GL_FLOAT;
    //*        imageFormat = QImage::Format_ARGB32_Premultiplied;
    //*        stride = rect.width() * 16;
    //*        break;
    //*#endif
    //*    default:
    //*        auto warning = qWarning();
    //*        warning << "Unable to convert";
    //*        QtDebugUtils::formatQEnum(warning, m_renderTargetFormat);
    //*        warning << "render target texture format to QImage.";
    //*        return img;
    //*    }
    //*
    //*    GLint samples = 0;
    //*    m_gl->functions()->glGetIntegerv(GL_SAMPLES, &samples);
    //*    if (samples > 0 &&
    //!m_glHelper->supportsFeature(GraphicsHelperInterface::BlitFramebuffer)) {
    //*        qCWarning(Backend) << Q_FUNC_INFO << "Unable to capture multisampled framebuffer; "
    //*                                             "Required feature BlitFramebuffer is missing.";
    //*        return img;
    //*    }
    //*
    //*    img = QImage(rect.width(), rect.height(), imageFormat);
    //*
    //*    QScopedArrayPointer<uchar> data(new uchar [bytes]);
    //*
    //*    if (samples > 0) {
    //*        // resolve multisample-framebuffer to renderbuffer and read pixels from it
    //*        GLuint fbo, rb;
    //*        QOpenGLFunctions *gl = m_gl->functions();
    //*        gl->glGenFramebuffers(1, &fbo);
    //*        gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    //*        gl->glGenRenderbuffers(1, &rb);
    //*        gl->glBindRenderbuffer(GL_RENDERBUFFER, rb);
    //*        gl->glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, rect.width(),
    //rect.height());
    //*        gl->glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    //GL_RENDERBUFFER, rb);
    //*
    //*        const GLenum status = gl->glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    //*        if (status != GL_FRAMEBUFFER_COMPLETE) {
    //*            gl->glDeleteRenderbuffers(1, &rb);
    //*            gl->glDeleteFramebuffers(1, &fbo);
    //*            qCWarning(Backend) << Q_FUNC_INFO << "Copy-framebuffer not complete: " << status;
    //*            return img;
    //*        }
    //*
    //*        m_glHelper->blitFramebuffer(rect.x(), rect.y(), rect.x() + rect.width(), rect.y() +
    //rect.height(),
    //*                                    0, 0, rect.width(), rect.height(),
    //*                                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
    //*        gl->glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    //*        gl->glReadPixels(0,0,rect.width(), rect.height(), format, type, data.data());
    //*
    //*        copyGLFramebufferDataToImage(img, data.data(), stride, rect.width(), rect.height(),
    //m_renderTargetFormat);
    //*
    //*        gl->glBindRenderbuffer(GL_RENDERBUFFER, rb);
    //*        gl->glDeleteRenderbuffers(1, &rb);
    //*        gl->glBindFramebuffer(GL_FRAMEBUFFER, m_activeFBO);
    //*        gl->glDeleteFramebuffers(1, &fbo);
    //*    } else {
    //*        // read pixels directly from framebuffer
    //*        m_gl->functions()->glReadPixels(rect.x(), rect.y(), rect.width(), rect.height(),
    //format, type, data.data());
    //*        copyGLFramebufferDataToImage(img, data.data(), stride, rect.width(), rect.height(),
    //m_renderTargetFormat);
    //*    }
    //*
    //*    return img;
}

void SubmissionContext::releaseResources()
{
    m_renderBufferHash.clear();
    RHI_UNIMPLEMENTED;

    // Free RHI resources
    {
        qCDebug(Backend) << Q_FUNC_INFO;

        // We must ensure no remaining resource before deleting m_rhi.
        m_renderer->rhiResourceManagers()->releaseAllResources();

        auto it = m_swapChains.begin();
        while (it != m_swapChains.end()) {
            SwapChainInfo &swapChainInfo = it.value();
            delete swapChainInfo.renderPassDescriptor;
            delete swapChainInfo.renderBuffer;
            delete swapChainInfo.swapChain;
            it = m_swapChains.erase(it);
        }

        delete m_rhi;
        m_rhi = nullptr;

#ifndef QT_NO_OPENGL
        delete m_fallbackSurface;
        m_fallbackSurface = nullptr;
#endif
    }

    //*    // Stop and destroy the OpenGL logger
    //*    if (m_debugLogger) {
    //*        m_debugLogger->stopLogging();
    //*        m_debugLogger.reset(nullptr);
    //*    }
}

// Called only from RenderThread
bool SubmissionContext::activateShader(RHIShader *shader)
{
    RHI_UNIMPLEMENTED;
    //* if (shader->shaderProgram() != m_activeShader) {
    //*     // Ensure material uniforms are re-applied
    //*     m_material = nullptr;
    //*
    //*     m_activeShader = shader->shaderProgram();
    //*     if (Q_LIKELY(m_activeShader != nullptr)) {
    //*         m_activeShader->bind();
    //*     } else {
    //*         m_glHelper->useProgram(0);
    //*         qWarning() << "No shader program found";
    //*         return false;
    //*     }
    //* }
    return true;
}

void SubmissionContext::bindFrameBufferAttachmentHelper(GLuint fboId,
                                                        const AttachmentPack &attachments)
{
    RHI_UNIMPLEMENTED;
    // Set FBO attachments. These are normally textures, except that on Open GL
    // ES <= 3.1 we must use a renderbuffer if a combined depth+stencil is
    // desired since this cannot be achieved neither with a single texture (not
    // before GLES 3.2) nor with separate textures (no suitable format for
    // stencil before 3.1 with the appropriate extension).

    //* QSize fboSize;
    //* RHITextureManager *rhiTextureManager =
    //m_renderer->rhiResourceManagers()->rhiTextureManager();
    //* const auto attachments_ = attachments.attachments();
    //* for (const Attachment &attachment : attachments_) {
    //*     RHITexture *rTex = rhiTextureManager->lookupResource(attachment.m_textureUuid);
    //*     if (!m_glHelper->frameBufferNeedsRenderBuffer(attachment)) {
    //*         QOpenGLTexture *glTex = rTex ? rTex->getGLTexture() : nullptr;
    //*         if (glTex != nullptr) {
    //*             // The texture can not be rendered simultaniously by another renderer
    //*             Q_ASSERT(!rTex->isExternalRenderingEnabled());
    //*             if (fboSize.isEmpty())
    //*                 fboSize = QSize(glTex->width(), glTex->height());
    //*             else
    //*                 fboSize = QSize(qMin(fboSize.width(), glTex->width()),
    //qMin(fboSize.height(), glTex->height()));
    //*             m_glHelper->bindFrameBufferAttachment(glTex, attachment);
    //*         }
    //*     } else {
    //*         RenderBuffer *renderBuffer = rTex ? rTex->getOrCreateRenderBuffer() : nullptr;
    //*         if (renderBuffer) {
    //*             if (fboSize.isEmpty())
    //*                 fboSize = QSize(renderBuffer->width(), renderBuffer->height());
    //*             else
    //*                 fboSize = QSize(qMin(fboSize.width(), renderBuffer->width()),
    //qMin(fboSize.height(), renderBuffer->height()));
    //*             m_glHelper->bindFrameBufferAttachment(renderBuffer, attachment);
    //*         }
    //*     }
    //* }
    //* m_renderTargetsSize.insert(fboId, fboSize);
}

void SubmissionContext::activateDrawBuffers(const AttachmentPack &attachments)
{
    RHI_UNIMPLEMENTED;
    //* const QVector<int> activeDrawBuffers = attachments.getGlDrawBuffers();
    //*
    //* if (m_glHelper->checkFrameBufferComplete()) {
    //*     if (activeDrawBuffers.size() > 1) {// We need MRT
    //*         if (m_glHelper->supportsFeature(GraphicsHelperInterface::MRT)) {
    //*             // Set up MRT, glDrawBuffers...
    //*             m_glHelper->drawBuffers(activeDrawBuffers.size(), activeDrawBuffers.data());
    //*         }
    //*     }
    //* } else {
    //*     qCWarning(Backend) << "FBO incomplete";
    //* }
}

void SubmissionContext::setActiveMaterial(Material *rmat)
{
    if (m_material == rmat)
        return;

    // m_textureContext.deactivateTexturesWithScope(TextureSubmissionContext::TextureScopeMaterial);
    m_material = rmat;
}

void SubmissionContext::applyState(const StateVariant &stateVariant,
                                   QRhiGraphicsPipeline *graphicsPipeline)
{
    switch (stateVariant.type) {

    case AlphaCoverageStateMask: {
        applyStateHelper(static_cast<const AlphaCoverage *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }
    case AlphaTestMask: {
        applyStateHelper(static_cast<const AlphaFunc *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }
    case BlendStateMask: {
        applyStateHelper(static_cast<const BlendEquation *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }
    case BlendEquationArgumentsMask: {
        applyStateHelper(static_cast<const BlendEquationArguments *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }
    case MSAAEnabledStateMask: {
        applyStateHelper(static_cast<const MSAAEnabled *>(stateVariant.constState()),
                         graphicsPipeline, m_renderer->format());
        break;
    }

    case CullFaceStateMask: {
        applyStateHelper(static_cast<const CullFace *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case DepthWriteStateMask: {
        applyStateHelper(static_cast<const NoDepthMask *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case DepthTestStateMask: {
        applyStateHelper(static_cast<const DepthTest *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case DepthRangeMask: {
        applyStateHelper(static_cast<const DepthRange *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case RasterModeMask: {
        applyStateHelper(static_cast<const RasterMode *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case FrontFaceStateMask: {
        applyStateHelper(static_cast<const FrontFace *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case ScissorStateMask: {
        applyStateHelper(static_cast<const ScissorTest *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case StencilTestStateMask: {
        applyStateHelper(static_cast<const StencilTest *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case PointSizeMask: {
        applyStateHelper(static_cast<const PointSize *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case PolygonOffsetStateMask: {
        applyStateHelper(static_cast<const PolygonOffset *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case ColorStateMask: {
        applyStateHelper(static_cast<const ColorMask *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case ClipPlaneMask: {
        applyStateHelper(static_cast<const ClipPlane *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case SeamlessCubemapMask: {
        applyStateHelper(static_cast<const SeamlessCubemap *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case StencilOpMask: {
        applyStateHelper(static_cast<const StencilOp *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case StencilWriteStateMask: {
        applyStateHelper(static_cast<const StencilMask *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case DitheringStateMask: {
        applyStateHelper(static_cast<const Dithering *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }

    case LineWidthMask: {
        applyStateHelper(static_cast<const LineWidth *>(stateVariant.constState()),
                         graphicsPipeline);
        break;
    }
    default:
        Q_UNREACHABLE();
    }
}

void SubmissionContext::applyStateSet(const RenderStateSet *ss,
                                      QRhiGraphicsPipeline *graphicsPipeline)
{
    // Set default state values on graphicsPipeline
    graphicsPipeline->setDepthWrite(true);
    graphicsPipeline->setDepthTest(false);
    graphicsPipeline->setSampleCount(format().samples());

    const QVector<StateVariant> statesToSet = ss->states();
    for (const StateVariant &ds : statesToSet)
        applyState(ds, graphicsPipeline);
}

StateVariant *SubmissionContext::getState(RenderStateSet *ss, StateMask type) const
{
    const QVector<StateVariant> &statesToSet = ss->states();
    for (int i = 0, m = statesToSet.size(); i < m; ++i) {
        const StateVariant &ds = statesToSet.at(i);
        if (ds.type == type)
            return ss->states().begin() + i;
    }
    return nullptr;
}

SubmissionContext::SwapChainInfo *SubmissionContext::swapChainForSurface(QSurface *surface) noexcept
{
    SwapChainInfo &swapChainInfo = m_swapChains[surface];
    auto &swapChain = swapChainInfo.swapChain;

    if (swapChain == nullptr) {
        swapChain = m_rhi->newSwapChain();
        Q_ASSERT(surface->surfaceClass() == QSurface::Window);
        QWindow *window = static_cast<QWindow *>(surface);
        Q_ASSERT(window != nullptr);
        const int samples = format().samples();

        swapChain->setWindow(window);
        swapChain->setFlags(QRhiSwapChain::Flags {});
        swapChain->setSampleCount(samples);

        QRhiRenderBuffer *renderBuffer =
                m_rhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil, QSize(), samples,
                                       QRhiRenderBuffer::UsedWithSwapChainOnly);
        swapChain->setDepthStencil(renderBuffer);

        QRhiRenderPassDescriptor *renderPassDescriptor =
                swapChain->newCompatibleRenderPassDescriptor();
        swapChain->setRenderPassDescriptor(renderPassDescriptor);

        // Build swapChain the first time
        if (swapChain->buildOrResize()) {
            swapChainInfo.swapChain = swapChain;
            swapChainInfo.renderBuffer = renderBuffer;
            swapChainInfo.renderPassDescriptor = renderPassDescriptor;
        } else {
            swapChain->releaseAndDestroyLater();
            m_swapChains.remove(surface);
            return nullptr;
        }
    }
    return &swapChainInfo;
}

QRhiCommandBuffer *SubmissionContext::currentFrameCommandBuffer() const
{
    return m_currentSwapChain->currentFrameCommandBuffer();
}

QRhiRenderTarget *SubmissionContext::currentFrameRenderTarget() const
{
    return m_currentSwapChain->currentFrameRenderTarget();
}

QRhiSwapChain *SubmissionContext::currentSwapChain() const
{
    return m_currentSwapChain;
}

QRhiRenderPassDescriptor *SubmissionContext::currentRenderPassDescriptor() const
{
    return m_currentRenderPassDescriptor;
}

QSurfaceFormat SubmissionContext::format() const noexcept
{
    if (this->m_rhi && this->m_rhi->backend() == QRhi::OpenGLES2) {
        auto rhi_gl = static_cast<const QRhiGles2NativeHandles *>(this->m_rhi->nativeHandles());
        return rhi_gl->context->format();
    }
    return QSurfaceFormat::defaultFormat();
}

// It will be easier if the QGraphicContext applies the QUniformPack
// than the other way around
bool SubmissionContext::setParameters(ShaderParameterPack &parameterPack)
{
    static const int irradianceId = StringToInt::lookupId(QLatin1String("envLight_irradiance"));
    static const int specularId = StringToInt::lookupId(QLatin1String("envLight_specular"));
    // Activate textures and update TextureUniform in the pack
    // with the correct textureUnit

    // Set the pinned texture of the previous material texture
    // to pinable so that we should easily find an available texture unit
    // m_textureContext.deactivateTexturesWithScope(TextureSubmissionContext::TextureScopeMaterial);
    // Update the uniforms with the correct texture unit id's
    PackUniformHash &uniformValues = parameterPack.uniforms();

    // Fill Texture Uniform Value with proper texture units
    // so that they can be applied as regular uniforms in a second step
    for (int i = 0; i < parameterPack.textures().size(); ++i) {
        RHI_UNIMPLEMENTED;
        //* const ShaderParameterPack::NamedResource &namedTex = parameterPack.textures().at(i);
        //* // Given a Texture QNodeId, we retrieve the associated shared RHITexture
        //* if (uniformValues.contains(namedTex.glslNameId)) {
        //*     RHITexture *t =
        //m_renderer->rhiResourceManagers()->rhiTextureManager()->lookupResource(namedTex.nodeId);
        //*     if (t != nullptr) {
        //*         UniformValue &texUniform = uniformValues.value(namedTex.glslNameId);
        //*         if (texUniform.valueType() == UniformValue::TextureValue) {
        //*             const int texUnit =
        //m_textureContext.activateTexture(TextureSubmissionContext::TextureScopeMaterial, m_gl, t);
        //*             texUniform.data<int>()[namedTex.uniformArrayIndex] = texUnit;
        //*             if (texUnit == -1) {
        //*                 if (namedTex.glslNameId != irradianceId &&
        //*                     namedTex.glslNameId != specularId) {
        //*                     // Only return false if we are not dealing with env light textures
        //*                     qCWarning(Backend) << "Unable to find suitable Texture Unit";
        //*                     return false;
        //*                 }
        //*             }
        //*         }
        //*     }
        //* }
    }

    RHIShader *shader = activeShader();

    // TO DO: We could cache the binding points somehow and only do the binding when necessary
    // for SSBO and UBO

    // Bind Shader Storage block to SSBO and update SSBO
    const QVector<BlockToSSBO> &blockToSSBOs = parameterPack.shaderStorageBuffers();
    for (const BlockToSSBO &b : blockToSSBOs) {
        RHI_UNIMPLEMENTED;
        Buffer *cpuBuffer =
                m_renderer->nodeManagers()->bufferManager()->lookupResource(b.m_bufferID);
        RHIBuffer *ssbo = rhiBufferForRenderBuffer(cpuBuffer);
        // bindShaderStorageBlock
        // This is currently not required as we are introspecting the bindingIndex
        // value from the shaders and not replacing them, making such a call useless
        // bindShaderStorageBlock(shader->programId(), b.m_blockIndex, b.m_bindingIndex);
        //        bindShaderStorageBlock(shader->programId(), b.m_blockIndex, b.m_bindingIndex);
        // Needed to avoid conflict where the buffer would already
        // be bound as a VertexArray
        bindRHIBuffer(ssbo, RHIBuffer::ShaderStorageBuffer);
        ssbo->bindBufferBase(this, b.m_bindingIndex, RHIBuffer::ShaderStorageBuffer);
        // TO DO: Make sure that there's enough binding points
    }

    // Bind UniformBlocks to UBO and update UBO from Buffer
    // TO DO: Convert ShaderData to Buffer so that we can use that generic process
    const QVector<BlockToUBO> blockToUBOs = parameterPack.uniformBuffers();
    int uboIndex = 0;
    for (const BlockToUBO &b : blockToUBOs) {
        RHI_UNIMPLEMENTED;
        Buffer *cpuBuffer =
                m_renderer->nodeManagers()->bufferManager()->lookupResource(b.m_bufferID);
        RHIBuffer *ubo = rhiBufferForRenderBuffer(cpuBuffer);
        //        bindUniformBlock(shader->programId(), b.m_blockIndex, uboIndex);
        // Needed to avoid conflict where the buffer would already
        // be bound as a VertexArray
        bindRHIBuffer(ubo, RHIBuffer::UniformBuffer);
        ubo->bindBufferBase(this, uboIndex++, RHIBuffer::UniformBuffer);
        // TO DO: Make sure that there's enough binding points
    }

    // Update uniforms in the Default Uniform Block
    const PackUniformHash values = parameterPack.uniforms();
    const QVector<ShaderUniform> activeUniforms = parameterPack.submissionUniforms();

    for (const ShaderUniform &uniform : activeUniforms) {
        RHI_UNIMPLEMENTED;
        // We can use [] as we are sure the the uniform wouldn't
        // be un activeUniforms if there wasn't a matching value
        const UniformValue &v = values.value(uniform.m_nameId);

        // skip invalid textures/images
        if ((v.valueType() == UniformValue::TextureValue
             || v.valueType() == UniformValue::ShaderImageValue)
            && *v.constData<int>() == -1)
            continue;

        //        applyUniform(uniform, v);
    }
    // if not all data is valid, the next frame will be rendered immediately
    return true;
}

void SubmissionContext::updateBuffer(Buffer *buffer)
{
    const QHash<Qt3DCore::QNodeId, HRHIBuffer>::iterator it =
            m_renderBufferHash.find(buffer->peerId());
    if (it != m_renderBufferHash.end())
        uploadDataToRHIBuffer(
                buffer, m_renderer->rhiResourceManagers()->rhiBufferManager()->data(it.value()));
}

QByteArray SubmissionContext::downloadBufferContent(Buffer *buffer)
{
    const QHash<Qt3DCore::QNodeId, HRHIBuffer>::iterator it =
            m_renderBufferHash.find(buffer->peerId());
    if (it != m_renderBufferHash.end())
        return downloadDataFromRHIBuffer(
                buffer, m_renderer->rhiResourceManagers()->rhiBufferManager()->data(it.value()));
    return QByteArray();
}

void SubmissionContext::releaseBuffer(Qt3DCore::QNodeId bufferId)
{
    auto it = m_renderBufferHash.find(bufferId);
    if (it != m_renderBufferHash.end()) {
        HRHIBuffer glBuffHandle = it.value();
        RHIBuffer *glBuff =
                m_renderer->rhiResourceManagers()->rhiBufferManager()->data(glBuffHandle);

        Q_ASSERT(glBuff);
        // Destroy the GPU resource
        glBuff->destroy(this);
        // Destroy the RHIBuffer instance
        m_renderer->rhiResourceManagers()->rhiBufferManager()->releaseResource(bufferId);
        // Remove Id - HRHIBuffer entry
        m_renderBufferHash.erase(it);
    }
}

bool SubmissionContext::hasRHIBufferForBuffer(Buffer *buffer)
{
    const QHash<Qt3DCore::QNodeId, HRHIBuffer>::iterator it =
            m_renderBufferHash.find(buffer->peerId());
    return (it != m_renderBufferHash.end());
}

RHIBuffer *SubmissionContext::rhiBufferForRenderBuffer(Buffer *buf)
{
    if (!m_renderBufferHash.contains(buf->peerId()))
        m_renderBufferHash.insert(buf->peerId(), createRHIBufferFor(buf));
    return m_renderer->rhiResourceManagers()->rhiBufferManager()->data(
            m_renderBufferHash.value(buf->peerId()));
}

HRHIBuffer SubmissionContext::createRHIBufferFor(Buffer *buffer)
{
    m_renderer->rhiResourceManagers()->rhiBufferManager()->getOrCreateResource(buffer->peerId());
    return m_renderer->rhiResourceManagers()->rhiBufferManager()->lookupHandle(buffer->peerId());
}

bool SubmissionContext::bindRHIBuffer(RHIBuffer *buffer, RHIBuffer::Type type)
{
    return buffer->bind(this, type);
}

void SubmissionContext::uploadDataToRHIBuffer(Buffer *buffer, RHIBuffer *b, bool releaseBuffer)
{
    // If the buffer is dirty (hence being called here)
    // there are two possible cases
    // * setData was called changing the whole data or functor (or the usage pattern)
    // * partial buffer updates where received

    // Note: we are only storing the updates data CPU side at this point
    // actually upload will be performed when the buffer will be bound
    // as we would otherwise need to know the usage type of the buffer
    QVector<Qt3DRender::QBufferUpdate> updates = std::move(buffer->pendingBufferUpdates());
    for (auto it = updates.begin(); it != updates.end(); ++it) {
        auto update = it;
        // We have a partial update
        if (update->offset >= 0) {
            // accumulate sequential updates as single one
            int bufferSize = update->data.size();
            auto it2 = it + 1;
            while ((it2 != updates.end()) && (it2->offset - update->offset == bufferSize)) {
                bufferSize += it2->data.size();
                ++it2;
            }
            update->data.resize(bufferSize);
            while (it + 1 != it2) {
                ++it;
                update->data.replace(it->offset - update->offset, it->data.size(), it->data);
                it->data.clear();
            }
            // TO DO: based on the number of updates .., it might make sense to
            // sometime use glMapBuffer rather than glBufferSubData
            b->update(this, update->data, update->offset);
        } else {
            // We have an update that was done by calling QBuffer::setData
            // which is used to resize or entirely clear the buffer
            // Note: we use the buffer data directly in that case
            b->orphan(this); // orphan the buffer
            b->allocate(this, buffer->data(), false);
        }
    }

    if (releaseBuffer) {
        b->release(this);
    }
    qCDebug(Io) << "uploaded buffer size=" << buffer->data().size();
}

QByteArray SubmissionContext::downloadDataFromRHIBuffer(Buffer *buffer, RHIBuffer *b)
{
    if (!bindRHIBuffer(b,
                       RHIBuffer::ArrayBuffer)) // We're downloading, the type doesn't matter here
        qCWarning(Io) << Q_FUNC_INFO << "buffer bind failed";

    return b->download(this, buffer->data().size());
}

void SubmissionContext::blitFramebuffer(Qt3DCore::QNodeId inputRenderTargetId,
                                        Qt3DCore::QNodeId outputRenderTargetId, QRect inputRect,
                                        QRect outputRect, uint defaultFboId,
                                        QRenderTargetOutput::AttachmentPoint inputAttachmentPoint,
                                        QRenderTargetOutput::AttachmentPoint outputAttachmentPoint,
                                        QBlitFramebuffer::InterpolationMethod interpolationMethod)
{
    RHI_UNIMPLEMENTED;
    //*    GLuint inputFboId = defaultFboId;
    //*    bool inputBufferIsDefault = true;
    //*    if (!inputRenderTargetId.isNull()) {
    //*        RenderTarget *renderTarget =
    //m_renderer->nodeManagers()->renderTargetManager()->lookupResource(inputRenderTargetId);
    //*        if (renderTarget) {
    //*            AttachmentPack attachments(renderTarget,
    //m_renderer->nodeManagers()->attachmentManager());
    //*            if (m_renderTargets.contains(inputRenderTargetId))
    //*                inputFboId = updateRenderTarget(inputRenderTargetId, attachments, false);
    //*            else
    //*                inputFboId = createRenderTarget(inputRenderTargetId, attachments);
    //*        }
    //*        inputBufferIsDefault = false;
    //*    }
    //*
    //*    GLuint outputFboId = defaultFboId;
    //*    bool outputBufferIsDefault = true;
    //*    if (!outputRenderTargetId.isNull()) {
    //*        RenderTarget *renderTarget =
    //m_renderer->nodeManagers()->renderTargetManager()->lookupResource(outputRenderTargetId);
    //*        if (renderTarget) {
    //*            AttachmentPack attachments(renderTarget,
    //m_renderer->nodeManagers()->attachmentManager());
    //*            if (m_renderTargets.contains(outputRenderTargetId))
    //*                outputFboId = updateRenderTarget(outputRenderTargetId, attachments, false);
    //*            else
    //*                outputFboId = createRenderTarget(outputRenderTargetId, attachments);
    //*        }
    //*        outputBufferIsDefault = false;
    //*    }
    //*
    //*    // Up until this point the input and output rects are normal Qt rectangles.
    //*    // Convert them to GL rectangles (Y at bottom).
    //*    const int inputFboHeight = inputFboId == defaultFboId ? m_surfaceSize.height() :
    //m_renderTargetsSize[inputFboId].height();
    //*    const GLint srcX0 = inputRect.left();
    //*    const GLint srcY0 = inputFboHeight - (inputRect.top() + inputRect.height());
    //*    const GLint srcX1 = srcX0 + inputRect.width();
    //*    const GLint srcY1 = srcY0 + inputRect.height();
    //*
    //*    const int outputFboHeight = outputFboId == defaultFboId ? m_surfaceSize.height() :
    //m_renderTargetsSize[outputFboId].height();
    //*    const GLint dstX0 = outputRect.left();
    //*    const GLint dstY0 = outputFboHeight - (outputRect.top() + outputRect.height());
    //*    const GLint dstX1 = dstX0 + outputRect.width();
    //*    const GLint dstY1 = dstY0 + outputRect.height();
    //*
    //*    //Get the last bounded framebuffers
    //*    const GLuint lastDrawFboId = boundFrameBufferObject();
    //*
    //*    // Activate input framebuffer for reading
    //*    bindFramebuffer(inputFboId, GraphicsHelperInterface::FBORead);
    //*
    //*    // Activate output framebuffer for writing
    //*    bindFramebuffer(outputFboId, GraphicsHelperInterface::FBODraw);
    //*
    //*    //Bind texture
    //*    if (!inputBufferIsDefault)
    //*        readBuffer(GL_COLOR_ATTACHMENT0 + inputAttachmentPoint);
    //*
    //*    if (!outputBufferIsDefault) {
    //*        // Note that we use glDrawBuffers, not glDrawBuffer. The
    //*        // latter is not available with GLES.
    //*        const int buf = outputAttachmentPoint;
    //*        drawBuffers(1, &buf);
    //*    }
    //*
    //*    // Blit framebuffer
    //*    const GLenum mode = interpolationMethod ? GL_NEAREST : GL_LINEAR;
    //*    m_glHelper->blitFramebuffer(srcX0, srcY0, srcX1, srcY1,
    //*                                dstX0, dstY0, dstX1, dstY1,
    //* GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT,
    //*                                mode);
    //*
    //*    // Reset draw buffer
    //*    bindFramebuffer(lastDrawFboId, GraphicsHelperInterface::FBOReadAndDraw);
    //*    if (outputAttachmentPoint != QRenderTargetOutput::Color0) {
    //*        const int buf = QRenderTargetOutput::Color0;
    //*        drawBuffers(1, &buf);
    //*    }
}

namespace {
template<std::size_t N>
constexpr int getFirstAvailableBit(const std::bitset<N> &bits)
{
    for (std::size_t i = 0; i < N; i++) {
        if (!bits.test(i))
            return i;
    }
    return -1;
}
// This function ensures that the shader stages all have the same bindings
void preprocessRHIShader(QVector<QByteArray> &shaderCodes)
{
    // Map the variable names to bindings
    std::map<QByteArray, int> bindings;
    bindings["qt3d_render_view_uniforms"] = 0;
    bindings["qt3d_command_uniforms"] = 1;
    std::bitset<512> assignedBindings;
    assignedBindings.set(0);
    assignedBindings.set(1);

    thread_local const QRegularExpression samplerRegex(
            QStringLiteral("binding\\s*=\\s*([0-9]+).*\\)\\s*uniform\\s*[ui]?sampler[a-zA-Z0-9]+"
                           "\\s*([a-zA-Z0-9_]+)\\s*;"));
    thread_local const QRegularExpression uboRegex(
            QStringLiteral("(?:std140\\s*,\\s*binding\\s*=\\s*([0-9]+).*|binding\\s*=\\s*([0-9]+)"
                           "\\s*,\\s*std140.*)\\)\\s*uniform\\s*([a-zA-Z0-9_]+)"));

    auto replaceBinding = [&bindings, &assignedBindings](
                                  int &offset, QRegularExpressionMatch &match, QString &code,
                                  int indexCapture, int variableCapture) noexcept {
        int index = match.captured(indexCapture).toInt();
        QByteArray variable = match.captured(variableCapture).toUtf8();

        auto it = bindings.find(variable);
        if (it == bindings.end()) {
            // 1. Check if the index is already used
            if (assignedBindings.test(index)) {
                index = getFirstAvailableBit(assignedBindings);
                if (index == -1) {
                    return;
                }

                const int indexStartOffset = match.capturedStart(indexCapture);
                const int indexEndOffset = match.capturedEnd(indexCapture);
                const int indexLength = indexEndOffset - indexStartOffset;
                code.replace(indexStartOffset, indexLength, QByteArray::number(index));
            }

            assignedBindings.set(index);
            bindings.emplace(std::move(variable), index);
        } else {
            int indexToUse = it->second;
            const int indexStartOffset = match.capturedStart(indexCapture);
            const int indexEndOffset = match.capturedEnd(indexCapture);
            const int indexLength = indexEndOffset - indexStartOffset;
            code.replace(indexStartOffset, indexLength, QByteArray::number(indexToUse));
        }
        // This may fail in the case where the replaced offset is an incredibly long number,
        // which seems quite unlikely
        offset = match.capturedEnd(0);
    };

    for (QByteArray &shaderCode : shaderCodes) {
        // Since QRegularExpression::match takes a QString anyway, convert once beforehand
        QString shaderString = shaderCode;

        // Regex for the sampler variables
        int offset = 0;
        auto match = samplerRegex.match(shaderString, offset);
        while (match.hasMatch()) {
            const int indexCapture = 1;
            const int variableCapture = 2;
            replaceBinding(offset, match, shaderString, indexCapture, variableCapture);

            match = samplerRegex.match(shaderString, offset);
        }

        // Regex for the UBOs
        offset = 0;
        match = uboRegex.match(shaderString, offset);
        while (match.hasMatch()) {
            const int indexCapture = !match.capturedView(1).isEmpty() ? 1 : 2;
            const int variableCapture = 3;
            replaceBinding(offset, match, shaderString, indexCapture, variableCapture);

            match = uboRegex.match(shaderString, offset);
        }

        shaderCode = shaderString.toUtf8();
    }
}

int glslVersionForFormat(const QSurfaceFormat &format) noexcept
{
    const int major = format.majorVersion();
    const int minor = format.minorVersion();

    static const QHash<std::pair<int, int>, int> glVersionToGLSLVersion = {
        { { 4, 6 }, 460 }, { { 4, 5 }, 450 }, { { 4, 4 }, 440 }, { { 4, 3 }, 430 },
        { { 4, 2 }, 420 }, { { 4, 1 }, 410 }, { { 4, 0 }, 400 }, { { 3, 3 }, 330 },
        { { 3, 2 }, 150 }, { { 3, 2 }, 120 }, { { 3, 1 }, 120 },
    };

    const auto it = glVersionToGLSLVersion.find({ major, minor });
    if (it == glVersionToGLSLVersion.end()) {
        if (major < 3) {
            return 120;
        } else {
            return major * 100 + minor * 10;
        }
    } else {
        return *it;
    }
}
}

// Called by GL Command Thread
SubmissionContext::ShaderCreationInfo SubmissionContext::createShaderProgram(RHIShader *shader)
{
    // Compile shaders
    const auto &shaderCode = shader->shaderCode();
    QShaderBaker b;
    b.setGeneratedShaders({
            { QShader::SpirvShader, 100 },
#ifndef QT_NO_OPENGL
            { QShader::GlslShader, glslVersionForFormat(format()) },
#endif
            { QShader::HlslShader, QShaderVersion(50) },
            { QShader::MslShader, QShaderVersion(12) },
    });

    b.setGeneratedShaderVariants({ QShader::Variant {},
#ifndef QT_NO_OPENGL
                                   QShader::Variant {},
#endif
                                   QShader::Variant {}, QShader::Variant {} });

    // TODO handle caching as QShader does not have a built-in mechanism for that
    QString logs;
    bool success = true;
    for (int i = QShaderProgram::Vertex; i <= QShaderProgram::Compute; ++i) {
        const QShaderProgram::ShaderType type = static_cast<QShaderProgram::ShaderType>(i);
        if (!shaderCode.at(i).isEmpty()) {
            // Note: logs only return the error but not all the shader code
            // we could append it

            const auto rhiStage = rhiShaderStage(type);
            b.setSourceString(shaderCode.at(i), rhiStage);
            QShader bakedShader = b.bake();
            if (b.errorMessage() != QString() || !bakedShader.isValid()) {
                qDebug() << "Shader Error: " << b.errorMessage() << shaderCode.at(i).data()
                         << rhiStage;
                logs += b.errorMessage();
                success = false;
            }
            shader->m_stages[rhiStage] = std::move(bakedShader);
        }
    }

    // Perform shader introspection
    if (success)
        shader->introspect();

    return { success, logs };
}

// Called by Renderer::updateResources
void SubmissionContext::loadShader(Shader *shaderNode, ShaderManager *shaderManager,
                                   RHIShaderManager *rhiShaderManager)
{
    const Qt3DCore::QNodeId shaderId = shaderNode->peerId();
    RHIShader *rhiShader = rhiShaderManager->lookupResource(shaderId);

    // We already have a shader associated with the node
    if (rhiShader != nullptr) {
        // We need to abandon it
        rhiShaderManager->abandon(rhiShader, shaderNode);
    }

    // We create or adopt an already created rhiShader
    rhiShader = rhiShaderManager->createOrAdoptExisting(shaderNode);

    const QVector<Qt3DCore::QNodeId> sharedShaderIds =
            rhiShaderManager->shaderIdsForProgram(rhiShader);
    if (sharedShaderIds.size() == 1) {
        // Shader in the cache hasn't been loaded yet
        QVector<QByteArray> shaderCodes = shaderNode->shaderCode();
        preprocessRHIShader(shaderCodes);
        rhiShader->setShaderCode(shaderCodes);

        const ShaderCreationInfo loadResult = createShaderProgram(rhiShader);
        shaderNode->setStatus(loadResult.linkSucceeded ? QShaderProgram::Ready
                                                       : QShaderProgram::Error);
        shaderNode->setLog(loadResult.logs);
        // Loaded in the sense we tried to load it (and maybe it failed)
        rhiShader->setLoaded(true);
    } else {
        // Find an already loaded shader that shares the same QShaderProgram
        for (const Qt3DCore::QNodeId &sharedShaderId : sharedShaderIds) {
            if (sharedShaderId != shaderNode->peerId()) {
                Shader *refShader = shaderManager->lookupResource(sharedShaderId);
                // We only introspect once per actual OpenGL shader program
                // rather than once per ShaderNode.
                shaderNode->initializeFromReference(*refShader);
                break;
            }
        }
    }
    shaderNode->unsetDirty();
    // Ensure we will rebuilt material caches
    shaderNode->requestCacheRebuild();
}

const GraphicsApiFilterData *SubmissionContext::contextInfo() const
{
    return &m_contextInfo;
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender of namespace

QT_END_NAMESPACE
