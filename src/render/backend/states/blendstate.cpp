/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "blendstate_p.h"

#include <Qt3DRenderer/private/qgraphicscontext_p.h>

#include <QOpenGLFunctions_3_2_Core>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

template <class State>
State* getOrCreateImpl(const State& data)
{
    static QList<State*> static_instances;
    foreach (State* ext, static_instances) {
        if (ext->isEqual(data))
            return ext;
    }

    State* result = new State(data);
    static_instances.append(result);
    return result;
}

void BlendState::apply(QGraphicsContext* gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_BLEND);
    gc->openGLContext()->functions()->glBlendFunc( m_1, m_2 );
}

BlendState *BlendState::getOrCreate(GLenum src, GLenum dst)
{
    BlendState bs(src, dst);
    return getOrCreateImpl(bs);
}

BlendState::BlendState(GLenum src, GLenum dst) :
    GenericState2<BlendState, GLenum, GLenum>(src, dst)
{
}

void BlendEquation::apply(QGraphicsContext *gc) const
{
    gc->blendEquation(m_1);
}

BlendEquation *BlendEquation::getOrCreate(GLenum func)
{
    return getOrCreateImpl(BlendEquation(func));
}

BlendEquation::BlendEquation(GLenum func) :
    GenericState1<BlendEquation, GLenum>(func)
{
}


void AlphaFunc::apply(QGraphicsContext* gc) const
{
    gc->alphaTest(m_1, m_2);
}

AlphaFunc *AlphaFunc::getOrCreate(GLenum func, GLclampf value)
{
    AlphaFunc af(func, value);
    return getOrCreateImpl(af);
}

AlphaFunc::AlphaFunc(GLenum func, GLclampf value) :
    GenericState2<AlphaFunc, GLenum, GLclampf>(func, value)
{
}

void DepthTest::apply(QGraphicsContext *gc) const
{
    gc->depthTest(m_1);
}

DepthTest *DepthTest::getOrCreate(GLenum func)
{
    DepthTest dt(func);
    return getOrCreateImpl(dt);
}

DepthTest::DepthTest(GLenum func) :
    GenericState1<DepthTest, GLenum>(func)
{
}

void CullFace::apply(QGraphicsContext *gc) const
{
    gc->cullFace(m_1);
}

CullFace *CullFace::getOrCreate(GLenum func)
{
    return getOrCreateImpl(CullFace(func));
}

CullFace::CullFace(GLenum func) :
    GenericState1<CullFace, GLenum>(func)
{
}

void FrontFace::apply(QGraphicsContext *gc) const
{
    gc->frontFace(m_1);
}

FrontFace *FrontFace::getOrCreate(GLenum func)
{
    return getOrCreateImpl(FrontFace(func));
}

FrontFace::FrontFace(GLenum func) :
    GenericState1<FrontFace, GLenum>(func)
{
}

void DepthMask::apply(QGraphicsContext *gc) const
{
   gc->depthMask(m_1);
}

DepthMask *DepthMask::getOrCreate(GLboolean flag)
{
    return getOrCreateImpl(DepthMask(flag));
}

DepthMask::DepthMask(GLboolean flag) :
    GenericState1<DepthMask, GLboolean>(flag)
{
}

Dithering::Dithering()
    : RenderState()
{
}

void Dithering::apply(QGraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_DITHER);
}

Dithering *Dithering::getOrCreate()
{
    return getOrCreateImpl(Dithering());
}

ScissorTest::ScissorTest(int left, int bottom, int width, int height)
    : GenericState4<ScissorTest, int, int, int, int>(left, bottom, width, height)
{
}

void ScissorTest::apply(QGraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_SCISSOR_TEST);
    gc->openGLContext()->functions()->glScissor(m_1, m_2, m_3, m_4);
}

ScissorTest *ScissorTest::getOrCreate(int left, int bottom, int width, int height)
{
    return getOrCreateImpl(ScissorTest(left, bottom, width, height));
}

StencilTest *StencilTest::getOrCreate(GLenum frontFunc, int frontRef, uint frontMask, GLenum backFunc, int backRef, uint backMask)
{
    return getOrCreateImpl(StencilTest(frontFunc, frontRef, frontMask,
                                       backFunc, backRef, backMask));
}

StencilTest::StencilTest(GLenum frontFunc, int frontRef, uint frontMask, GLenum backFunc, int backRef, uint backMask)
    : GenericState6<StencilTest, GLenum, int, uint, GLenum, int, uint>(frontFunc, frontRef, frontMask,
                                                                       backFunc, backRef, backMask)
{
}

void StencilTest::apply(QGraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_STENCIL_TEST);
    gc->openGLContext()->functions()->glStencilFuncSeparate(GL_FRONT, m_1, m_2, m_3);
    gc->openGLContext()->functions()->glStencilFuncSeparate(GL_BACK, m_4, m_5, m_6);
}

AlphaCoverage::AlphaCoverage()
    : RenderState()
{
}

void AlphaCoverage::apply(QGraphicsContext *gc) const
{
    gc->enableAlphaCoverage();
}

AlphaCoverage *AlphaCoverage::getOrCreate()
{
    return getOrCreateImpl(AlphaCoverage());
}

void PolygonOffset::apply(QGraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_POLYGON_OFFSET_FILL);
    gc->openGLContext()->functions()->glPolygonOffset(m_1, m_2);
}

PolygonOffset *PolygonOffset::getOrCreate(GLfloat factor, GLfloat units)
{
    return getOrCreateImpl(PolygonOffset(factor, units));
}

PolygonOffset::PolygonOffset(GLfloat factor, GLfloat units)
    : GenericState2<PolygonOffset, GLfloat, GLfloat>(factor, units)
{
}

void ColorMask::apply(QGraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glColorMask(m_1, m_2, m_3, m_4);
}

ColorMask::ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
    : GenericState4<ColorMask, GLboolean, GLboolean, GLboolean, GLboolean>(red, green, blue, alpha)
{
}

ColorMask *ColorMask::getOrCreate(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    return getOrCreateImpl(ColorMask(red, green, blue, alpha));
}

void BlendStateSeparate::apply(QGraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_BLEND);
    gc->openGLContext()->functions()->glBlendFuncSeparate(m_1, m_2, m_3, m_4);
}

BlendStateSeparate *BlendStateSeparate::getOrCreate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    return getOrCreateImpl(BlendStateSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha));
}

BlendStateSeparate::BlendStateSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
    : GenericState4<BlendStateSeparate, GLenum, GLenum, GLenum, GLenum>(srcRGB, dstRGB, srcAlpha, dstAlpha)
{
}

void ClipPlane::apply(QGraphicsContext *gc) const
{
    gc->enableClipPlane(m_1);
}

ClipPlane::ClipPlane(int plane)
    : GenericState1<ClipPlane, int>(plane)
{
}

ClipPlane *ClipPlane::getOrCreate(int plane)
{
    return getOrCreateImpl(ClipPlane(plane));
}

void StencilOp::apply(QGraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glStencilOpSeparate(GL_FRONT, m_1, m_2, m_3);
    gc->openGLContext()->functions()->glStencilOpSeparate(GL_BACK, m_4, m_5, m_6);
}

StencilOp *StencilOp::getOrCreate(GLenum fsfail, GLenum fdfail, GLenum fdspass, GLenum bsfail, GLenum bdfail, GLenum bdspass)
{
    return getOrCreateImpl(StencilOp(fsfail, fdfail, fdspass, bsfail, bdfail, bdspass));
}

StencilOp::StencilOp(GLenum fsfail, GLenum fdfail, GLenum fdspass, GLenum bsfail, GLenum bdfail, GLenum bdspass)
    : GenericState6<StencilOp, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum>(fsfail, fdfail, fdspass, bsfail, bdfail, bdspass)
{
}

void StencilMask::apply(QGraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glStencilMaskSeparate(GL_FRONT, m_1);
    gc->openGLContext()->functions()->glStencilMaskSeparate(GL_BACK, m_2);
}

StencilMask *StencilMask::getOrCreate(uint frontMask, uint backMask)
{
    return getOrCreateImpl(StencilMask(frontMask, backMask));
}

StencilMask::StencilMask(uint frontMask, uint backMask)
    : GenericState2<StencilMask, uint, uint>(frontMask, backMask)
{
}

} // Render
} // Qt3D

QT_END_NAMESPACE
