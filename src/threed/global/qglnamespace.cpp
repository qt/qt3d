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

#include "qglnamespace.h"
#include <QtCore/qsize.h>

QT_BEGIN_NAMESPACE

/*!
    \namespace QGL

    \brief The QGL namespace contains miscellaneous identifiers
    used throughout the Qt3D module.
*/

/*!
    \enum QGL::VertexAttribute
    \since 4.8
    This enum defines the type of vertex attribute to set on an effect with QGLPainter::setVertexAttribute()

    \value Position The primary position of the vertex.
    \value Normal The normal at each vertex.
    \value Color The color at each vertex.
    \value TextureCoord0 The texture co-ordinate at each vertex for texture unit 0.
    \value TextureCoord1 The texture co-ordinate at each vertex for texture unit 1.
    \value TextureCoord2 The texture co-ordinate at each vertex for texture unit 2.
    \value CustomVertex0 First custom vertex attribute.  Custom attributes
           can be used for any purpose: texture co-ordinates, colors,
           or other values of interest to shader programs.
    \value CustomVertex1 Second custom vertex attribute.
    \value UserVertex First user-assigned vertex attribute.  Additional
           attributes can be assigned as UserVertex, UserVertex + 1, etc.
           Note that on OpenGL/ES 2.0 systems, usually the maximum
           number of vertex attributes is 8, which means that user-assigned
           vertex attributes will be out of range.
*/

/*!
    \enum QGL::Face
    \since 4.8
    This enum defines the faces to apply an operation to.

    \value FrontFaces Apply the operation to front faces only.
    \value BackFaces Apply the operation to back faces only.
    \value AllFaces Apply the operation to both front and back faces.
*/

/*!
    \enum QGL::DrawingMode
    \since 4.8
    This enum defines the type of OpenGL primitive to render with QGLPainter::draw().

    \value Points Draws a point at each of the specified vertices.
    \value Lines Draws a series of unconnected line segments, using two
           vertices for each line.
    \value LineLoop series of connected line segments, from the
           first to the last vertex, and then connecting the last
           vertex back to the first vertex.
    \value LineStrip Draws a series of connected line segments, from the
           first to the last vertex.
    \value Triangles Draws a series of triangles using three vertices from
           the enabled vertex arrays for each triangle.
    \value TriangleStrip Draws a series of triangles in a strip, starting
           with the first three vertices and then one extra vertex for each
           additional triangle.
    \value TriangleFan Draws a series of triangles that fan out around the
           first vertex in the enabled vertex arrays, starting with the
           first three vertices and then one extra vertex for each additional
           triangle.
    \value LinesAdjacency Draws a series of unconnected lines, using
           two vertices for each line to define the positions, and an
           additional vertices per line to define adjacent points.
           This drawing mode is only supported on OpenGL systems that
           have geometry shaders.
    \value LineStripAdjacency Draws a series of connected line segments,
           from the second to the second last vertex.  The first and last
           vertices define adjacent points.  This drawing mode is only
           supported on OpenGL systems that have geometry shaders.
    \value TrianglesAdjacency Draws a series of triangles using three
           vertices from the enabled vertex arrays for each triangle.
           An additional three vertices per triangle are supplied to
           define adjacent points.  This drawing mode is only supported
           on OpenGL systems that have geometry shaders.
    \value TriangleStripAdjacency Draws a series of triangles in a strip,
           with additional vertices for points adjacent to the strip.
           This drawing mode is only supported on OpenGL systems that
           have geometry shaders.
*/

/*!
    \enum QGL::StandardEffect
    \since 4.8
    This enum defines a standard drawing effect for use with QGLPainter.

    \value FlatColor Single flat color specified by QGLPainter::setColor()
           with no lighting.
    \value FlatPerVertexColor Per-vertex flat colors specified by a
           QGL::Color vertex array with no lighting.
    \value FlatReplaceTexture2D Map a texture across the fragments with
           no lighting.  The final fragment color is replaced directly
           with the texture.  The texture is sourced from texture unit 0.
    \value FlatDecalTexture2D Map a texture across the fragments, combined
           with QGLPainter::color(), and no lighting.  The texture is sourced
           from texture unit 0.
    \value LitMaterial Material colors specified by
           QGLPainter::setFaceMaterial() with lighting enabled.  It is
           assumed that per-vertex normals are provided.  Under OpenGL/ES 2.0
           only one light is supported, with single-sided materials,
           and no attenuation.
    \value LitDecalTexture2D Map a texture across the fragments, combined
           with the material color specified by QGLPainter::setFaceMaterial(),
           and lighting using the GL_DECAL combination rule.  The texture is
           sourced from texture unit 0.  It is assumed that per-vertex
           normals are provided.  Under OpenGL/ES 2.0 only one light is
           supported, with single-sided materials, and no attenuation.
    \value LitModulateTexture2D Map a texture across the fragments, combined
           with the material color specified by QGLPainter::setFaceMaterial(),
           and lighting using the GL_MODULATE combination rule.  The texture
           is sourced from texture unit 0.  It is assumed that per-vertex
           normals are provided.  Under OpenGL/ES 2.0 only one light is
           supported, with single-sided materials, and no attenuation.
*/

/*!
    \enum QGL::TextureWrap
    \since 4.8
    This enum defines the wrapping mode for texture co-ordinates.

    \value Repeat Ignore the integer part of the texture co-ordinate and
        create a repeating pattern using the texture.
    \value Clamp Clamps the texture co-ordinate to the range 0 to 1.
        This value is not supported in OpenGL/ES, where it will be
        treated the same as ClampToEdge.
    \value ClampToBorder Clamps the texture co-ordinate to the range
        (-1 / 2N) to (1 + 1 / 2N) where N is the size of the texture in
        the direction of clamping.  This value is supported in OpenGL 1.3
        and higher.  In OpenGL versions less than 1.3, this value will
        be treated the same as Clamp.  In OpenGL/ES, this value will be
        treated the same as ClampToEdge.
    \value ClampToEdge Clamps the texture co-ordinate to the range
        (1 / 2N) to (1 - 1 / 2N) where N is the size of the texture in
        the direction of clamping.  This value is supported in OpenGL 1.2
        and higher, and in all versions of OpenGL/ES.  In OpenGL versions
        less than 1.2, this value will be treated the same as Clamp.
    \value MirroredRepeat If the integer part of the texture co-ordinate
        is even, then use the fractional part.  If the integer part of
        the texture co-ordinate is odd, then use 1 - the fractional part.
        This creates an alternating repeating pattern from the texture.
        This value is only supported in OpenGL 1.4 and higher, or
        OpenGL/ES 2.0 and higher.  On other versions, MirroredRepeat
        will be treated the same as Repeat.
*/

/*!
    \enum QGL::Eye
    \since 4.8
    This enum defines the eye that is being rendered by a QGLPainter
    when stereo rendering is in effect.

    \value NoEye Do not perform an eye adjustment on the camera because
        stereo rendering is not in effect.
    \value LeftEye Render the scene from the perspective of the left eye.
    \value RightEye Render the scene from the perspective of the right eye.
*/

/*!
    \enum QGL::Smoothing
    \since 4.8
    \relates QGLBuilder

    This enum defines vertex smoothing treatments.
    \value NoSmoothing No smoothing processing is performed.
    \value Smooth Lighting normals averaged for each face for a smooth appearance.
    \value Faceted Lighting normals separate for each face for a faceted appearance.
*/

/*!
    \enum QGL::Mouse3DKeys
    \since 4.8
    This enum defines extra key codes for QKeyEvent related to 3D mice
    devices.  See QMouse3DEventProvider for further details.

    \value Key_Fit Reset the transform so the model is centered to
           fit within the view.
    \value Key_TopView Switch to the top view.
    \value Key_LeftView Switch to the left view.
    \value Key_RightView Switch to the right view.
    \value Key_FrontView Switch to the front view.
    \value Key_BottomView Switch to the bottom view.
    \value Key_BackView Switch to the back view.
    \value Key_RotateCW90 Rotate clockwise by 90 degrees.
    \value Key_RotateCCW90 Rotate counter-clockwise by 90 degrees.
    \value Key_ISO1 Show an isometric view of the model with top, front,
           and right sides.
    \value Key_ISO2 Show an isometric view of the model with top, back
           and left sides.
    \value Key_Button1 Special function button 1.
    \value Key_Button2 Special function button 2.
    \value Key_Button3 Special function button 3.
    \value Key_Button4 Special function button 4.
    \value Key_Button5 Special function button 5.
    \value Key_Button6 Special function button 6.
    \value Key_Button7 Special function button 7.
    \value Key_Button8 Special function button 8.
    \value Key_Button9 Special function button 9.
    \value Key_Button10 Special function button 10.
    \value Key_Rotations Toggles whether or not the rotation axes on a
           3D mouse will be filtered.
    \value Key_Translations Toggles whether or not the translation axes
           on a 3D mouse will be filtered.
    \value Key_DominantAxis Toggles whether or not 3D mouse movements
           should be constrained to the dominant axis.
    \value Key_IncreaseSensitivity Increase the sensitivity of the mouse
           to wrist movements.
    \value Key_DecreaseSensitivity Decrease the sensitivity of the mouse
           to wrist movements.
*/

/*!
    Returns the next power of two that is greater than or
    equal to \a value.  The \a value must be positive or the
    result is undefined.

    This is a convenience function for use with GL texture
    handling code.
*/
int QGL::nextPowerOfTwo(int value)
{
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    ++value;
    return value;
}

/*!
    Returns the next power of two that is greater than or
    equal to the width and height components of \a value.

    This is a convenience function for use with GL texture
    handling code.
*/
QSize QGL::nextPowerOfTwo(const QSize& value)
{
    return QSize(nextPowerOfTwo(value.width()),
                 nextPowerOfTwo(value.height()));
}

QT_END_NAMESPACE
