/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglabstracteffect.h"
#include "qglpainter_p.h"
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractEffect
    \since 4.8
    \brief The QGLAbstractEffect class provides a standard interface for rendering surface material effects with GL.
    \ingroup qt3d
    \ingroup qt3d::painting

    \section1 Vertex attributes

    Vertex attributes for the effect are specified using
    QGLPainter::setVertexAttribute() and QGLPainter::setVertexBundle(),
    and may be independent of the effect itself.  Those functions
    will bind standard attributes to specific indexes within the
    GL state.  For example, the QGL::Position will be bound
    to index 0, QGL::TextureCoord0 will be bound to index 3, etc.

    Effect subclasses that use vertex shaders should bind their attributes
    to these indexes using QGLShaderProgram::bindAttributeLocation()
    just before the program is linked.  For example:

    \code
    QGLShaderProgram *program = new QGLShaderProgram();
    program->addShaderFromSourceCode(QGLShader::Vertex, vshaderSource);
    program->addShaderFromSourceCode(QGLShader::Fragment, fshaderSource);
    program->bindAttributeLocation("vertex", QGL::Position);
    program->bindAttributeLocation("normal", QGL::Normal);
    program->bindAttributeLocation("texcoord", QGL::TextureCoord0);
    program->link();
    \endcode

    The QGLShaderProgramEffect class can assist with writing
    shader-based effects.  It will automatically bind special
    variable names, such as \c{qt_Vertex}, \c{qt_MultiTexCoord0}, etc,
    to the standard indexes.  This alleviates the need for the
    application to bind the names itself.
*/

/*!
    Constructs a new effect object.
*/
QGLAbstractEffect::QGLAbstractEffect()
{
}

/*!
    Destroys this effect object.
*/
QGLAbstractEffect::~QGLAbstractEffect()
{
}

/*!
    Returns true if this effect supports object picking; false otherwise.
    The default implementation returns false, which causes QGLPainter
    to use the effect associated with QGL::FlatColor to perform
    object picking.

    Effects that support object picking render fragments with
    QGLPainter::pickColor() when QGLPainter::isPicking() returns true.
    By default, only the effect associated with QGL::FlatColor does this,
    rendering the entire fragment with the flat pick color.

    In some cases, rendering the entire fragment with the pick color
    may not be appropriate.  An alpha-blended icon texture that is
    drawn to the screen as a quad may have an irregular shape smaller
    than the quad.  For picking, the application may not want the
    entire quad to be "active" for object selection as it would appear
    to allow the user to click off the icon to select it.

    This situation can be handled by implementing an icon rendering
    effect that draws the icon normally when QGLPainter::isPicking()
    is false, and draws a mask texture defining the outline of the icon
    with QGLPainter::pickColor() when QGLPainter::isPicking() is true.

    \sa QGLPainter::setPicking()
*/
bool QGLAbstractEffect::supportsPicking() const
{
    return false;
}

/*!
    \fn void QGLAbstractEffect::setActive(QGLPainter *painter, bool flag)

    Activates or deactivates this effect on \a painter,
    according to \a flag, on the current GL context by selecting
    shader programs, setting lighting and material parameters, etc.

    \sa update()
*/

/*!
    \fn void QGLAbstractEffect::update(QGLPainter *painter, QGLPainter::Updates updates)

    Updates the current GL context with information from \a painter
    just prior to the drawing of triangles, quads, etc.

    The \a updates parameter specifies the properties on \a painter
    that have changed since the last call to update() or setActive().

    \sa setActive()
*/

QT_END_NAMESPACE
