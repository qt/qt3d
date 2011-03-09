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

#include "floatingitemeffect.h"
#include "floatingitem.h"
#include "stereoview.h"
#include <QtGui/private/qgraphicseffect_p.h>
#include <QtGui/private/qgraphicsitem_p.h>
#include <QtGui/qdesktopwidget.h>
#include <QtGui/qapplication.h>

FloatingItemEffect::FloatingItemEffect(FloatingItem *parent)
    : QGraphicsEffect(parent)
    , m_item(parent)
    , m_sourced(0)
{
}

FloatingItemEffect::~FloatingItemEffect()
{
}

QRectF FloatingItemEffect::boundingRectFor(const QRectF &sourceRect) const
{
    qreal depth = qAbs(m_item->depth());
    depth = depth * qApp->desktop()->logicalDpiX() / 100.0f;
    return sourceRect.adjusted(-depth, -depth, depth, depth);
}

void FloatingItemEffect::draw(QPainter *painter)
{
    // Correct the depth value for the screen's DPI.  We treat 100 DPI
    // as "normal" and scale the depth value accordingly.  This way,
    // the same number of millimeters are used on all displays viewed
    // at the same viewing distance.  A depth of 1 is 0.254 millimeters.
    // Note: we should probably correct for viewing distance also.
    qreal depth = m_item->depth() * painter->device()->logicalDpiX() / 100.0f;

    // Determine which eye is being rendered by the StereoView.
    StereoView *view = StereoView::findView(m_item);
    QGL::Eye eye = view ? view->eye() : QGL::NoEye;
    if (eye == QGL::NoEye || depth == 0.0f) {
        // No eye being rendered or zero depth, so draw source as-is.
        drawSource(painter);
    } else if (eye == QGL::LeftEye) {
        // Modify the effectTransform to adjust the position
        // and draw the left eye version of the source.
        QGraphicsItemPaintInfo *drawContext = m_sourced->info;
        const QTransform *origTransform = drawContext->effectTransform;
        QTransform transform(Qt::Uninitialized);
        if (!origTransform) {
            transform = QTransform::fromTranslate(depth / 2.0f, 0.0f);
        } else {
            transform = *origTransform;
            transform.translate(depth / 2.0f, 0.0f);
        }
        drawContext->effectTransform = &transform;
        drawSource(painter);
        drawContext->effectTransform = origTransform;
    } else {
        // Modify the effectTransform to adjust the position
        // and draw the right eye version of the source.
        QGraphicsItemPaintInfo *drawContext = m_sourced->info;
        const QTransform *origTransform = drawContext->effectTransform;
        QTransform transform(Qt::Uninitialized);
        if (!origTransform) {
            transform = QTransform::fromTranslate(-depth / 2.0f, 0.0f);
        } else {
            transform = *origTransform;
            transform.translate(-depth / 2.0f, 0.0f);
        }
        drawContext->effectTransform = &transform;
        drawSource(painter);
        drawContext->effectTransform = origTransform;
    }
}

void FloatingItemEffect::sourceChanged(QGraphicsEffect::ChangeFlags flags)
{
    if (flags & SourceAttached) {
        QGraphicsEffectPrivate *ep =
            static_cast<QGraphicsEffectPrivate *>
                (QObjectPrivate::get(this));
        m_sourced = static_cast<QGraphicsItemEffectSourcePrivate *>
            (QObjectPrivate::get(ep->source));
    } else if (flags & SourceDetached) {
        m_sourced = 0;
    }
}
