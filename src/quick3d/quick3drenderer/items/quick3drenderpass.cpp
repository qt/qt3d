/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#include "quick3drenderpass.h"
#include <Qt3DRenderer/qparametermapping.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DRenderPass::Quick3DRenderPass(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAnnotation> Quick3DRenderPass::annotationList()
{
    return QQmlListProperty<Qt3D::QAnnotation>(this, 0,
                                              &Quick3DRenderPass::appendAnnotation,
                                              &Quick3DRenderPass::annotationsCount,
                                              &Quick3DRenderPass::annotationAt,
                                              &Quick3DRenderPass::clearAnnotations);
}

QQmlListProperty<QParameterMapping> Quick3DRenderPass::bindingList()
{
    return QQmlListProperty<Qt3D::QParameterMapping>(this, 0,
                                                    &Quick3DRenderPass::appendBinding,
                                                    &Quick3DRenderPass::bindingsCount,
                                                    &Quick3DRenderPass::bindingAt,
                                                    &Quick3DRenderPass::clearBindings);
}

QQmlListProperty<QRenderState> Quick3DRenderPass::renderStateList()
{
    return QQmlListProperty<Qt3D::QRenderState>(this, 0,
                                              &Quick3DRenderPass::appendRenderState,
                                              &Quick3DRenderPass::renderStateCount,
                                              &Quick3DRenderPass::renderStateAt,
                                              &Quick3DRenderPass::clearRenderStates);
}

void Quick3DRenderPass::appendAnnotation(QQmlListProperty<QAnnotation> *list, QAnnotation *annotation)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    rPass->parentRenderPass()->addAnnotation(annotation);
}

QAnnotation *Quick3DRenderPass::annotationAt(QQmlListProperty<QAnnotation> *list, int index)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->annotations().at(index);
}

int Quick3DRenderPass::annotationsCount(QQmlListProperty<QAnnotation> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->annotations().count();
}

void Quick3DRenderPass::clearAnnotations(QQmlListProperty<QAnnotation> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    Q_FOREACH (QAnnotation *c, rPass->parentRenderPass()->annotations())
        rPass->parentRenderPass()->removeAnnotation(c);
}

void Quick3DRenderPass::appendBinding(QQmlListProperty<QParameterMapping> *list, QParameterMapping *binding)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    rPass->parentRenderPass()->addBinding(binding);
}

QParameterMapping *Quick3DRenderPass::bindingAt(QQmlListProperty<QParameterMapping> *list, int index)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->bindings().at(index);
}

int Quick3DRenderPass::bindingsCount(QQmlListProperty<QParameterMapping> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->bindings().count();
}

void Quick3DRenderPass::clearBindings(QQmlListProperty<QParameterMapping> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    Q_FOREACH (QParameterMapping *binding, rPass->parentRenderPass()->bindings())
        rPass->parentRenderPass()->removeBinding(binding);
}

void Quick3DRenderPass::appendRenderState(QQmlListProperty<QRenderState> *list, QRenderState *state)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    rPass->parentRenderPass()->addRenderState(state);
}

QRenderState *Quick3DRenderPass::renderStateAt(QQmlListProperty<QRenderState> *list, int index)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->renderStates().at(index);
}

int Quick3DRenderPass::renderStateCount(QQmlListProperty<QRenderState> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->renderStates().count();
}

void Quick3DRenderPass::clearRenderStates(QQmlListProperty<QRenderState> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    Q_FOREACH (QRenderState *s, rPass->parentRenderPass()->renderStates())
        rPass->parentRenderPass()->removeRenderState(s);
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
