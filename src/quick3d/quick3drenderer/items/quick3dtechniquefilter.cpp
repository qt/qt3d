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

#include "quick3dtechniquefilter.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DTechniqueFilter::Quick3DTechniqueFilter(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3D::QAnnotation> Quick3DTechniqueFilter::requireList()
{
    return QQmlListProperty<Qt3D::QAnnotation>(this, 0,
                                               &Quick3DTechniqueFilter::appendRequire,
                                               &Quick3DTechniqueFilter::requiresCount,
                                               &Quick3DTechniqueFilter::requireAt,
                                               &Quick3DTechniqueFilter::clearRequires);
}

QQmlListProperty<QParameter> Quick3DTechniqueFilter::parameterList()
{
    return QQmlListProperty<Qt3D::QParameter>(this, 0,
                                             &Quick3DTechniqueFilter::appendParameter,
                                             &Quick3DTechniqueFilter::parametersCount,
                                             &Quick3DTechniqueFilter::parameterAt,
                                             &Quick3DTechniqueFilter::clearParameterList);
}

void Quick3DTechniqueFilter::appendRequire(QQmlListProperty<QAnnotation> *list, QAnnotation *criterion)
{
    Quick3DTechniqueFilter *filter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
    if (filter) {
        criterion->setParent(filter->parentTechniqueFilter());
        filter->parentTechniqueFilter()->addRequirement(criterion);
    }
}

QAnnotation *Quick3DTechniqueFilter::requireAt(QQmlListProperty<QAnnotation> *list, int index)
{
    Quick3DTechniqueFilter *filter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
    if (filter)
        return filter->parentTechniqueFilter()->criteria().at(index);
    return 0;
}

int Quick3DTechniqueFilter::requiresCount(QQmlListProperty<QAnnotation> *list)
{
    Quick3DTechniqueFilter *filter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
    if (filter)
        return filter->parentTechniqueFilter()->criteria().size();
    return 0;
}

void Quick3DTechniqueFilter::clearRequires(QQmlListProperty<QAnnotation> *list)
{
    Quick3DTechniqueFilter *filter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
    if (filter) {
        Q_FOREACH (QAnnotation *criterion, filter->parentTechniqueFilter()->criteria())
            filter->parentTechniqueFilter()->removeRequirement(criterion);
    }
}

void Quick3DTechniqueFilter::appendParameter(QQmlListProperty<QParameter> *list, QParameter *param)
{
    Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
    techniqueFilter->parentTechniqueFilter()->addParameter(param);
}

QParameter *Quick3DTechniqueFilter::parameterAt(QQmlListProperty<QParameter> *list, int index)
{
    Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
    return techniqueFilter->parentTechniqueFilter()->parameters().at(index);
}

int Quick3DTechniqueFilter::parametersCount(QQmlListProperty<QParameter> *list)
{
    Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
    return techniqueFilter->parentTechniqueFilter()->parameters().count();
}

void Quick3DTechniqueFilter::clearParameterList(QQmlListProperty<QParameter> *list)
{
    Quick3DTechniqueFilter *techniqueFilter = qobject_cast<Quick3DTechniqueFilter *>(list->object);
    Q_FOREACH (QParameter *p, techniqueFilter->parentTechniqueFilter()->parameters())
        techniqueFilter->parentTechniqueFilter()->removeParameter(p);
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
