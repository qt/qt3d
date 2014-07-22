/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DRenderer/qparametermapper.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DRenderPass::Quick3DRenderPass(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QCriterion> Quick3DRenderPass::criteriaList()
{
    return QQmlListProperty<Qt3D::QCriterion>(this, 0,
                                                       &Quick3DRenderPass::appendCriteria,
                                                       &Quick3DRenderPass::criteriaCount,
                                                       &Quick3DRenderPass::criterionAt,
                                                       &Quick3DRenderPass::clearCriteria);
}

QQmlListProperty<QParameterMapper> Quick3DRenderPass::bindingList()
{
    return QQmlListProperty<Qt3D::QParameterMapper>(this, 0,
                                                   &Quick3DRenderPass::appendBinding,
                                                   &Quick3DRenderPass::bindingsCount,
                                                   &Quick3DRenderPass::bindingAt,
                                                   &Quick3DRenderPass::clearBindings);
}

void Quick3DRenderPass::appendCriteria(QQmlListProperty<QCriterion> *list, QCriterion *criterion)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    rPass->parentRenderPass()->addCriterion(criterion);
}

QCriterion *Quick3DRenderPass::criterionAt(QQmlListProperty<QCriterion> *list, int index)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->criteria().at(index);
}

int Quick3DRenderPass::criteriaCount(QQmlListProperty<QCriterion> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->criteria().count();
}

void Quick3DRenderPass::clearCriteria(QQmlListProperty<QCriterion> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    Q_FOREACH (QCriterion *c, rPass->parentRenderPass()->criteria())
        rPass->parentRenderPass()->removeCriterion(c);
}

void Quick3DRenderPass::appendBinding(QQmlListProperty<QParameterMapper> *list, QParameterMapper *binding)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    rPass->parentRenderPass()->addBinding(binding);
}

QParameterMapper *Quick3DRenderPass::bindingAt(QQmlListProperty<QParameterMapper> *list, int index)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->bindings().at(index);
}

int Quick3DRenderPass::bindingsCount(QQmlListProperty<QParameterMapper> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    return rPass->parentRenderPass()->bindings().count();
}

void Quick3DRenderPass::clearBindings(QQmlListProperty<QParameterMapper> *list)
{
    Quick3DRenderPass *rPass = qobject_cast<Quick3DRenderPass *>(list->object);
    Q_FOREACH (QParameterMapper *binding, rPass->parentRenderPass()->bindings())
        rPass->parentRenderPass()->removeBinding(binding);
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
