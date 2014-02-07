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
    float progress;
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

float QFocusAdaptor::progress() const
{
    return d->progress;
}

void QFocusAdaptor::setProgress(float progress)
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

            float nearDist = cam->nearPlane();

            QSizeF v = cam->viewSize();

            float vh(d->view->geometry().height());
            float vw(d->view->geometry().width());
            if (!qFuzzyIsNull(vw - vh))
            {
                float asp = vh / vw;
                if (vh > vw)
                    v.setHeight(v.height() * asp);
                else
                    v.setWidth(v.width() / asp);
            }

            float qh = (nearDist * sz.y()) / v.height();
            float qw = (nearDist * sz.x()) / v.width();

            float q = qMax(qh, qw);
            q = qMax(q, float(nearDist * 1.05f));

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
