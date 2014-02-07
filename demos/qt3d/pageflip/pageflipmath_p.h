/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef PAGEFLIPMATH_P_H
#define PAGEFLIPMATH_P_H

#include <QtGlobal>
#include <QRect>
#include <QOpenGLBuffer>

QT_BEGIN_NAMESPACE

class PageFlipMath
{
public:
    PageFlipMath();
    ~PageFlipMath();

    enum StartCorner
    {
        BottomRight,
        TopRight,
        BottomLeft,
        TopLeft,
        BottomLeftOnePage,
        TopLeftOnePage,
        VerticalBottomRight,
        VerticalTopRight,
        VerticalBottomLeft,
        VerticalTopLeft
    };

    // Corner to start flipping from.
    StartCorner startCorner() const { return m_startCorner; }
    void setStartCorner(StartCorner value) { m_startCorner = value; }

    // Rectangle to display the main page (usually the one on the right).
    QRect pageRect() const { return m_pageRect; }
    void setPageRect(const QRect& rect) { m_pageRect = rect; }

    // Show the reverse of the right-hand page on the back when
    // flipping pages.  That is, the user will essentially see
    // the texture on the front of the page "through" it in reverse
    // while it is being flipped.
    bool showPageReverse() const { return m_showPageReverse; }
    void setShowPageReverse(bool value) { m_showPageReverse = value; }

    // Get the vertex array pointer.
    const GLfloat *vertexArray() const { return vertices[0][0]; }

    // Get the vertex array stride in bytes.
    int stride() const { return 5 * sizeof(GLfloat); }

    // Draw a specific page.
    void drawPage(int page) const;

    // Draw the outline of a page as a set of lines.
    void drawOutline(int page) const;

    // Compute the frame at position t (0...1) in the animation.
    void compute(float t);

private:
    StartCorner m_startCorner;
    QRect m_pageRect;
    bool m_showPageReverse;

    // Vertex array: up to 4 pages, with up to 5 vertices per page,
    // and 5 components (2D position, 2D texcoord, 1D gradient control)
    // per vertex.  The gradient control value is interpolated between
    // 0 and 1 - it is 1 at the fold point and 0 on the side of the page
    // opposite the fold point.  Shaders can use this to extract a color
    // value from a gradient texture to blend with the page texture.
    GLfloat vertices[4][5][5];

    // Number of vertices for drawing the triangle fan for each page.
    int pageCount[4];

    void flip(float pageWidth, float pageHeight, float t);
};

QT_END_NAMESPACE

#endif
