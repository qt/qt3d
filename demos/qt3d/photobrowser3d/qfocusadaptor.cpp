/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#include "qfocusadaptor.h"
#include "qglscenenode.h"
#include "qglview.h"
#include "qglcamera.h"
#include "thumbnailnode.h"

class QFocusAdaptorPrivate
{
public:
    QFocusAdaptorPrivate()
        : progress(0.0)
        , reset(true)
        , target(0)
        , view(0)
    {}
    qreal progress;
    QVector3D targetEye;
    QVector3D targetCenter;
    QVector3D sourceEye;
    QVector3D sourceCenter;
    bool reset;
    QGLSceneNode *target;
    QGLView *view;
};

QFocusAdaptor::QFocusAdaptor(QObject *parent)
    : QObject(parent)
    , d(new QFocusAdaptorPrivate)
{
    QGLView *v = qobject_cast<QGLView*>(parent);
    if (v)
        d->view = v;
}

QFocusAdaptor::~QFocusAdaptor()
{
    delete d;
}

qreal QFocusAdaptor::progress() const
{
    return d->progress;
}

void QFocusAdaptor::setProgress(qreal progress)
{
    if (d->progress != progress)
    {
        d->progress = progress;
        calculateValues();
        emit progressChanged();
    }
}

QGLView *QFocusAdaptor::view() const
{
    return d->view;
}

void QFocusAdaptor::setView(QGLView *view)
{
    d->view = view;
    d->reset = true;
}

QGLSceneNode *QFocusAdaptor::target() const
{
    return d->target;
}

void QFocusAdaptor::setTarget(QGLSceneNode *target)
{
    if (d->target != target)
    {
        d->target = target;
        d->reset = true;
    }
}

void QFocusAdaptor::calculateValues()
{
    if (d->target && d->view)
    {
        QGLCamera *cam = d->view->camera();
        Q_ASSERT(cam);
        if (d->reset)
        {
            QGeometryData data = d->target->geometry();
            if (data.count() == 0 || d->target->count() == 0)
            {
                qWarning("Could not setup focus animation");
                return;
            }
            // assume that the first triangle referenced by this node is the one
            // you want to focus on - works for simple rects and like cases
            QGL::IndexArray ixs = data.indices();
            QVector3D a = data.vertexAt(ixs.at(d->target->start()));
            QVector3D b = data.vertexAt(ixs.at(d->target->start() + 1));
            QVector3D c = data.vertexAt(ixs.at(d->target->start() + 2));

            // assumes that first triangle is facing the camera
            QVector3D toCam = QVector3D::normal(a, b, c);

            // wont work very well if the target is not axis-aligned
            // find the distance q for the eye to be away from this object
            // in order that it is a tight fit in the viewport
            QGeometryData g = d->target->geometry();
            QGL::IndexArray inxs = g.indices();
            QBox3D box;
            for (int i = d->target->start(); i < (d->target->start() + d->target->count()); ++i)
                box.unite(g.vertexAt(inxs.at(i)));
            QVector3D sz = box.size();

            qreal nearDist = cam->nearPlane();

            QSizeF v = cam->viewSize();

            qreal vh = d->view->geometry().height();
            qreal vw = d->view->geometry().width();
            if (!qFuzzyIsNull(vw - vh))
            {
                qreal asp = vh / vw;
                if (vh > vw)
                    v.setHeight(v.height() * asp);
                else
                    v.setWidth(v.width() / asp);
            }

            qreal qh = (nearDist * sz.y()) / v.height();
            qreal qw = (nearDist * sz.x()) / v.width();

            qreal q = qMax(qh, qw);
            q = qMax(q, qreal(nearDist * 1.05));

            d->sourceCenter = cam->center();
            d->sourceEye = cam->eye();

            d->targetCenter = d->target->position();
            d->targetEye = d->targetCenter + (toCam * q);

            d->reset = false;
        }
        cam->setCenter(d->sourceCenter + ((d->targetCenter - d->sourceCenter) * d->progress));
        cam->setEye(d->sourceEye + ((d->targetEye - d->sourceEye) * d->progress));
    }
}
