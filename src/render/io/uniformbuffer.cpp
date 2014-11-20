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

#include "uniformbuffer_p.h"
#include <private/qgraphicscontext_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

UniformBuffer::UniformBuffer()
    : m_bufferId(-1)
    , m_isCreated(false)
{
}

void UniformBuffer::create(QGraphicsContext *ctx)
{
    ctx->openGLContext()->functions()->glGenBuffers(1, &m_bufferId);
    m_isCreated = true;
}

void UniformBuffer::destroy(QGraphicsContext *ctx)
{
    ctx->openGLContext()->functions()->glDeleteBuffers(1, &m_bufferId);
    m_isCreated = false;
}

void UniformBuffer::allocate(QGraphicsContext *ctx, uint size, bool dynamic)
{
    ctx->openGLContext()->functions()->glBindBuffer(GL_UNIFORM_BUFFER, m_bufferId);
    // Either GL_STATIC_DRAW OR GL_DYNAMIC_DRAW depending on  the use case
    // TO DO: find a way to know how a buffer/QShaderData will be used to use the right usage
    ctx->openGLContext()->functions()->glBufferData(GL_UNIFORM_BUFFER, size, NULL, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    ctx->openGLContext()->functions()->glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update(QGraphicsContext *ctx, const void *data, uint size)
{
    ctx->openGLContext()->functions()->glBindBuffer(GL_UNIFORM_BUFFER, m_bufferId);
    // TO DO: At the moment, replaces the whole buffer,
    // look at updating only the parts that have change
    ctx->openGLContext()->functions()->glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    ctx->openGLContext()->functions()->glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::bindToUniformBlock(QGraphicsContext *ctx, int bindingPoint)
{
    ctx->bindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_bufferId);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
