/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderstatenode_p.h"
#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>

#include <Qt3DRender/qalphacoverage.h>
#include <Qt3DRender/qalphatest.h>
#include <Qt3DRender/private/qalphatest_p.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/private/qblendequation_p.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/private/qblendequationarguments_p.h>
#include <Qt3DRender/qcolormask.h>
#include <Qt3DRender/private/qcolormask_p.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/private/qcullface_p.h>
#include <Qt3DRender/qnodepthmask.h>
#include <Qt3DRender/qdepthrange.h>
#include <Qt3DRender/private/qdepthrange_p.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/private/qdepthtest_p.h>
#include <Qt3DRender/qrastermode.h>
#include <Qt3DRender/private/qrastermode_p.h>
#include <Qt3DRender/qdithering.h>
#include <Qt3DRender/qfrontface.h>
#include <Qt3DRender/private/qfrontface_p.h>
#include <Qt3DRender/qpointsize.h>
#include <Qt3DRender/private/qpointsize_p.h>
#include <Qt3DRender/qpolygonoffset.h>
#include <Qt3DRender/private/qpolygonoffset_p.h>
#include <Qt3DRender/qscissortest.h>
#include <Qt3DRender/private/qscissortest_p.h>
#include <Qt3DRender/qstenciltest.h>
#include <Qt3DRender/private/qstenciltest_p.h>
#include <Qt3DRender/qstenciltestarguments.h>
#include <Qt3DRender/private/qstenciltestarguments_p.h>
#include <Qt3DRender/qclipplane.h>
#include <Qt3DRender/private/qclipplane_p.h>
#include <Qt3DRender/qmultisampleantialiasing.h>
#include <Qt3DRender/qseamlesscubemap.h>
#include <Qt3DRender/qstenciloperation.h>
#include <Qt3DRender/private/qstenciloperation_p.h>
#include <Qt3DRender/qstenciloperationarguments.h>
#include <Qt3DRender/private/qstenciloperationarguments_p.h>
#include <Qt3DRender/qstencilmask.h>
#include <Qt3DRender/private/qstencilmask_p.h>
#include <Qt3DRender/qlinewidth.h>
#include <Qt3DRender/private/qlinewidth_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

StateVariant createStateImplementation(const QRenderState *node)
{
    const QRenderStatePrivate *d = static_cast<const QRenderStatePrivate *>(Qt3DCore::QNodePrivate::get(node));
    switch (d->m_type) {

    case AlphaCoverageStateMask: {
        return StateVariant::createState<AlphaCoverage>();
    }

    case AlphaTestMask: {
        const QAlphaTest *alphaTest = static_cast<const QAlphaTest *>(node);
        return StateVariant::createState<AlphaFunc>(alphaTest->alphaFunction(), alphaTest->referenceValue());
    }

    case BlendStateMask: {
        const QBlendEquation *blendEquation = static_cast<const QBlendEquation *>(node);
        return StateVariant::createState<BlendEquation>(blendEquation->blendFunction());
    }

    case BlendEquationArgumentsMask: {
        const QBlendEquationArguments *blendArgs = static_cast<const QBlendEquationArguments *>(node);
        return StateVariant::createState<BlendEquationArguments>(
                    blendArgs->sourceRgb(), blendArgs->destinationRgb(),
                    blendArgs->sourceAlpha(), blendArgs->destinationAlpha(),
                    blendArgs->isEnabled(),
                    blendArgs->bufferIndex());
    }

    case MSAAEnabledStateMask: {
        return StateVariant::createState<MSAAEnabled>(node->isEnabled());
    }

    case CullFaceStateMask: {
        const QCullFace *cullFace = static_cast<const QCullFace *>(node);
        return StateVariant::createState<CullFace>(cullFace->mode());
    }

    case DepthRangeMask: {
        const QDepthRange *depthRange = static_cast<const QDepthRange *>(node);
        return StateVariant::createState<DepthRange>(depthRange->nearValue(), depthRange->farValue());
    }

    case DepthWriteStateMask: {
        return StateVariant::createState<NoDepthMask>(!node->isEnabled());
    }

    case DepthTestStateMask: {
        const QDepthTest *depthTest = static_cast<const QDepthTest *>(node);
        return StateVariant::createState<DepthTest>(depthTest->depthFunction());
    }

    case RasterModeMask: {
        const QRasterMode *rasterMode = static_cast<const QRasterMode *>(node);
        return StateVariant::createState<RasterMode>(rasterMode->faceMode(), rasterMode->rasterMode());
    }

    case FrontFaceStateMask: {
        const QFrontFace *frontFace = static_cast<const QFrontFace *>(node);
        return StateVariant::createState<FrontFace>(frontFace->direction());
    }

    case ScissorStateMask: {
        const QScissorTest *scissorTest = static_cast<const QScissorTest *>(node);
        return StateVariant::createState<ScissorTest>(scissorTest->left(), scissorTest->bottom(),
                                                      scissorTest->width(), scissorTest->height());
    }

    case StencilTestStateMask: {
        const QStencilTest *stencilTest = static_cast<const QStencilTest *>(node);
        return StateVariant::createState<StencilTest>(stencilTest->front()->stencilFunction(),
                                                      stencilTest->front()->referenceValue(),
                                                      stencilTest->front()->comparisonMask(),
                                                      stencilTest->back()->stencilFunction(),
                                                      stencilTest->back()->referenceValue(),
                                                      stencilTest->back()->comparisonMask());
    }

    case PointSizeMask: {
        const QPointSize *pointSize = static_cast<const QPointSize *>(node);
        const bool isProgrammable = (pointSize->sizeMode() == QPointSize::Programmable);
        return StateVariant::createState<PointSize>(isProgrammable, pointSize->value());
    }

    case PolygonOffsetStateMask: {
        const QPolygonOffset *offset = static_cast<const QPolygonOffset *>(node);
        return StateVariant::createState<PolygonOffset>(offset->scaleFactor(), offset->depthSteps());
    }

    case ColorStateMask: {
        const QColorMask *colorMask = static_cast<const QColorMask *>(node);
        return StateVariant::createState<ColorMask>(colorMask->isRedMasked(), colorMask->isGreenMasked(),
                                                    colorMask->isBlueMasked(), colorMask->isAlphaMasked());
    }

    case ClipPlaneMask: {
        const QClipPlane *clipPlane = static_cast<const QClipPlane *>(node);
        return StateVariant::createState<ClipPlane>(clipPlane->planeIndex(),
                                                    clipPlane->normal(),
                                                    clipPlane->distance());
    }

    case SeamlessCubemapMask: {
        return StateVariant::createState<SeamlessCubemap>();
    }

    case StencilOpMask: {
        const QStencilOperation *stencilOp = static_cast<const QStencilOperation *>(node);
        return StateVariant::createState<StencilOp>(stencilOp->front()->stencilTestFailureOperation(),
                                                    stencilOp->front()->depthTestFailureOperation(),
                                                    stencilOp->front()->allTestsPassOperation(),
                                                    stencilOp->back()->stencilTestFailureOperation(),
                                                    stencilOp->back()->depthTestFailureOperation(),
                                                    stencilOp->back()->allTestsPassOperation());
    }

    case StencilWriteStateMask: {
        const QStencilMask *stencilMask = static_cast<const QStencilMask *>(node);
        return StateVariant::createState<StencilMask>(stencilMask->frontOutputMask(),
                                                      stencilMask->backOutputMask());
    }

    case DitheringStateMask: {
        return StateVariant::createState<Dithering>();
    }

    case LineWidthMask: {
        const QLineWidth *lineWidth = static_cast<const QLineWidth *>(node);
        return StateVariant::createState<LineWidth>(lineWidth->value(), lineWidth->smooth());
    }

    default:
        Q_UNREACHABLE();
        return StateVariant();
    }
}

} // anonymous

RenderStateNode::RenderStateNode()
    : BackendNode()
{
}

RenderStateNode::~RenderStateNode()
{
    cleanup();
}

void RenderStateNode::cleanup()
{
}

void RenderStateNode::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QRenderState *node = qobject_cast<const QRenderState *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (firstTime)
        m_impl = createStateImplementation(node);

    m_impl.state()->updateProperties(node);
    markDirty(AbstractRenderer::AllDirty);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
