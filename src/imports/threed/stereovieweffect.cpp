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

#include "stereovieweffect.h"
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>
#include <QtOpenGL/qgl.h>

StereoViewEffect::StereoViewEffect(StereoView *parent)
    : QGraphicsEffect(parent)
    , m_view(parent)
{
}

StereoViewEffect::~StereoViewEffect()
{
}

void StereoViewEffect::draw(QPainter *painter)
{
    // Determine if we have an OpenGL rendering surface or not.
    QPaintEngine *engine = painter->paintEngine();
    bool isOpenGL = (engine->type() == QPaintEngine::OpenGL ||
                     engine->type() == QPaintEngine::OpenGL2);

    // Determine how to draw the two eye images.
    StereoView::Layout layout = m_view->layout();
    if (layout == StereoView::RedCyan) {
        // Draw the scene twice with red and cyan filters.
        if (isOpenGL) {
            m_view->setEye(QGL::LeftEye);
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
            drawSource(painter);
            m_view->setEye(QGL::RightEye);
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            drawSource(painter);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            m_view->setEye(QGL::NoEye);
        } else {
            // No OpenGL, so draw normally.
            drawSource(painter);
        }
    } else if (layout == StereoView::Default ||
                    layout == StereoView::Hardware) {
        // Draw the scene into the left and right back buffers.
#if defined(GL_BACK_LEFT) && defined(GL_BACK_RIGHT)
        if (isOpenGL && QGLContext::currentContext()->format().stereo()) {
            m_view->setEye(QGL::LeftEye);
            glDrawBuffer(GL_BACK_LEFT);
            drawSource(painter);
            m_view->setEye(QGL::RightEye);
            glDrawBuffer(GL_BACK_RIGHT);
            drawSource(painter);
            m_view->setEye(QGL::NoEye);
        } else
#endif
        if (isOpenGL && layout == StereoView::Default) {
            // No hardware stereo, so use red-cyan instead.
            m_view->setEye(QGL::LeftEye);
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
            drawSource(painter);
            m_view->setEye(QGL::RightEye);
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            drawSource(painter);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            m_view->setEye(QGL::NoEye);
        } else {
            // No OpenGL or red-cyan not allowed, so draw normally.
            drawSource(painter);
        }
    } else if (layout != StereoView::Disabled) {
        // Fetch the original window and viewport from the painter.
        // We then use these values to set up an alternative logical
        // co-ordinate system that shifts drawing into the left or
        // right screen halves.
        QRect window = painter->window();
        QRect viewport = painter->viewport();
        QRect leftViewport(viewport);
        QRect rightViewport(viewport);
        switch (layout) {
        case StereoView::LeftRight: default:
            window.setWidth(window.width() / 2);
            // Fall through.
        case StereoView::StretchedLeftRight:
            leftViewport.setWidth(viewport.width() / 2);
            rightViewport.setX(leftViewport.right() + 1);
            rightViewport.setWidth(viewport.width() - leftViewport.width());
            break;
        case StereoView::RightLeft:
            window.setWidth(window.width() / 2);
            // Fall through.
        case StereoView::StretchedRightLeft:
            rightViewport.setWidth(viewport.width() / 2);
            leftViewport.setX(rightViewport.right() + 1);
            leftViewport.setWidth(viewport.width() - rightViewport.width());
            break;
        case StereoView::TopBottom:
            window.setHeight(window.height() / 2);
            // Fall through.
        case StereoView::StretchedTopBottom:
            leftViewport.setHeight(viewport.height() / 2);
            rightViewport.setY(leftViewport.bottom() + 1);
            rightViewport.setHeight(viewport.height() - leftViewport.height());
            break;
        case StereoView::BottomTop:
            window.setHeight(window.height() / 2);
            // Fall through.
        case StereoView::StretchedBottomTop:
            rightViewport.setHeight(viewport.height() / 2);
            leftViewport.setY(rightViewport.bottom() + 1);
            leftViewport.setHeight(viewport.height() - rightViewport.height());
            break;
        }

        // Draw the left eye image into one half of the screen.
        m_view->setEye(QGL::LeftEye);
        painter->save();
        painter->setWindow(window);
        painter->setViewport(leftViewport);
        painter->setClipRect(window, Qt::IntersectClip);
        drawSource(painter);
        painter->restore();

        // Draw the right eye image into the other half of the screen.
        m_view->setEye(QGL::RightEye);
        painter->save();
        painter->setWindow(window);
        painter->setViewport(rightViewport);
        painter->setClipRect(window, Qt::IntersectClip);
        drawSource(painter);
        painter->restore();
        m_view->setEye(QGL::NoEye);
    } else {
        // Stereo rendering has been explicitly disabled.
        drawSource(painter);
    }
}
