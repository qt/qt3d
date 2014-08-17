/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
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
    Q_UNUSED(gc);
    glEnable(GL_BLEND);
    glBlendFunc( m_1, m_2 );
}

StateMaskSet BlendState::mask() const
{
    return BlendStateMask;
}

BlendState *BlendState::getOrCreate(GLenum src, GLenum dst)
{
    BlendState bs(src, dst);
    return getOrCreateImpl(bs);
}

BlendState::BlendState(GLenum src, GLenum dst) :
    GenericState2(src, dst)
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
    GenericState1(func)
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
    GenericState2(func, value)
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
    GenericState1(func)
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
    GenericState1(func)
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
    GenericState1(func)
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
    GenericState1(flag)
{
}

Dithering::Dithering()
    : RenderState()
{
}

void Dithering::apply(QGraphicsContext *) const
{
    glEnable(GL_DITHER);
}

Dithering *Dithering::getOrCreate()
{
    return getOrCreateImpl(Dithering());
}

ScissorTest::ScissorTest(int left, int bottom, int width, int height)
    : GenericState4(left, bottom, width, height)
{
}

void ScissorTest::apply(QGraphicsContext *) const
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(m_1, m_2, m_3, m_4);
}

ScissorTest *ScissorTest::getOrCreate(int left, int bottom, int width, int height)
{
    return getOrCreateImpl(ScissorTest(left, bottom, width, height));
}

StencilTest *StencilTest::getOrCreate(uint mask, GLenum func, GLenum faceMode)
{
    return getOrCreateImpl(StencilTest(mask, func, faceMode));
}

StencilTest::StencilTest(uint mask, GLenum func, GLenum faceMode)
    : GenericState3(mask, func, faceMode)
{
}

void StencilTest::apply(QGraphicsContext *) const
{
    glEnable(GL_STENCIL);
    glStencilFunc(m_2, m_3, m_1);
}

} // Render
} // Qt3D

QT_END_NAMESPACE
