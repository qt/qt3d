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
        emit technique->renderPassesChanged();
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
        Q_FOREACH (QAbstractRenderPass *pass, technique->parentTechnique()->renderPasses())
            technique->parentTechnique()->removePass(pass);
        emit technique->renderPassesChanged();
    }
}

QQmlListProperty<Qt3D::TechniqueCriterion> Quick3DTechnique::criteriaList()
{
    return QQmlListProperty<Qt3D::TechniqueCriterion>(this, 0,
                                       &Quick3DTechnique::appendCriterion,
                                       &Quick3DTechnique::criteriaCount,
                                       &Quick3DTechnique::criterionAt,
                                       &Quick3DTechnique::clearCriteriaList);
}

void Quick3DTechnique::appendCriterion(QQmlListProperty<TechniqueCriterion> *list, TechniqueCriterion *criterion)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        if (!criterion->parent())
            criterion->setParent(technique->parentTechnique());
        technique->parentTechnique()->addCriterion(criterion);
        emit technique->criteriaChanged();
    }
}

TechniqueCriterion *Quick3DTechnique::criterionAt(QQmlListProperty<TechniqueCriterion> *list, int index)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique)
        return technique->parentTechnique()->criteria().at(index);
    return 0;
}

int Quick3DTechnique::criteriaCount(QQmlListProperty<TechniqueCriterion> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique)
        return technique->parentTechnique()->criteria().size();
    return 0;
}

void Quick3DTechnique::clearCriteriaList(QQmlListProperty<TechniqueCriterion> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        technique->parentTechnique()->clearCriteria();
        emit technique->criteriaChanged();
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
