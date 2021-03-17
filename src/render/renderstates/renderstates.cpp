/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "renderstates_p.h"

#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qpointsize.h>

#include <Qt3DRender/private/qstenciloperation_p.h>
#include <Qt3DRender/private/qstenciltest_p.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qalphatest.h>
#include <Qt3DRender/qdepthrange.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qrastermode.h>
#include <Qt3DRender/qfrontface.h>
#include <Qt3DRender/qscissortest.h>
#include <Qt3DRender/qpolygonoffset.h>
#include <Qt3DRender/qcolormask.h>
#include <Qt3DRender/qclipplane.h>
#include <Qt3DRender/qstencilmask.h>
#include <Qt3DRender/qlinewidth.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

void RenderStateImpl::updateProperties(const QRenderState *)
{
}

void BlendEquationArguments::updateProperties(const QRenderState *node)
{
    const QBlendEquationArguments *args = static_cast<const QBlendEquationArguments *>(node);

    std::get<0>(m_values) = args->sourceRgb();
    std::get<1>(m_values) = args->destinationRgb();
    std::get<2>(m_values) = args->sourceAlpha();
    std::get<3>(m_values) = args->destinationAlpha();
    std::get<4>(m_values) = args->isEnabled();
    std::get<5>(m_values) = args->bufferIndex();
}

void BlendEquation::updateProperties(const QRenderState *node)
{
    const QBlendEquation *equation = static_cast<const QBlendEquation *>(node);
    std::get<0>(m_values) = equation->blendFunction();
}

void AlphaFunc::updateProperties(const QRenderState *node)
{
    const QAlphaTest *alphaTest = static_cast<const QAlphaTest *>(node);
    std::get<0>(m_values) = alphaTest->alphaFunction();
    std::get<1>(m_values) = alphaTest->referenceValue();
}

void MSAAEnabled::updateProperties(const QRenderState *node)
{
    std::get<0>(m_values) = node->isEnabled();
}

void DepthRange::updateProperties(const QRenderState *node)
{
    const QDepthRange *depthRange = static_cast<const QDepthRange *>(node);

    std::get<0>(m_values) = depthRange->nearValue();
    std::get<1>(m_values) = depthRange->farValue();
}

void DepthTest::updateProperties(const QRenderState *node)
{
    const QDepthTest *depthTest = static_cast<const QDepthTest *>(node);

    std::get<0>(m_values) = depthTest->depthFunction();
}

void RasterMode::updateProperties(const QRenderState *node)
{
    const QRasterMode *rasterMode = static_cast<const QRasterMode *>(node);

    std::get<0>(m_values) = rasterMode->faceMode();
    std::get<1>(m_values) = rasterMode->rasterMode();
}

void CullFace::updateProperties(const QRenderState *node)
{
    const QCullFace *cullFace = static_cast<const QCullFace *>(node);

    std::get<0>(m_values) = cullFace->mode();
}

void FrontFace::updateProperties(const QRenderState *node)
{
    const QFrontFace *frontFace = static_cast<const QFrontFace *>(node);

    std::get<0>(m_values) = frontFace->direction();
}

void NoDepthMask::updateProperties(const QRenderState *node)
{
    std::get<0>(m_values) = !node->isEnabled();
}

void ScissorTest::updateProperties(const QRenderState *node)
{
    const QScissorTest *scissorTest = static_cast<const QScissorTest *>(node);

    std::get<0>(m_values) = scissorTest->left();
    std::get<1>(m_values) = scissorTest->bottom();
    std::get<2>(m_values) = scissorTest->width();
    std::get<3>(m_values) = scissorTest->height();
}

void StencilTest::updateProperties(const QRenderState *node)
{
    const QStencilTest *stencilTest = static_cast<const QStencilTest *>(node);
    std::get<0>(m_values) = stencilTest->front()->stencilFunction();
    std::get<1>(m_values) = stencilTest->front()->referenceValue();
    std::get<2>(m_values) = stencilTest->front()->comparisonMask();
    std::get<3>(m_values) = stencilTest->back()->stencilFunction();
    std::get<4>(m_values) = stencilTest->back()->referenceValue();
    std::get<5>(m_values) = stencilTest->back()->comparisonMask();
}

void PointSize::updateProperties(const QRenderState *node)
{
    const QPointSize *pointSize = static_cast<const QPointSize *>(node);

    std::get<0>(m_values) = (pointSize->sizeMode() == QPointSize::Programmable);
    std::get<1>(m_values) = pointSize->value();
}

void PolygonOffset::updateProperties(const QRenderState *node)
{
    const QPolygonOffset *offset = static_cast<const QPolygonOffset *>(node);

    std::get<0>(m_values) = offset->scaleFactor();
    std::get<1>(m_values) = offset->depthSteps();
}

void ColorMask::updateProperties(const QRenderState *node)
{
    const QColorMask *colorMask = static_cast<const QColorMask *>(node);

    std::get<0>(m_values) = colorMask->isRedMasked();
    std::get<1>(m_values) = colorMask->isGreenMasked();
    std::get<2>(m_values) = colorMask->isBlueMasked();
    std::get<3>(m_values) = colorMask->isAlphaMasked();
}

void ClipPlane::updateProperties(const QRenderState *node)
{
    const QClipPlane *clipPlane = static_cast<const QClipPlane *>(node);

    std::get<0>(m_values) = clipPlane->planeIndex();
    std::get<1>(m_values) = clipPlane->normal();
    std::get<2>(m_values) = clipPlane->distance();
}

void StencilOp::updateProperties(const QRenderState *node)
{
    const QStencilOperation *stencilOp = static_cast<const QStencilOperation *>(node);

    std::get<0>(m_values) = stencilOp->front()->stencilTestFailureOperation();
    std::get<1>(m_values) = stencilOp->front()->depthTestFailureOperation();
    std::get<2>(m_values) = stencilOp->front()->allTestsPassOperation();
    std::get<3>(m_values) = stencilOp->back()->stencilTestFailureOperation();
    std::get<4>(m_values) = stencilOp->back()->depthTestFailureOperation();
    std::get<5>(m_values) = stencilOp->back()->allTestsPassOperation();
}

void StencilMask::updateProperties(const QRenderState *node)
{
    const QStencilMask *stencilMask = static_cast<const QStencilMask *>(node);
    std::get<0>(m_values) = stencilMask->frontOutputMask();
    std::get<1>(m_values) = stencilMask->backOutputMask();
}

void LineWidth::updateProperties(const QRenderState *node)
{
    const QLineWidth *lineWidth = static_cast<const QLineWidth *>(node);
    std::get<0>(m_values) = lineWidth->value();
    std::get<1>(m_values) = lineWidth->smooth();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
