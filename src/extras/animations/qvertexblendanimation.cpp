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

#include "qvertexblendanimation.h"

#include <private/qvertexblendanimation_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

QVertexBlendAnimationPrivate::QVertexBlendAnimationPrivate()
    : QAbstractAnimationPrivate(QAbstractAnimation::VertexBlendAnimation)
    , m_currentBase(nullptr)
    , m_currentTarget(nullptr)
{

}

void QVertexBlendAnimationPrivate::getAttributesInPosition(float position, int *target0,
                                                           int *target1, float *interpolator)
{
    if (position < m_targetPositions.first()) {
        *target0 = 0;
        *target1 = qMin(1, m_targetPositions.size());
        *interpolator = 0.0f;
    } else if (position > m_targetPositions.last()) {
        *target0 = qMax(m_targetPositions.size() - 2, 0);
        *target1 = qMax(m_targetPositions.size() - 1, 0);
        *interpolator = 1.0f;
    } else {
        for (int i = 0; i < m_targetPositions.size() - 1; i++) {
            if (position >= m_targetPositions[i] && position < m_targetPositions[i + 1]) {
                *target0 = i;
                *target1 = i + 1;
                float a = (position - m_targetPositions[i])
                            / (m_targetPositions[i + 1] - m_targetPositions[i]);
                *interpolator = a;
            }
        }
    }
}

static Qt3DRender::QAttribute *findAttribute(QVector<Qt3DRender::QAttribute *> &attributes,
                                             QString name)
{
    for (Qt3DRender::QAttribute *gattr : attributes) {
        if (gattr->name() == name)
            return gattr;
    }
    return nullptr;
}

void QVertexBlendAnimationPrivate::updateAnimation(float position)
{
    Q_Q(QVertexBlendAnimation);
    if (!m_target || !m_target->geometry())
        return;

    Qt3DExtras::QMorphTarget *base;
    Qt3DExtras::QMorphTarget *target;
    int target0, target1;
    float interpolator;
    getAttributesInPosition(position, &target0, &target1, &interpolator);

    base = m_morphTargets.at(target0);
    target = m_morphTargets.at(target1);

    Qt3DRender::QGeometry *geometry = m_target->geometry();

    // remove attributes from previous frame
    if ((m_currentBase && (base != m_currentBase))
        || (m_currentTarget && (target != m_currentTarget))) {
        const QVector<Qt3DRender::QAttribute *> baseAttributes = m_currentBase->attributeList();
        const QVector<Qt3DRender::QAttribute *> targetAttributes = m_currentTarget->attributeList();
        for (int i = 0; i < baseAttributes.size(); ++i) {
            geometry->removeAttribute(baseAttributes.at(i));
            geometry->removeAttribute(targetAttributes.at(i));
        }
    }

    const QVector<Qt3DRender::QAttribute *> baseAttributes = base->attributeList();
    const QVector<Qt3DRender::QAttribute *> targetAttributes = target->attributeList();
    const QStringList attributeNames = base->attributeNames();

    // add attributes from current frame to the geometry
    if (base != m_currentBase || target != m_currentTarget) {
        for (int i = 0; i < baseAttributes.size(); ++i) {
            const QString baseName = attributeNames.at(i);
            QString targetName = baseName;
            targetName.append("Target");

            baseAttributes[i]->setName(baseName);
            geometry->addAttribute(baseAttributes.at(i));
            targetAttributes[i]->setName(targetName);
            geometry->addAttribute(targetAttributes.at(i));
        }
    }
    m_currentBase = base;
    m_currentTarget = target;

    if (!qFuzzyCompare(interpolator, m_interpolator)) {
        m_interpolator = interpolator;
        emit q->interpolatorChanged(interpolator);
    }
}

QVertexBlendAnimation::QVertexBlendAnimation(QObject *parent)
    : QAbstractAnimation(*new QVertexBlendAnimationPrivate, parent)
{
    Q_D(QVertexBlendAnimation);
    d->m_positionConnection = QObject::connect(this, &QAbstractAnimation::positionChanged,
                                               this, &QVertexBlendAnimation::updateAnimation);
}

QVector<float> QVertexBlendAnimation::targetPositions() const
{
    Q_D(const QVertexBlendAnimation);
    return d->m_targetPositions;
}

float QVertexBlendAnimation::interpolator() const
{
    Q_D(const QVertexBlendAnimation);
    return d->m_interpolator;
}

Qt3DRender::QGeometryRenderer *QVertexBlendAnimation::target() const
{
    Q_D(const QVertexBlendAnimation);
    return d->m_target;
}

QString QVertexBlendAnimation::targetName() const
{
    Q_D(const QVertexBlendAnimation);
    return d->m_targetName;
}

void QVertexBlendAnimation::setMorphTargets(const QVector<Qt3DExtras::QMorphTarget *> &targets)
{
    Q_D(QVertexBlendAnimation);
    d->m_morphTargets = targets;
}

void QVertexBlendAnimation::addMorphTarget(Qt3DExtras::QMorphTarget *target)
{
    Q_D(QVertexBlendAnimation);
    if (!d->m_morphTargets.contains(target))
        d->m_morphTargets.push_back(target);
}

void QVertexBlendAnimation::removeMorphTarget(Qt3DExtras::QMorphTarget *target)
{
    Q_D(QVertexBlendAnimation);
    d->m_morphTargets.removeAll(target);
}

void QVertexBlendAnimation::setTargetPositions(const QVector<float> &targetPositions)
{
    Q_D(QVertexBlendAnimation);
    if (d->m_targetPositions == targetPositions)
        return;
    d->m_targetPositions = targetPositions;
    emit targetPositionsChanged(targetPositions);
    setDuration(d->m_targetPositions.last());
}

void QVertexBlendAnimation::setTarget(Qt3DRender::QGeometryRenderer *target)
{
    Q_D(QVertexBlendAnimation);
    if (d->m_target != target) {
        d->m_target = target;
        emit targetChanged(target);
    }
}

QVector<Qt3DExtras::QMorphTarget *> QVertexBlendAnimation::morphTargetList()
{
    Q_D(QVertexBlendAnimation);
    return d->m_morphTargets;
}

void QVertexBlendAnimation::setTargetName(const QString name)
{
    Q_D(QVertexBlendAnimation);
    if (d->m_targetName != name) {
        d->m_targetName = name;
        emit targetNameChanged(name);
    }
}

void QVertexBlendAnimation::updateAnimation(float position)
{
    Q_D(QVertexBlendAnimation);
    d->updateAnimation(position);
}

} // Qt3DExtras

QT_END_NAMESPACE
