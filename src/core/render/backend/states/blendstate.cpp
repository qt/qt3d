/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "blendstate.h"

#include "qgraphicscontext.h"

#include <QOpenGLFunctions_3_2_Core>

namespace Qt3D {

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
    Q_UNUSED(gc);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc( m_1, m_2 );
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
    Q_UNUSED(gc);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc( m_1 );
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
    Q_UNUSED(gc);
    glEnable(GL_CULL_FACE);
    glCullFace( m_1 );
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
    Q_UNUSED(gc);
    glFrontFace(m_1);
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
    Q_UNUSED(gc)
    glDepthMask(m_1);
}

DepthMask *DepthMask::getOrCreate(GLboolean flag)
{
    return getOrCreateImpl(DepthMask(flag));
}

DepthMask::DepthMask(GLboolean flag) :
    GenericState1(flag)
{
}


} // of namespace
