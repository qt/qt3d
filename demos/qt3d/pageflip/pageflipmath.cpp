/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "pageflipmath_p.h"
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PageFlipMath::PageFlipMath()
{
    m_showPageReverse = false;
    m_startCorner = BottomRight;

    qMemSet(vertices, 0, sizeof(vertices));
    qMemSet(pageCount, 0, sizeof(pageCount));
}

PageFlipMath::~PageFlipMath()
{
}

void PageFlipMath::drawPage(int page) const
{
    if (page < 0 || page >= 4 || pageCount[page] == 0)
    return;
    glDrawArrays(GL_TRIANGLE_FAN, page * 5, pageCount[page]);
}

void PageFlipMath::drawOutline(int page) const
{
    if (page < 0 || page >= 4 || pageCount[page] == 0)
    return;
    glDrawArrays(GL_LINE_LOOP, page * 5, pageCount[page]);
}

void PageFlipMath::compute(qreal t)
{
    int page, vertex;

    // Compute the relative vertices for position t.
    if (m_startCorner < VerticalBottomRight)
        flip(m_pageRect.width() - 1, m_pageRect.height() - 1, t);
    else
        flip(m_pageRect.height() - 1, m_pageRect.width() - 1, t);

    // Deal with starting corner issues by swapping co-ordinates.
    switch (m_startCorner) {

    case BottomRight: break;

    case TopRight:
        for (page = 0; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                vertices[page][vertex][1]
                    = m_pageRect.height() - 1 - vertices[page][vertex][1];
            vertices[page][vertex][3]
                    = 1.0f - vertices[page][vertex][3];
            }
        }
        break;

    case BottomLeft:
        for (page = 0; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                vertices[page][vertex][0]
                    = m_pageRect.width() - 1 - vertices[page][vertex][0];
                vertices[page][vertex][0] -= m_pageRect.width();
            vertices[page][vertex][2]
                    = 1.0f - vertices[page][vertex][2];
            }
        }
        break;

    case TopLeft:
        for (page = 0; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                vertices[page][vertex][0]
                    = m_pageRect.width() - 1 - vertices[page][vertex][0];
                vertices[page][vertex][0] -= m_pageRect.width();
                vertices[page][vertex][1]
                    = m_pageRect.height() - 1 - vertices[page][vertex][1];
            vertices[page][vertex][2]
                    = 1.0f - vertices[page][vertex][2];
            vertices[page][vertex][3]
                    = 1.0f - vertices[page][vertex][3];
            }
        }
        break;

    case BottomLeftOnePage:
        for (page = 1; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                vertices[page][vertex][0]
                    = m_pageRect.width() - 1 - vertices[page][vertex][0];
            vertices[page][vertex][2]
                    = 1.0f - vertices[page][vertex][2];
            }
        }
        break;

    case TopLeftOnePage:
        for (page = 1; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                vertices[page][vertex][0]
                    = m_pageRect.width() - 1 - vertices[page][vertex][0];
                vertices[page][vertex][1]
                    = m_pageRect.height() - 1 - vertices[page][vertex][1];
            vertices[page][vertex][2]
                    = 1.0f - vertices[page][vertex][2];
            vertices[page][vertex][3]
                    = 1.0f - vertices[page][vertex][3];
            }
        }
        break;

    case VerticalBottomRight:
        for (page = 0; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                qSwap(vertices[page][vertex][0], vertices[page][vertex][1]);
                vertices[page][vertex][0]
                    = m_pageRect.width() - 1 - vertices[page][vertex][0];
                vertices[page][vertex][1]
                    = m_pageRect.height() - 1 - vertices[page][vertex][1];
                qSwap(vertices[page][vertex][2], vertices[page][vertex][3]);
            vertices[page][vertex][2]
                    = 1.0f - vertices[page][vertex][2];
            vertices[page][vertex][3]
                    = 1.0f - vertices[page][vertex][3];
            }
        }
        break;

    case VerticalTopRight:
        for (page = 0; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                qSwap(vertices[page][vertex][0], vertices[page][vertex][1]);
                vertices[page][vertex][0]
                    = m_pageRect.width() - 1 - vertices[page][vertex][0];
                qSwap(vertices[page][vertex][2], vertices[page][vertex][3]);
            vertices[page][vertex][2]
                    = 1.0f - vertices[page][vertex][2];
            }
        }
        break;

    case VerticalBottomLeft:
        for (page = 0; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                qSwap(vertices[page][vertex][0], vertices[page][vertex][1]);
                vertices[page][vertex][1]
                    = m_pageRect.height() - 1 - vertices[page][vertex][1];
                qSwap(vertices[page][vertex][2], vertices[page][vertex][3]);
            vertices[page][vertex][3]
                    = 1.0f - vertices[page][vertex][3];
            }
        }
        break;

    case VerticalTopLeft:
        for (page = 0; page < 4; ++page) {
        for (vertex = 0; vertex < pageCount[page]; ++vertex) {
                qSwap(vertices[page][vertex][0], vertices[page][vertex][1]);
                qSwap(vertices[page][vertex][2], vertices[page][vertex][3]);
            }
        }
        break;
    }

    // Adjust the vertices for the final rectangle position.
    for (page = 0; page < 4; ++page) {
    for (vertex = 0; vertex < pageCount[page]; ++vertex) {
        vertices[page][vertex][0] += m_pageRect.x();
        vertices[page][vertex][1]
        = m_pageRect.y() +
          (m_pageRect.height() - 1 - vertices[page][vertex][1]);
    }
    }

    // Flip the x texture co-ordinates for page 2 if showing the page reverse.
    if (m_showPageReverse) {
        if (m_startCorner < VerticalBottomRight) {
            for (int vertex = 0; vertex < pageCount[2]; ++vertex)
                vertices[2][vertex][2] = 1.0f - vertices[2][vertex][2];
        } else {
            for (int vertex = 0; vertex < pageCount[2]; ++vertex)
                vertices[2][vertex][3] = 1.0f - vertices[2][vertex][3];
        }
    }
}

// Page 1 is the reference page and extends from the bottom-left
// corner at (0, 0) to the top-right corner at (pageWidth, pageHeight).
// The flip starts at the bottom-right corner and proceeds leftwards
// across the page.  All other flip directions and starting corners
// can be derived from this basic reference flip animation.
void PageFlipMath::flip(qreal pageWidth, qreal pageHeight, qreal t)
{
    // Handle the simple start and end position cases first.
    if (t <= 0.0f) {
    // Starting position: pages 0 and 1 in their rest states
    // and pages 2 and 3 not visible.
    pageCount[0] = 4;
    pageCount[1] = 4;
    pageCount[2] = 0;
    pageCount[3] = 0;

    vertices[0][0][0] = -pageWidth; // corner 0 at (-pageWidth, 0)
    vertices[0][0][1] = 0.0f;
    vertices[0][0][2] = 0.0f; // texture co-ordinate
    vertices[0][0][3] = 0.0f;
    vertices[0][0][4] = 0.0f; // gradient control

    vertices[0][1][0] = 0.0f; // corner 1 at (0, 0)
    vertices[0][1][1] = 0.0f;
    vertices[0][1][2] = 1.0f;
    vertices[0][1][3] = 0.0f;
    vertices[0][1][4] = 1.0f; // gradient along fold on the right

    vertices[0][2][0] = 0.0f; // corner 2 at (0, pageHeight)
    vertices[0][2][1] = pageHeight;
    vertices[0][2][2] = 1.0f;
    vertices[0][2][3] = 1.0f;
    vertices[0][2][4] = 1.0f;

    vertices[0][3][0] = -pageWidth; // corner 3 at (-pageWidth, pageHeight)
    vertices[0][3][1] = pageHeight;
    vertices[0][3][2] = 0.0f;
    vertices[0][3][3] = 1.0f;
    vertices[0][3][4] = 0.0f;

    vertices[1][0][0] = 0.0f; // corner 0 at (0, 0)
    vertices[1][0][1] = 0.0f;
    vertices[1][0][2] = 0.0f; // texture co-ordinate
    vertices[1][0][3] = 0.0f;
    vertices[1][0][4] = 1.0f; // gradient along fold on the left

    vertices[1][1][0] = pageWidth; // corner 1 at (pageWidth, 0)
    vertices[1][1][1] = 0.0f;
    vertices[1][1][2] = 1.0f;
    vertices[1][1][3] = 0.0f;
        vertices[1][1][4] = 0.0f;

    vertices[1][2][0] = pageWidth; // corner 2 at (pageWidth, pageHeight)
    vertices[1][2][1] = pageHeight;
    vertices[1][2][2] = 1.0f;
    vertices[1][2][3] = 1.0f;
    vertices[1][2][4] = 0.0f;

    vertices[1][3][0] = 0.0f; // corner 3 at (0, pageHeight)
    vertices[1][3][1] = pageHeight;
    vertices[1][3][2] = 0.0f;
    vertices[1][3][3] = 1.0f;
    vertices[1][3][4] = 1.0f;
    return;
    } else if (t >= 1.0f) {
    // Ending position: pages 0 and 1 are not visible, but
    // pages 2 and 3 are visible in the rest states.
    pageCount[0] = 0;
    pageCount[1] = 0;
    pageCount[2] = 4;
    pageCount[3] = 4;

    vertices[2][0][0] = -pageWidth; // corner 0 at (-pageWidth, 0)
    vertices[2][0][1] = 0.0f;
    vertices[2][0][2] = 0.0f; // texture co-ordinate
    vertices[2][0][3] = 0.0f;
    vertices[2][0][4] = 0.0f; // gradient control

    vertices[2][1][0] = 0.0f; // corner 1 at (0, 0)
    vertices[2][1][1] = 0.0f;
    vertices[2][1][2] = 1.0f;
    vertices[2][1][3] = 0.0f;
    vertices[2][1][4] = 1.0f; // gradient along fold on the right

    vertices[2][2][0] = 0.0f; // corner 2 at (0, pageHeight)
    vertices[2][2][1] = pageHeight;
    vertices[2][2][2] = 1.0f;
    vertices[2][2][3] = 1.0f;
    vertices[2][2][4] = 1.0f;

    vertices[2][3][0] = -pageWidth; // corner 3 at (-pageWidth, pageHeight)
    vertices[2][3][1] = pageHeight;
    vertices[2][3][2] = 0.0f;
    vertices[2][3][3] = 1.0f;
    vertices[2][3][4] = 0.0f;

    vertices[3][0][0] = 0.0f; // corner 0 at (0, 0)
    vertices[3][0][1] = 0.0f;
    vertices[3][0][2] = 0.0f; // texture co-ordinate
    vertices[3][0][3] = 0.0f;
    vertices[3][0][4] = 1.0f; // gradient along fold on the left

    vertices[3][1][0] = pageWidth; // corner 1 at (pageWidth, 0)
    vertices[3][1][1] = 0.0f;
    vertices[3][1][2] = 1.0f;
    vertices[3][1][3] = 0.0f;
    vertices[3][1][4] = 0.0f;

    vertices[3][2][0] = pageWidth; // corner 2 at (pageWidth, pageHeight)
    vertices[3][2][1] = pageHeight;
    vertices[3][2][2] = 1.0f;
    vertices[3][2][3] = 1.0f;
    vertices[3][2][4] = 0.0f;

    vertices[3][3][0] = 0.0f; // corner 3 at (0, pageHeight)
    vertices[3][3][1] = pageHeight;
    vertices[3][3][2] = 0.0f;
    vertices[3][3][3] = 1.0f;
    vertices[3][3][4] = 1.0f;
    return;
    }

    // Page 0 is the same for all other animation positions.
    pageCount[0] = 4;

    vertices[0][0][0] = -pageWidth; // corner 0 at (-pageWidth, 0)
    vertices[0][0][1] = 0.0f;
    vertices[0][0][2] = 0.0f;  // texture co-ordinate
    vertices[0][0][3] = 0.0f;
    vertices[0][0][4] = 0.0f;  // gradient control

    vertices[0][1][0] = 0.0f;  // corner 1 at (0, 0)
    vertices[0][1][1] = 0.0f;
    vertices[0][1][2] = 1.0f;
    vertices[0][1][3] = 0.0f;
    vertices[0][1][4] = 1.0f;  // gradient along fold on the right

    vertices[0][2][0] = 0.0f;  // corner 2 at (0, pageHeight)
    vertices[0][2][1] = pageHeight;
    vertices[0][2][2] = 1.0f;
    vertices[0][2][3] = 1.0f;
    vertices[0][2][4] = 1.0f;

    vertices[0][3][0] = -pageWidth; // corner 3 at (-pageWidth, pageHeight)
    vertices[0][3][1] = pageHeight;
    vertices[0][3][2] = 0.0f;
    vertices[0][3][3] = 1.0f;
    vertices[0][3][4] = 0.0f;

    // Get the angle of the "curling" dividing line to the bottom of the page.
    // Basically: 45deg + (45deg * t) = 45deg * (1 + t), where t is between
    // 0 and 1 but is neither 0 nor 1.
    qreal angle = (M_PI / 4.0f) * (1.0f + t);

    // We need the cos and sin of both the angle and angle * 2.
    qreal cosAngle = qCos(angle);
    qreal sinAngle = qSin(angle);
    qreal cosAngle2 = qCos(angle * 2.0f);
    qreal sinAngle2 = qSin(angle * 2.0f);

    // Find the reference point.  This is the point along the bottom of
    // the page where the dividing line intersects the page bottom.
    qreal refx = pageWidth * (1.0f - t);
    qreal refy = 0.0f;

    // Distance from the reference point to the right side of the page.
    qreal d = pageWidth - refx;

    // Determine the intersection of the dividing line with the
    // top of the page.  If the intersection is not on the page (k >= d),
    // then we need to generate similar triangles.  If the intersection is
    // on the page (k < d), then we need to generate similar trapezoids.
    qreal k = (pageHeight * cosAngle) / sinAngle;
    if (k >= d) {
    // Generate similar triangles.  Find the intersection with
    // the right-hand side of the page at x == pageWidth.
    qreal intx = pageWidth;
    qreal inty = refy + (d * sinAngle) / cosAngle;

    // Find the opposite triangle corner on the back page.
    qreal oppx = refx + d * cosAngle2;
    qreal oppy = refy + d * sinAngle2;

    // Generate vertices and texture co-ordinates for the back page.
    qreal texa = 1.0f - (d * sinAngle) / (pageHeight * cosAngle);
    qreal texb = d / pageWidth;
    vertices[2][0][0] = intx;
    vertices[2][0][1] = inty;
    vertices[2][0][2] = 0.0f;
    vertices[2][0][3] = 1.0f - texa;
    vertices[2][0][4] = 1.0f;

    vertices[2][1][0] = oppx;
    vertices[2][1][1] = oppy;
    vertices[2][1][2] = 0.0f;
    vertices[2][1][3] = 0.0f;
    vertices[2][1][4] = 1.0f - texb;

    vertices[2][2][0] = refx;
    vertices[2][2][1] = refy;
    vertices[2][2][2] = texb;
    vertices[2][2][3] = 0.0f;
    vertices[2][2][4] = 1.0f;

    pageCount[2] = 3;

    // Generate vertices and texture co-ordinates for the next page.
    vertices[3][0][0] = intx;
    vertices[3][0][1] = inty;
    vertices[3][0][2] = 1.0f;
    vertices[3][0][3] = 1.0f - texa;
    vertices[3][0][4] = 1.0f;

    vertices[3][1][0] = refx;
    vertices[3][1][1] = refy;
    vertices[3][1][2] = 1.0f - texb;
    vertices[3][1][3] = 0.0f;
    vertices[3][1][4] = 1.0f;

    vertices[3][2][0] = pageWidth;
    vertices[3][2][1] = 0.0f;
    vertices[3][2][2] = 1.0f;
    vertices[3][2][3] = 0.0f;
    vertices[3][2][4] = 1.0f - texb;

    pageCount[3] = 3;

    // Set page 1's vertices to clip off pixels we don't need to draw.
    vertices[1][0][0] = 0.0f;
    vertices[1][0][1] = 0.0f;
    vertices[1][0][2] = 0.0f;
    vertices[1][0][3] = 0.0f;
    vertices[1][0][4] = 1.0f;

    vertices[1][1][0] = pageWidth - d;
    vertices[1][1][1] = 0.0f;
    vertices[1][1][2] = 1.0f - texb;
    vertices[1][1][3] = 0.0f;
    vertices[1][1][4] = texb;

    vertices[1][2][0] = intx;
    vertices[1][2][1] = inty;
    vertices[1][2][2] = 1.0f;
    vertices[1][2][3] = 1.0f - texa;
    vertices[1][2][4] = 0.0f;

    vertices[1][3][0] = pageWidth;
    vertices[1][3][1] = pageHeight;
    vertices[1][3][2] = 1.0f;
    vertices[1][3][3] = 1.0f;
    vertices[1][3][4] = 0.0f;

    vertices[1][4][0] = 0.0f;
    vertices[1][4][1] = pageHeight;
    vertices[1][4][2] = 0.0f;
    vertices[1][4][3] = 1.0f;
    vertices[1][4][4] = 1.0f;

    pageCount[1] = 5;
    } else {
    // Generate similar trapezoids.  Find the intersection with
    // the top of the page at y == pageHeight.
    qreal intx = refx + (pageHeight * cosAngle) / sinAngle;
    qreal inty = pageHeight;

    // Get the distance between the intersection and the right of the page.
    qreal e = pageWidth - intx;

    // Find the opposite trapezoid corners to "ref" and "int".
    qreal opprefx = refx + d * cosAngle2;
    qreal opprefy = refy + d * sinAngle2;
    qreal oppintx = intx + e * cosAngle2;
    qreal oppinty = inty + e * sinAngle2;

    // Generate vertices and texture co-ordinates for the back page.
    qreal texa = e / pageWidth;
    qreal texb = d / pageWidth;
    vertices[2][0][0] = intx;
    vertices[2][0][1] = inty;
    vertices[2][0][2] = texa;
    vertices[2][0][3] = 1.0f;
    vertices[2][0][4] = 1.0f;

    vertices[2][1][0] = oppintx;
    vertices[2][1][1] = oppinty;
    vertices[2][1][2] = 0.0f;
    vertices[2][1][3] = 1.0f;
    vertices[2][1][4] = 1.0f - texa;

    vertices[2][2][0] = opprefx;
    vertices[2][2][1] = opprefy;
    vertices[2][2][2] = 0.0f;
    vertices[2][2][3] = 0.0f;
    vertices[2][2][4] = 1.0f - texb;

    vertices[2][3][0] = refx;
    vertices[2][3][1] = refy;
    vertices[2][3][2] = texb;
    vertices[2][3][3] = 0.0f;
    vertices[2][3][4] = 1.0f;

    pageCount[2] = 4;

    // Generate vertices and texture co-ordinates for the next page.
    vertices[3][0][0] = intx;
    vertices[3][0][1] = inty;
    vertices[3][0][2] = 1.0f - texa;
    vertices[3][0][3] = 1.0f;
    vertices[3][0][4] = 1.0f;

    vertices[3][1][0] = refx;
    vertices[3][1][1] = refy;
    vertices[3][1][2] = 1.0f - texb;
    vertices[3][1][3] = 0.0f;
    vertices[3][1][4] = 1.0f;

    vertices[3][2][0] = pageWidth;
    vertices[3][2][1] = 0.0f;
    vertices[3][2][2] = 1.0f;
    vertices[3][2][3] = 0.0f;
    vertices[3][2][4] = 1.0f - texb;

    vertices[3][3][0] = pageWidth;
    vertices[3][3][1] = pageHeight;
    vertices[3][3][2] = 1.0f;
    vertices[3][3][3] = 1.0f;
    vertices[3][3][4] = 1.0f - texa;

    pageCount[3] = 4;

    // Set page 1's vertices to clip off pixels we don't need to draw.
    vertices[1][0][0] = 0.0f;
    vertices[1][0][1] = 0.0f;
    vertices[1][0][2] = 0.0f;
    vertices[1][0][3] = 0.0f;
    vertices[1][0][4] = 1.0f;

    vertices[1][1][0] = pageWidth - d;
    vertices[1][1][1] = 0.0f;
    vertices[1][1][2] = 1.0f - texb;
    vertices[1][1][3] = 0.0f;
    vertices[1][1][4] = texb;

    vertices[1][2][0] = pageWidth - e;
    vertices[1][2][1] = pageHeight;
    vertices[1][2][2] = 1.0f - texa;
    vertices[1][2][3] = 1.0f;
    vertices[1][2][4] = texa;

    vertices[1][3][0] = 0.0f;
    vertices[1][3][1] = pageHeight;
    vertices[1][3][2] = 0.0f;
    vertices[1][3][3] = 1.0f;
    vertices[1][3][4] = 1.0f;

    pageCount[1] = 4;
    }
}

QT_END_NAMESPACE
