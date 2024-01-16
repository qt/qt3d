// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "submissioncontext_p.h"

#include <Qt3DCore/private/qbuffer_p.h>
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
#include <Qt3DRender/private/stringtoint_p.h>
#if QT_CONFIG(qt3d_vulkan) && QT_CONFIG(vulkan)
#  include <Qt3DRender/private/vulkaninstance_p.h>
#endif
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
#include <rhi/qrhi.h>
#include <rhi/qshaderbaker.h>

#include <bitset>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Rhi {

namespace {

//RHIBuffer::Type attributeTypeToGLBufferType(QAttribute::AttributeType type) noexcept
//{
//    switch (type) {
//    case QAttribute::VertexAttribute:
//        return RHIBuffer::ArrayBuffer;
//    case QAttribute::IndexAttribute:
//        return RHIBuffer::IndexBuffer;
//    case QAttribute::DrawIndirectAttribute:
//        return RHIBuffer::DrawIndirectBuffer;
//    default:
//        Q_UNREACHABLE();
//    }
//}

//void copyGLFramebufferDataToImage(QImage &img, const uchar *srcData, uint stride, uint width,
//                                  uint height, QAbstractTexture::TextureFormat format) noexcept
//{
//    switch (format) {
//    case QAbstractTexture::RGBA32F: {
//        uchar *srcScanline = const_cast<uchar *>(srcData) + stride * (height - 1);
//        for (uint i = 0; i < height; ++i) {
//            uchar *dstScanline = img.scanLine(i);
//            float *pSrc = reinterpret_cast<float *>(srcScanline);
//            for (uint j = 0; j < width; j++) {
//                *dstScanline++ = (uchar)(255.0f * qBound(0.0f, pSrc[4 * j + 2], 1.0f));
//                *dstScanline++ = (uchar)(255.0f * qBound(0.0f, pSrc[4 * j + 1], 1.0f));
//                *dstScanline++ = (uchar)(255.0f * qBound(0.0f, pSrc[4 * j + 0], 1.0f));
//                *dstScanline++ = (uchar)(255.0f * qBound(0.0f, pSrc[4 * j + 3], 1.0f));
//            }
//            srcScanline -= stride;
//        }
//    } break;
//    default: {
//        uchar *srcScanline = (uchar *)srcData + stride * (height - 1);
//        for (uint i = 0; i < height; ++i) {
//            memcpy(img.scanLine(i), srcScanline, stride);
//            srcScanline -= stride;
//        }
//    } break;
//    }
//}

// Render States Helpers

template<typename GenericState>
void applyStateHelper(const GenericState *state, QRhiGraphicsPipeline *gp) noexcept
{
    Q_UNUSED(state);
    Q_UNUSED(gp);
    qCWarning(Backend) << "RHI Unhandled render state" << typeid(GenericState).name();
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
        default:
            return QRhiGraphicsPipeline::Add;
        }
    };

    targetBlend.enable = true;
    targetBlend.opAlpha = getRHIBlendOp(equation);
    gp->setTargetBlends({ targetBlend });
}

void applyStateHelper(const MSAAEnabled *state, QRhiGraphicsPipeline *gp,
                      const QSurfaceFormat &format) noexcept
{
    Q_UNUSED(state);
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
        qCWarning(Backend) << "RHI doesn't handle FrontAndBack CullFace";
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

void applyStateHelper(const PolygonOffset *state, QRhiGraphicsPipeline *gp)
{
    const auto values = state->values();
    auto slopeScaledDepthBias = std::get<0>(values);
    auto depthBias = std::get<1>(values);
    gp->setSlopeScaledDepthBias(slopeScaledDepthBias);
    gp->setDepthBias(depthBias);
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

void applyStateHelper(const LineWidth *state, QRhiGraphicsPipeline *gp) noexcept
{
    const auto values = state->values();
    gp->setLineWidth(std::get<0>(values));
    // no GL_LINE_SMOOTH equivalent on RHI
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
    : m_initialized(false),
      m_ownsRhiCtx(false),
      m_drivenExternally(false),
      m_material(nullptr),
      m_renderer(nullptr),
      m_rhi(nullptr),
      m_currentSwapChain(nullptr),
      m_currentRenderPassDescriptor(nullptr),
      m_defaultRenderTarget(nullptr),
      m_defaultCommandBuffer(nullptr)
#ifndef QT_NO_OPENGL
      ,
      m_fallbackSurface(nullptr)
#endif
{
    m_contextInfo.m_api = QGraphicsApiFilter::RHI;

    // We set those version numbers because QShaderGenerator wants major > 0
    m_contextInfo.m_major = 1;
    m_contextInfo.m_minor = 0;
}

SubmissionContext::~SubmissionContext()
{
    releaseResources();
}

void SubmissionContext::initialize()
{
    m_initialized = true;

    // If the RHI instance was already set (Scene3D)
    // no point in continuing below;
    if (m_rhi)
        return;

    m_ownsRhiCtx = true;

    Qt3DRender::API requestedApi = Qt3DRender::API::RHI;
    const auto userRequestedApi = qgetenv("QSG_RHI_BACKEND").toLower();
    if (!userRequestedApi.isEmpty()) {
        if (userRequestedApi == QByteArrayLiteral("opengl") ||
            userRequestedApi == QByteArrayLiteral("gl") ||
            userRequestedApi == QByteArrayLiteral("gles2")) {
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

    // If nothing specified, deduce best backend API based on platform
    if (requestedApi == Qt3DRender::API::RHI) {
#if defined(Q_OS_WIN)
        requestedApi = Qt3DRender::API::DirectX;
#elif defined(Q_OS_MACOS) || defined(Q_OS_IOS)
        requestedApi = Qt3DRender::API::Metal;
#elif QT_CONFIG(opengl)
        requestedApi = Qt3DRender::API::OpenGL;
#else
        requestedApi = Qt3DRender::API::Vulkan;
#endif
    }

    QRhi::Flags rhiFlags = QRhi::EnableDebugMarkers;

#if QT_CONFIG(qt3d_vulkan) && QT_CONFIG(vulkan)
    if (requestedApi == Qt3DRender::API::Vulkan) {
        QRhiVulkanInitParams params;
        params.inst = &Qt3DRender::staticVulkanInstance();
        m_rhi = QRhi::create(QRhi::Vulkan, &params, rhiFlags);
        qCWarning(Backend) << "Initializing RHI with Vulkan backend";
    }
#endif

#ifdef Q_OS_WIN
    if (requestedApi == Qt3DRender::API::DirectX) {
        QRhiD3D11InitParams params;
        params.enableDebugLayer = true;
        m_rhi = QRhi::create(QRhi::D3D11, &params, rhiFlags);
        qCWarning(Backend) << "Initializing RHI with DirectX backend";
    }
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
    if (requestedApi == Qt3DRender::API::Metal) {
        QRhiMetalInitParams params;
        m_rhi = QRhi::create(QRhi::Metal, &params, rhiFlags);
        qCWarning(Backend) << "Initializing RHI with Metal backend";
    }
#endif
    if (requestedApi == Qt3DRender::API::Null) {
        QRhiInitParams params;
        m_rhi = QRhi::create(QRhi::Null, &params, rhiFlags);
        qCWarning(Backend) << "Initializing RHI with Null backend";
    }

    if (requestedApi != Qt3DRender::API::OpenGL && m_rhi == nullptr) {
        qCWarning(Backend) << "RHI: Unable to use requested RHI Api, trying to fall back on OpenGL";
        requestedApi = Qt3DRender::API::OpenGL;
    }

    if (requestedApi == Qt3DRender::API::OpenGL) {
#ifndef QT_NO_OPENGL
        m_fallbackSurface = QRhiGles2InitParams::newFallbackSurface();
        QRhiGles2InitParams params;
        params.format = QSurfaceFormat::defaultFormat();
        params.fallbackSurface = m_fallbackSurface;
        m_rhi = QRhi::create(QRhi::OpenGLES2, &params, rhiFlags);
        qCWarning(Backend) << "Initializing RHI with OpenGL backend";
#else
        qCWarning(Backend) << "RHI: OpenGL not supported";
#endif
    }

    Q_ASSERT(m_rhi != nullptr);
}

void SubmissionContext::setDrivenExternally(bool drivenExternally)
{
    Q_ASSERT(!m_initialized);
    m_drivenExternally = drivenExternally;
}

bool SubmissionContext::drivenExternally() const
{
    return m_drivenExternally;
}

bool SubmissionContext::beginDrawing(QSurface *surface)
{
    Q_ASSERT(surface);

    Q_ASSERT(isInitialized());

    // In the Scene3D case it does not make sense to create SwapChains as we
    // can only record commands that will be used against QtQuick default's
    // swap chain. Also when rendering through Scene3D, QtQuick takes care of
    // beginning the frame
    if (m_drivenExternally)
        return true;

    // Check if we have a swapchain for the Window, if not create one
    SwapChainInfo *swapChainInfo = swapChainForSurface(surface);
    QRhiSwapChain *swapChain = swapChainInfo->swapChain;

    // Resize swapchain if needed
    if (surface->size() != swapChain->currentPixelSize()) {
        bool couldRebuild = swapChain->createOrResize();
        if (!couldRebuild)
            return false;
    }

    m_currentSwapChain = swapChain;
    m_currentRenderPassDescriptor = swapChainInfo->renderPassDescriptor;

    const auto success = m_rhi->beginFrame(m_currentSwapChain);
    return success == QRhi::FrameOpSuccess;
}

void SubmissionContext::endDrawing(bool swapBuffers)
{
    Q_UNUSED(swapBuffers);
    const bool shouldEndFrame = !m_drivenExternally;
    // When rendering through Scene3D, QtQuick takes care of ending the frame
    if (shouldEndFrame)
        m_rhi->endFrame(m_currentSwapChain, {});
}

QImage SubmissionContext::readFramebuffer(const QRect &rect)
{
    Q_UNUSED(rect);
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

    if (m_currentUpdates) {
        m_currentUpdates->release();
        m_currentUpdates = nullptr;
    }

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

        // Only destroy RHI context if we created it
        if (m_ownsRhiCtx)
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

// Called when Scene3D is used
void SubmissionContext::setRHIContext(QRhi *ctx)
{
    m_rhi = ctx;
}

// Scene3D
void SubmissionContext::setDefaultRenderTarget(QRhiRenderTarget *target)
{
    m_defaultRenderTarget = target;
}

// Scene3D
void SubmissionContext::setCommandBuffer(QRhiCommandBuffer *commandBuffer)
{
    m_defaultCommandBuffer = commandBuffer;
}

void SubmissionContext::bindFrameBufferAttachmentHelper(GLuint fboId,
                                                        const AttachmentPack &attachments)
{
    Q_UNUSED(fboId);
    Q_UNUSED(attachments);
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
    Q_UNUSED(attachments);
    RHI_UNIMPLEMENTED;
    //* const std::vector<int> activeDrawBuffers = attachments.getGlDrawBuffers();
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
    QRhiGraphicsPipeline::Flags flags{};
    graphicsPipeline->setFlags(flags);

    graphicsPipeline->setDepthWrite(true);
    graphicsPipeline->setDepthTest(true);
    graphicsPipeline->setDepthOp(QRhiGraphicsPipeline::Less);

    graphicsPipeline->setCullMode(QRhiGraphicsPipeline::Back);

    graphicsPipeline->setSampleCount(format().samples());

    graphicsPipeline->setStencilTest(false);
    graphicsPipeline->setStencilReadMask(0xff);
    graphicsPipeline->setStencilWriteMask(0xff);

    QRhiGraphicsPipeline::StencilOpState stencilOp;
    stencilOp.failOp = QRhiGraphicsPipeline::Keep;
    stencilOp.depthFailOp = QRhiGraphicsPipeline::Keep;
    stencilOp.passOp = QRhiGraphicsPipeline::Keep;
    stencilOp.compareOp = QRhiGraphicsPipeline::Always;

    graphicsPipeline->setStencilFront(stencilOp);
    graphicsPipeline->setStencilBack(stencilOp);


    QRhiGraphicsPipeline::TargetBlend blend;
    blend.colorWrite = QRhiGraphicsPipeline::ColorMask(0xF); // R | G | B | A
    blend.enable = false;
    blend.srcColor = QRhiGraphicsPipeline::One;
    blend.dstColor = QRhiGraphicsPipeline::Zero;
    blend.srcAlpha = QRhiGraphicsPipeline::One;
    blend.dstAlpha = QRhiGraphicsPipeline::Zero;

    graphicsPipeline->setTargetBlends({blend});


    const auto &statesToSet = ss->states();
    for (const StateVariant &ds : statesToSet)
        applyState(ds, graphicsPipeline);
}

StateVariant *SubmissionContext::getState(RenderStateSet *ss, StateMask type) const
{
    const auto &statesToSet = ss->states();
    for (qsizetype i = 0, m = statesToSet.size(); i < m; ++i) {
        const StateVariant &ds = statesToSet.at(i);
        if (ds.type == type)
            return ss->states().data() + i;
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
        // Allow to read back from swap chain for RenderCapture to work
        swapChain->setFlags(QRhiSwapChain::Flags { QRhiSwapChain::UsedAsTransferSource });
        swapChain->setSampleCount(samples);

        QRhiRenderBuffer *renderBuffer =
                m_rhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil, QSize(), samples,
                                       QRhiRenderBuffer::UsedWithSwapChainOnly);
        swapChain->setDepthStencil(renderBuffer);

        QRhiRenderPassDescriptor *renderPassDescriptor =
                swapChain->newCompatibleRenderPassDescriptor();
        swapChain->setRenderPassDescriptor(renderPassDescriptor);

        // Build swapChain the first time
        if (swapChain->createOrResize()) {
            swapChainInfo.swapChain = swapChain;
            swapChainInfo.renderBuffer = renderBuffer;
            swapChainInfo.renderPassDescriptor = renderPassDescriptor;
        } else {
            swapChain->deleteLater();
            m_swapChains.remove(surface);
            return nullptr;
        }
    }
    return &swapChainInfo;
}

QRhiCommandBuffer *SubmissionContext::currentFrameCommandBuffer() const
{
    // When rendering with Scene3D, we have to use the Command Buffer provided by QtQuick
    // When Qt3D renders on its own, we use our own Command Buffer which we can
    // retrieve from the current Swap Chain
    if (m_defaultCommandBuffer)
        return m_defaultCommandBuffer;
    return m_currentSwapChain->currentFrameCommandBuffer();
}

QRhiRenderTarget *SubmissionContext::currentFrameRenderTarget() const
{
    return m_currentSwapChain->currentFrameRenderTarget();
}

QRhiRenderTarget *SubmissionContext::defaultRenderTarget() const
{
    return m_defaultRenderTarget;
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
        glBuff->destroy();
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

void SubmissionContext::uploadDataToRHIBuffer(Buffer *buffer, RHIBuffer *b)
{
    // If the buffer is dirty (hence being called here)
    // there are two possible cases
    // * setData was called changing the whole data or functor (or the usage pattern)
    // * partial buffer updates where received

    // Note: we are only storing the updates data CPU side at this point
    // actually upload will be performed when the buffer will be bound
    // as we would otherwise need to know the usage type of the buffer
    auto updates = Qt3DCore::moveAndClear(buffer->pendingBufferUpdates());

    if (updates.empty())
        qCWarning(Backend) << "Buffer has no data to upload";

    for (auto it = updates.begin(); it != updates.end(); ++it) {
        auto update = it;
        // We have a partial update
        if (update->offset >= 0) {
            // accumulate sequential updates as single one
            qsizetype bufferSize = update->data.size();
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
            b->update(update->data, update->offset);
        } else {
            // We have an update that was done by calling QBuffer::setData
            // which is used to resize or entirely clear the buffer
            // Note: we use the buffer data directly in that case
            b->allocate(buffer->data(), false);
        }
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
    Q_UNUSED(inputRenderTargetId);
    Q_UNUSED(outputRenderTargetId);
    Q_UNUSED(inputRect);
    Q_UNUSED(outputRect);
    Q_UNUSED(defaultFboId);
    Q_UNUSED(inputAttachmentPoint);
    Q_UNUSED(outputAttachmentPoint);
    Q_UNUSED(interpolationMethod);
    RHI_UNIMPLEMENTED;
}

namespace {
template<std::size_t N>
constexpr int getFirstAvailableBit(const std::bitset<N> &bits)
{
    for (std::size_t i = 0; i < N; i++) {
        if (!bits.test(i))
            return int(i);
    }
    return -1;
}
// This function ensures that the shader stages all have the same bindings
void preprocessRHIShader(std::vector<QByteArray> &shaderCodes)
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
                                  qsizetype &offset, QRegularExpressionMatch &match, QString &code,
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

                const qsizetype indexStartOffset = match.capturedStart(indexCapture);
                const qsizetype indexEndOffset = match.capturedEnd(indexCapture);
                const qsizetype indexLength = indexEndOffset - indexStartOffset;
                code.replace(indexStartOffset, indexLength, QByteArray::number(index));
            }

            assignedBindings.set(index);
            bindings.emplace(std::move(variable), index);
        } else {
            int indexToUse = it->second;
            const qsizetype indexStartOffset = match.capturedStart(indexCapture);
            const qsizetype indexEndOffset = match.capturedEnd(indexCapture);
            const qsizetype indexLength = indexEndOffset - indexStartOffset;
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
        qsizetype offset = 0;
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

QShaderVersion glslVersionForFormat(const QSurfaceFormat &format) noexcept
{
    const int major = format.majorVersion();
    const int minor = format.minorVersion();
    const auto type = format.renderableType();

    if (type != QSurfaceFormat::OpenGLES) {
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
    else {
        static const QHash<std::pair<int, int>, int> glVersionToGLSLVersion = {
            { { 3, 2 }, 320 }, { { 3, 1 }, 310 }, { { 3, 0 }, 300 },
        };

        const auto it = glVersionToGLSLVersion.find({ major, minor });
        if (it == glVersionToGLSLVersion.end()) {
            if (major < 3) {
                return {100, QShaderVersion::GlslEs};
            } else {
                return {major * 100 + minor * 10, QShaderVersion::GlslEs};
            }
        } else {
            return {*it, QShaderVersion::GlslEs};
        }
    }
}
}

// Called by GL Command Thread
SubmissionContext::ShaderCreationInfo SubmissionContext::createShaderProgram(RHIShader *shader)
{
    // Compile shaders
    const auto &shaderCode = shader->shaderCode();
    QShaderBaker b;
    QList<QShaderBaker::GeneratedShader> generatedShaders;

#if QT_FEATURE_vulkan
    if (m_rhi->backend() == QRhi::Vulkan)
        generatedShaders.emplace_back(QShader::SpirvShader, 100);
#endif

#ifndef QT_NO_OPENGL
    if (m_rhi->backend() == QRhi::OpenGLES2)
        generatedShaders.emplace_back(QShader::GlslShader, glslVersionForFormat(format()));
#endif

#ifdef Q_OS_WIN
    if (m_rhi->backend() == QRhi::D3D11)
        generatedShaders.emplace_back(QShader::HlslShader, QShaderVersion(50));
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
    if (m_rhi->backend() == QRhi::Metal)
        generatedShaders.emplace_back(QShader::MslShader, QShaderVersion(12));
#endif

    QList<QShader::Variant> generatedShaderVariants(generatedShaders.size());

    b.setGeneratedShaders(generatedShaders);
    b.setGeneratedShaderVariants(generatedShaderVariants);

    // TODO handle caching as QShader does not have a built-in mechanism for that
    QString logs;
    bool success = true;
    for (size_t i = QShaderProgram::Vertex; i <= QShaderProgram::Compute; ++i) {
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

    const std::vector<Qt3DCore::QNodeId> &sharedShaderIds =
            rhiShaderManager->shaderIdsForProgram(rhiShader);
    if (sharedShaderIds.size() == 1) {
        {
            // Shader in the cache hasn't been loaded yet
            // We want a copy of the QByteArray as preprocessRHIShader will
            // modify them
            std::vector<QByteArray> shaderCodes = shaderNode->shaderCode();
            preprocessRHIShader(shaderCodes);
            rhiShader->setShaderCode(std::move(shaderCodes));
        }

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
