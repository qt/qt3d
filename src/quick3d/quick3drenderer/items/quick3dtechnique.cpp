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

#include "quick3dtechnique.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DTechnique::Quick3DTechnique(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3D::QRenderPass> Quick3DTechnique::renderPassList()
{
    return QQmlListProperty<Qt3D::QRenderPass>(this, 0,
                                              &Quick3DTechnique::appendRenderPass,
                                              &Quick3DTechnique::renderPassCount,
                                              &Quick3DTechnique::renderPassAt,
                                              &Quick3DTechnique::clearRenderPasses);
}

QQmlListProperty<QParameter> Quick3DTechnique::parameterList()
{
    return QQmlListProperty<Qt3D::QParameter>(this, 0,
                                             &Quick3DTechnique::appendParameter,
                                             &Quick3DTechnique::parametersCount,
                                             &Quick3DTechnique::parameterAt,
                                             &Quick3DTechnique::clearParameterList);
}

void Quick3DTechnique::appendParameter(QQmlListProperty<QParameter> *list, QParameter *param)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    technique->parentTechnique()->addParameter(param);
}

QParameter *Quick3DTechnique::parameterAt(QQmlListProperty<QParameter> *list, int index)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    return technique->parentTechnique()->parameters().at(index);
}

int Quick3DTechnique::parametersCount(QQmlListProperty<QParameter> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    return technique->parentTechnique()->parameters().count();
}

void Quick3DTechnique::clearParameterList(QQmlListProperty<QParameter> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    Q_FOREACH (QParameter *p, technique->parentTechnique()->parameters())
        technique->parentTechnique()->removeParameter(p);
}

void Quick3DTechnique::appendRenderPass(QQmlListProperty<QRenderPass> *list, QRenderPass *renderPass)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        technique->parentTechnique()->addPass(renderPass);
    }
}

QRenderPass *Quick3DTechnique::renderPassAt(QQmlListProperty<QRenderPass> *list, int index)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique)
        return qobject_cast<QRenderPass *>(technique->parentTechnique()->renderPasses().at(index));
    return 0;
}

int Quick3DTechnique::renderPassCount(QQmlListProperty<QRenderPass> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique)
        return technique->parentTechnique()->renderPasses().size();
    return 0;
}

void Quick3DTechnique::clearRenderPasses(QQmlListProperty<QRenderPass> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        Q_FOREACH (QRenderPass *pass, technique->parentTechnique()->renderPasses())
            technique->parentTechnique()->removePass(pass);
    }
}

QQmlListProperty<Qt3D::QAnnotation> Quick3DTechnique::annotationList()
{
    return QQmlListProperty<Qt3D::QAnnotation>(this, 0,
                                       &Quick3DTechnique::appendAnnotation,
                                       &Quick3DTechnique::annotationCount,
                                       &Quick3DTechnique::annotationAt,
                                       &Quick3DTechnique::clearAnnotationList);
}

void Quick3DTechnique::appendAnnotation(QQmlListProperty<QAnnotation> *list, QAnnotation *annotation)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        if (!annotation->parent())
            annotation->setParent(technique->parentTechnique());
        technique->parentTechnique()->addAnnotation(annotation);
    }
}

QAnnotation *Quick3DTechnique::annotationAt(QQmlListProperty<QAnnotation> *list, int index)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique)
        return technique->parentTechnique()->annotations().at(index);
    return 0;
}

int Quick3DTechnique::annotationCount(QQmlListProperty<QAnnotation> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique)
        return technique->parentTechnique()->annotations().size();
    return 0;
}

void Quick3DTechnique::clearAnnotationList(QQmlListProperty<QAnnotation> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        Q_FOREACH (QAnnotation *a, technique->parentTechnique()->annotations())
            technique->parentTechnique()->removeAnnotation(a);
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
