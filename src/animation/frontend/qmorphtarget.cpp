/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

#include "qmorphtarget.h"
#include "Qt3DAnimation/private/qmorphtarget_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QMorphTargetPrivate::QMorphTargetPrivate()
    : QObjectPrivate()
{

}

void QMorphTargetPrivate::updateAttributeNames()
{
    m_attributeNames.clear();
    for (const Qt3DRender::QAttribute *attr : m_targetAttributes)
        m_attributeNames.push_back(attr->name());
}

QMorphTarget::QMorphTarget(QObject *parent)
    : QObject(*new QMorphTargetPrivate, parent)
{

}

QVector<Qt3DRender::QAttribute *> QMorphTarget::attributeList() const
{
    Q_D(const QMorphTarget);
    return d->m_targetAttributes;
}

QStringList QMorphTarget::attributeNames() const
{
    Q_D(const QMorphTarget);
    return d->m_attributeNames;
}

void QMorphTarget::setAttributes(const QVector<Qt3DRender::QAttribute *> &attributes)
{
    Q_D(QMorphTarget);
    d->m_targetAttributes = attributes;
    d->m_attributeNames.clear();
    for (const Qt3DRender::QAttribute *attr : attributes)
        d->m_attributeNames.push_back(attr->name());

    emit attributeNamesChanged(d->m_attributeNames);
}

void QMorphTarget::addAttribute(Qt3DRender::QAttribute *attribute)
{
    Q_D(QMorphTarget);
    for (const Qt3DRender::QAttribute *attr : d->m_targetAttributes) {
        if (attr->name() == attribute->name())
            return;
    }
    d->m_targetAttributes.push_back(attribute);
    d->m_attributeNames.push_back(attribute->name());
    emit attributeNamesChanged(d->m_attributeNames);
}

void QMorphTarget::removeAttribute(Qt3DRender::QAttribute *attribute)
{
    Q_D(QMorphTarget);
    if (d->m_targetAttributes.contains(attribute)) {
        d->m_targetAttributes.removeAll(attribute);
        d->updateAttributeNames();
        emit attributeNamesChanged(d->m_attributeNames);
    }
}

QMorphTarget *QMorphTarget::fromGeometry(Qt3DRender::QGeometry *geometry, const QStringList &attributes)
{
    QMorphTarget *target = new QMorphTarget();
    for (Qt3DRender::QAttribute *attr : geometry->attributes()) {
        if (attributes.contains(attr->name()))
            target->addAttribute(attr);
    }
    return target;
}

} // Qt3DAnimation

QT_END_NAMESPACE
