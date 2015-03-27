/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quick3drenderpassfilter.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DRenderPassFilter::Quick3DRenderPassFilter(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAnnotation> Quick3DRenderPassFilter::includeList()
{
    return QQmlListProperty<Qt3D::QAnnotation>(this, 0,
                                               &Quick3DRenderPassFilter::appendInclude,
                                               &Quick3DRenderPassFilter::includesCount,
                                               &Quick3DRenderPassFilter::includeAt,
                                               &Quick3DRenderPassFilter::clearIncludes);
}

QQmlListProperty<QParameter> Quick3DRenderPassFilter::parameterList()
{
    return QQmlListProperty<Qt3D::QParameter>(this, 0,
                                              &Quick3DRenderPassFilter::appendParameter,
                                              &Quick3DRenderPassFilter::parametersCount,
                                              &Quick3DRenderPassFilter::parameterAt,
                                              &Quick3DRenderPassFilter::clearParameterList);

}

void Quick3DRenderPassFilter::appendInclude(QQmlListProperty<QAnnotation> *list, QAnnotation *annotation)
{
    Quick3DRenderPassFilter *filter = qobject_cast<Quick3DRenderPassFilter *>(list->object);
    if (filter) {
        annotation->setParent(filter->parentRenderPassFilter());
        filter->parentRenderPassFilter()->addInclude(annotation);
    }
}

QAnnotation *Quick3DRenderPassFilter::includeAt(QQmlListProperty<QAnnotation> *list, int index)
{
    Quick3DRenderPassFilter *filter = qobject_cast<Quick3DRenderPassFilter *>(list->object);
    if (filter)
        return filter->parentRenderPassFilter()->includes().at(index);
    return 0;
}

int Quick3DRenderPassFilter::includesCount(QQmlListProperty<QAnnotation> *list)
{
    Quick3DRenderPassFilter *filter = qobject_cast<Quick3DRenderPassFilter *>(list->object);
    if (filter)
        return filter->parentRenderPassFilter()->includes().count();
    return 0;
}

void Quick3DRenderPassFilter::clearIncludes(QQmlListProperty<QAnnotation> *list)
{
    Quick3DRenderPassFilter *filter = qobject_cast<Quick3DRenderPassFilter *>(list->object);
    if (filter) {
        Q_FOREACH (QAnnotation *criterion, filter->parentRenderPassFilter()->includes())
            filter->parentRenderPassFilter()->removeInclude(criterion);
    }
}

void Quick3DRenderPassFilter::appendParameter(QQmlListProperty<QParameter> *list, QParameter *param)
{
    Quick3DRenderPassFilter *rPassFilter = qobject_cast<Quick3DRenderPassFilter *>(list->object);
    rPassFilter->parentRenderPassFilter()->addParameter(param);
}

QParameter *Quick3DRenderPassFilter::parameterAt(QQmlListProperty<QParameter> *list, int index)
{
    Quick3DRenderPassFilter *rPassFilter = qobject_cast<Quick3DRenderPassFilter *>(list->object);
    return rPassFilter->parentRenderPassFilter()->parameters().at(index);
}

int Quick3DRenderPassFilter::parametersCount(QQmlListProperty<QParameter> *list)
{
    Quick3DRenderPassFilter *rPassFilter = qobject_cast<Quick3DRenderPassFilter *>(list->object);
    return rPassFilter->parentRenderPassFilter()->parameters().count();
}

void Quick3DRenderPassFilter::clearParameterList(QQmlListProperty<QParameter> *list)
{
    Quick3DRenderPassFilter *rPassFilter = qobject_cast<Quick3DRenderPassFilter *>(list->object);
    Q_FOREACH (QParameter *p, rPassFilter->parentRenderPassFilter()->parameters())
        rPassFilter->parentRenderPassFilter()->removeParameter(p);
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
