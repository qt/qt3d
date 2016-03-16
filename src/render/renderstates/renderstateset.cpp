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

#include "renderstateset_p.h"

#include <bitset>

#include <QDebug>
#include <QOpenGLContext>

#include <Qt3DRender/private/graphicscontext_p.h>
#include <Qt3DRender/private/renderstates_p.h>
#include <Qt3DRender/private/qrenderstate_p.h>

#include <Qt3DRender/qalphacoverage.h>
#include <Qt3DRender/qalphatest.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/qcolormask.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qnodepthmask.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qdithering.h>
#include <Qt3DRender/qfrontface.h>
#include <Qt3DRender/qpointsize.h>
#include <Qt3DRender/qpolygonoffset.h>
#include <Qt3DRender/qscissortest.h>
#include <Qt3DRender/qstenciltest.h>
#include <Qt3DRender/qstenciltestarguments.h>
#include <Qt3DRender/qclipplane.h>
#include <Qt3DRender/qseamlesscubemap.h>
#include <Qt3DRender/qstenciloperation.h>
#include <Qt3DRender/qstenciloperationarguments.h>
#include <Qt3DRender/qstencilmask.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

RenderStateSet::RenderStateSet()
    : m_stateMask(0)
    , m_cachedPrevious(0)
{
}

RenderStateSet::~RenderStateSet()
{
}

void RenderStateSet::addState(RenderStateImpl *ds)
{
    Q_ASSERT(ds);
    m_states.append(ds);
    m_stateMask |= ds->mask();
}

int RenderStateSet::changeCost(RenderStateSet *previousState)
{
    if (previousState == this)
        return 0;

    int cost = 0;

    // first, find cost of any resets
    StateMaskSet invOurState = ~stateMask();
    StateMaskSet stateToReset = previousState->stateMask() & invOurState;

    std::bitset<64> bs(stateToReset);
    cost += int(bs.count());

    // now, find out how many states we're changing
    Q_FOREACH (RenderStateImpl *ds, m_states) {
        // if the other state contains matching, then doesn't
        // contribute to cost at all
        if (previousState->contains(ds)) {
            continue;
        }

        // flat cost for now; could be replaced with a cost() method on
        // RenderState
        cost += 2;
    }

    return cost;
}

void RenderStateSet::apply(GraphicsContext *gc)
{
    RenderStateSet* previousStates = gc->currentStateSet();

    const StateMaskSet invOurState = ~stateMask();
    // generate a mask for each set bit in previous, where we do not have
    // the corresponding bit set.

    StateMaskSet stateToReset = 0;
    if (previousStates) {
        stateToReset = previousStates->stateMask() & invOurState;
        qCDebug(RenderStates) << "previous states " << QString::number(previousStates->stateMask(), 2);
    }
    qCDebug(RenderStates) << " current states " << QString::number(stateMask(), 2)  << "inverse " << QString::number(invOurState, 2) << " -> states to change:  " << QString::number(stateToReset, 2);

    resetMasked(stateToReset, gc);

    if (m_cachedPrevious && previousStates == m_cachedPrevious) {
        // state-change cache hit
        Q_FOREACH (RenderStateImpl *ds, m_cachedDeltaStates) {
            ds->apply(gc);
        }
    } else {
        // compute deltas and cache for next frame
        m_cachedDeltaStates.clear();
        m_cachedPrevious = previousStates;

        Q_FOREACH (RenderStateImpl *ds, m_states) {
            if (previousStates && previousStates->contains(ds)) {
                continue;
            }

            m_cachedDeltaStates.append(ds);
            ds->apply(gc);
        }
    }
}

StateMaskSet RenderStateSet::stateMask() const
{
    return m_stateMask;
}

void RenderStateSet::merge(RenderStateSet *other)
{
    m_stateMask |= other->stateMask();
}

void RenderStateSet::resetMasked(StateMaskSet maskOfStatesToReset, GraphicsContext *gc)
{
    // TO DO -> Call gcHelper methods instead of raw GL
    // QOpenGLFunctions shouldn't be used here directly
    QOpenGLFunctions *funcs = gc->openGLContext()->functions();

    if (maskOfStatesToReset & ScissorStateMask) {
        funcs->glDisable(GL_SCISSOR_TEST);
    }

    if (maskOfStatesToReset & BlendStateMask) {
        funcs->glDisable(GL_BLEND);
    }

    if (maskOfStatesToReset & StencilWriteStateMask) {
        funcs->glStencilMask(0);
    }

    if (maskOfStatesToReset & StencilTestStateMask) {
        funcs->glDisable(GL_STENCIL_TEST);
    }

    if (maskOfStatesToReset & DepthTestStateMask) {
        funcs->glDisable(GL_DEPTH_TEST);
    }

    if (maskOfStatesToReset & DepthWriteStateMask) {
        funcs->glDepthMask(GL_TRUE); // reset to default
    }

    if (maskOfStatesToReset & FrontFaceStateMask) {
        funcs->glFrontFace(GL_CCW); // reset to default
    }

    if (maskOfStatesToReset & CullFaceStateMask) {
        funcs->glDisable(GL_CULL_FACE);
    }

    if (maskOfStatesToReset & DitheringStateMask) {
        funcs->glDisable(GL_DITHER);
    }

    if (maskOfStatesToReset & AlphaCoverageStateMask) {
        gc->disableAlphaCoverage();
    }

    if (maskOfStatesToReset & PointSizeMask) {
        gc->pointSize(false, 1.0f);    // reset to default
    }

    if (maskOfStatesToReset & PolygonOffsetStateMask) {
        funcs->glDisable(GL_POLYGON_OFFSET_FILL);
    }

    if (maskOfStatesToReset & ColorStateMask) {
        funcs->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    if (maskOfStatesToReset & ClipPlaneMask) {
        GLint max = gc->maxClipPlaneCount();
        for (GLint i = 0; i < max; ++i)
            gc->disableClipPlane(i);
    }

    if (maskOfStatesToReset & SeamlessCubemapMask) {
        gc->setSeamlessCubemap(false);
    }

    if (maskOfStatesToReset & StencilOpMask) {
        funcs->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }
}

bool RenderStateSet::contains(RenderStateImpl *ds) const
{
    // trivial reject using the state mask bits
    if (!(ds->mask() & stateMask()))
        return false;

    Q_FOREACH (RenderStateImpl* rs, m_states) {
        if (ds->equalTo(*rs))
            return true;
    }

    return false;
}


RenderStateImpl* RenderStateImpl::getOrCreateState(QRenderState *renderState)
{
    switch (QRenderStatePrivate::get(renderState)->m_type) {
    case QRenderStatePrivate::AlphaTest: {
        QAlphaTest *alphaTest = static_cast<QAlphaTest *>(renderState);
        return getOrCreateRenderStateImpl<AlphaFunc>(alphaTest->alphaFunction(), alphaTest->referenceValue());
    }
    case QRenderStatePrivate::BlendEquation: {
        QBlendEquation *blendEquation = static_cast<QBlendEquation *>(renderState);
        return getOrCreateRenderStateImpl<BlendEquation>(blendEquation->blendFunction());
    }
    case QRenderStatePrivate::BlendEquationArguments: {
        QBlendEquationArguments *blendState = static_cast<QBlendEquationArguments *>(renderState);
        // just use the same values for RGB and Alpha
        return getOrCreateRenderStateImpl<BlendEquationArguments>(blendState->sourceRgb(), blendState->destinationRgb(),
                                       blendState->sourceRgb(), blendState->destinationRgb(),
                                       blendState->isEnabled(),
                                       blendState->bufferIndex());
    }
    case QRenderStatePrivate::BlendStateSeparate: {
        QBlendEquationArguments *blendState = static_cast<QBlendEquationArguments *>(renderState);
        return getOrCreateRenderStateImpl<BlendEquationArguments>(blendState->sourceRgb(), blendState->destinationRgb(),
                                       blendState->sourceAlpha(), blendState->destinationAlpha(),
                                       blendState->isEnabled(),
                                       blendState->bufferIndex());
    }
    case QRenderStatePrivate::CullFace: {
        QCullFace *cullFace = static_cast<QCullFace *>(renderState);
        return getOrCreateRenderStateImpl<CullFace>(cullFace->mode());
    }
    case QRenderStatePrivate::NoDepthMask: {
        return getOrCreateRenderStateImpl<NoDepthMask>(false);
    }
    case QRenderStatePrivate::DepthTest: {
        QDepthTest *depthTest = static_cast<QDepthTest *>(renderState);
        return getOrCreateRenderStateImpl<DepthTest>(depthTest->depthFunction());
    }
    case QRenderStatePrivate::AlphaCoverage:
    case QRenderStatePrivate::Dithering:
    case QRenderStatePrivate::FrontFace: {
        QFrontFace *frontFace = static_cast<QFrontFace *>(renderState);
        return getOrCreateRenderStateImpl<FrontFace>(frontFace->direction());
    }
    case QRenderStatePrivate::ScissorTest: {
        QScissorTest *scissorTest = static_cast<QScissorTest *>(renderState);
        return getOrCreateRenderStateImpl<ScissorTest>(scissorTest->left(),
                                        scissorTest->bottom(),
                                        scissorTest->width(),
                                        scissorTest->height());
    }
    case QRenderStatePrivate::StencilTest: {
        QStencilTest *stencilTest = static_cast<QStencilTest *>(renderState);
        return getOrCreateRenderStateImpl<StencilTest>(stencilTest->front()->stencilFunction(),
                                        stencilTest->front()->referenceValue(),
                                        stencilTest->front()->comparisonMask(),
                                        stencilTest->back()->stencilFunction(),
                                        stencilTest->back()->referenceValue(),
                                        stencilTest->back()->comparisonMask());
    }
    case QRenderStatePrivate::PointSize: {
        QPointSize *pointSize = static_cast<QPointSize *>(renderState);
        const bool isProgrammable = (pointSize->sizeMode() == QPointSize::Programmable);
        return getOrCreateRenderStateImpl<PointSize>(isProgrammable, pointSize->value());
    }
    case QRenderStatePrivate::PolygonOffset: {
        QPolygonOffset *polygonOffset = static_cast<QPolygonOffset *>(renderState);
        return getOrCreateRenderStateImpl<PolygonOffset>(polygonOffset->scaleFactor(),
                                          polygonOffset->depthSteps());
    }
    case QRenderStatePrivate::ColorMask: {
        QColorMask *colorMask = static_cast<QColorMask *>(renderState);
        return getOrCreateRenderStateImpl<ColorMask>(colorMask->isRedMasked(),
                                      colorMask->isGreenMasked(),
                                      colorMask->isBlueMasked(),
                                      colorMask->isAlphaMasked());
    }
    case QRenderStatePrivate::ClipPlane: {
        QClipPlane *clipPlane = static_cast<QClipPlane *>(renderState);
        return getOrCreateRenderStateImpl<ClipPlane>(clipPlane->planeIndex(),
                                                     clipPlane->normal(),
                                                     clipPlane->distance());
    }
    case QRenderStatePrivate::SeamlessCubemap: {
        QSeamlessCubemap *seamlessCubemap = static_cast<QSeamlessCubemap *>(renderState);
        return getOrCreateRenderStateImpl<SeamlessCubemap>(seamlessCubemap->isEnabled());
    }
    case QRenderStatePrivate::StencilOp: {
        QStencilOperation *stencilOp = static_cast<QStencilOperation *>(renderState);
        const QStencilOperationArguments *front = stencilOp->front();
        const QStencilOperationArguments *back = stencilOp->back();
        return getOrCreateRenderStateImpl<StencilOp>(front->stencilTestFailureOperation(), front->depthTestFailureOperation(), front->allTestsPassOperation(),
                                      back->stencilTestFailureOperation(), back->depthTestFailureOperation(), back->allTestsPassOperation());
    }
    case QRenderStatePrivate::StencilMask: {
        QStencilMask *stencilMask = static_cast<QStencilMask *>(renderState);
        return getOrCreateRenderStateImpl<StencilMask>(stencilMask->frontOutputMask(), stencilMask->backOutputMask());
     }

    default:
        Q_UNREACHABLE();
        qFatal("Should not happen");
        return Q_NULLPTR;
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
