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

#include "qmorphinganimation.h"
#include <private/qmorphinganimation_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QMorphingAnimationPrivate::QMorphingAnimationPrivate()
    : QAbstractAnimationPrivate(QAbstractAnimation::MorphingAnimation)
    , m_minposition(0.0f)
    , m_maxposition(0.0f)
    , m_flattened(nullptr)
    , m_method(QMorphingAnimation::Relative)
    , m_interpolator(0.0f)
    , m_target(nullptr)
    , m_currentTarget(nullptr)
{

}

QMorphingAnimationPrivate::~QMorphingAnimationPrivate()
{
    for (QVector<float> *weights : m_weights)
        delete weights;
}

void QMorphingAnimationPrivate::updateAnimation(float position)
{
    Q_Q(QMorphingAnimation);
    if (!m_target || !m_target->geometry())
        return;

    QVector<int> relevantValues;
    float sum = 0.0f;
    float interpolator;
    m_morphKey.resize(m_morphTargets.size());

    // calculate morph key
    if (position < m_minposition) {
        m_morphKey = *m_weights.first();
    } else if (position >= m_maxposition) {
        m_morphKey = *m_weights.last();
    } else {
        for (int i = 0; i < m_targetPositions.size() - 1; ++i) {
            if (position >= m_targetPositions.at(i) && position < m_targetPositions.at(i + 1)) {
                interpolator = (position - m_targetPositions.at(i))
                        / (m_targetPositions.at(i + 1) - m_targetPositions.at(i));
                interpolator = m_easing.valueForProgress(interpolator);
                float iip = 1.0f - interpolator;

                for (int j = 0; j < m_morphTargets.size(); ++j) {
                    m_morphKey[j] = interpolator * m_weights.at(i + 1)->at(j)
                                        + iip * m_weights.at(i)->at(j);
                }
            }
        }
    }

    // check relevant values
    for (int j = 0; j < m_morphKey.size(); ++j) {
        sum += m_morphKey[j];
        if (!qFuzzyIsNull(m_morphKey[j]))
            relevantValues.push_back(j);
    }

    if (relevantValues.size() == 0 || qFuzzyIsNull(sum)) {
        // only base is used
        interpolator = 0.0f;
    } else if (relevantValues.size() == 1) {
        // one morph target has non-zero weight
        setTargetInterpolated(relevantValues[0]);
        interpolator = sum;
    } else {
        // more than one morph target has non-zero weight
        // flatten morph targets to one
        qWarning() << Q_FUNC_INFO << "Flattening required";
    }

    // Relative method uses negative interpolator, normalized uses positive
    if (m_method == QMorphingAnimation::Relative)
        interpolator = -interpolator;

    if (!qFuzzyCompare(interpolator, m_interpolator)) {
        m_interpolator = interpolator;
        emit q->interpolatorChanged(m_interpolator);
    }
}

void QMorphingAnimationPrivate::setTargetInterpolated(int morphTarget)
{
    QMorphTarget *target = m_morphTargets[morphTarget];
    Qt3DRender::QGeometry *geometry = m_target->geometry();

    // remove attributes from previous frame
    if (m_currentTarget && (target != m_currentTarget)) {
        const QVector<Qt3DRender::QAttribute *> targetAttributes = m_currentTarget->attributeList();
        for (int i = 0; i < targetAttributes.size(); ++i)
            geometry->removeAttribute(targetAttributes.at(i));
    }

    const QVector<Qt3DRender::QAttribute *> targetAttributes = target->attributeList();

    // add attributes from current frame to the geometry
    if (target != m_currentTarget) {
        for (int i = 0; i < m_attributeNames.size(); ++i) {
            QString targetName = m_attributeNames.at(i);
            targetName.append(QLatin1String("Target"));
            targetAttributes[i]->setName(targetName);
            geometry->addAttribute(targetAttributes.at(i));
        }
    }
    m_currentTarget = target;
}

QMorphingAnimation::QMorphingAnimation(QObject *parent)
    : QAbstractAnimation(*new QMorphingAnimationPrivate, parent)
{
    Q_D(QMorphingAnimation);
    d->m_positionConnection = QObject::connect(this, &QAbstractAnimation::positionChanged,
                                               this, &QMorphingAnimation::updateAnimation);
}

QVector<float> QMorphingAnimation::targetPositions() const
{
    Q_D(const QMorphingAnimation);
    return d->m_targetPositions;
}

float QMorphingAnimation::interpolator() const
{
    Q_D(const QMorphingAnimation);
    return d->m_interpolator;
}

Qt3DRender::QGeometryRenderer *QMorphingAnimation::target() const
{
    Q_D(const QMorphingAnimation);
    return d->m_target;
}

QString QMorphingAnimation::targetName() const
{
    Q_D(const QMorphingAnimation);
    return d->m_targetName;
}

QMorphingAnimation::Method QMorphingAnimation::method() const
{
    Q_D(const QMorphingAnimation);
    return d->m_method;
}

QEasingCurve QMorphingAnimation::easing() const
{
    Q_D(const QMorphingAnimation);
    return d->m_easing;
}

void QMorphingAnimation::setMorphTargets(const QVector<Qt3DAnimation::QMorphTarget *> &targets)
{
    Q_D(QMorphingAnimation);
    d->m_morphTargets = targets;
    d->m_attributeNames = targets[0]->attributeNames();
    d->m_position = -1.0f;
}

void QMorphingAnimation::addMorphTarget(Qt3DAnimation::QMorphTarget *target)
{
    Q_D(QMorphingAnimation);
    if (!d->m_morphTargets.contains(target)) {
        d->m_morphTargets.push_back(target);
        d->m_position = -1.0f;
        if (d->m_attributeNames.empty())
            d->m_attributeNames = target->attributeNames();
    }
}

void QMorphingAnimation::removeMorphTarget(Qt3DAnimation::QMorphTarget *target)
{
    Q_D(QMorphingAnimation);
    d->m_morphTargets.removeAll(target);
    d->m_position = -1.0f;
}

void QMorphingAnimation::setTargetPositions(const QVector<float> &targetPositions)
{
    Q_D(QMorphingAnimation);
    d->m_targetPositions = targetPositions;
    emit targetPositionsChanged(targetPositions);
    d->m_minposition = targetPositions.first();
    d->m_maxposition = targetPositions.last();
    setDuration(d->m_targetPositions.last());
    if (d->m_weights.size() < targetPositions.size()) {
        d->m_weights.resize(targetPositions.size());
        for (int i = 0; i < d->m_weights.size(); ++i) {
            if (d->m_weights[i] == nullptr)
                d->m_weights[i] = new QVector<float>();
        }
    }
    d->m_position = -1.0f;
}

void QMorphingAnimation::setTarget(Qt3DRender::QGeometryRenderer *target)
{
    Q_D(QMorphingAnimation);
    if (d->m_target != target) {
        d->m_position = -1.0f;
        d->m_target = target;
        emit targetChanged(target);
    }
}

void QMorphingAnimation::setWeights(int positionIndex, const QVector<float> &weights)
{
    Q_D(QMorphingAnimation);
    if (d->m_weights.size() < positionIndex)
        d->m_weights.resize(positionIndex + 1);
    if (d->m_weights[positionIndex] == nullptr)
        d->m_weights[positionIndex] = new QVector<float>();
    *d->m_weights[positionIndex] = weights;
    d->m_position = -1.0f;
}

QVector<float> QMorphingAnimation::getWeights(int positionIndex)
{
    Q_D(QMorphingAnimation);
    return *d->m_weights[positionIndex];
}

QVector<Qt3DAnimation::QMorphTarget *> QMorphingAnimation::morphTargetList()
{
    Q_D(QMorphingAnimation);
    return d->m_morphTargets;
}

void QMorphingAnimation::setTargetName(const QString name)
{
    Q_D(QMorphingAnimation);
    if (d->m_targetName != name) {
        d->m_targetName = name;
        emit targetNameChanged(name);
    }
}

void QMorphingAnimation::setMethod(QMorphingAnimation::Method method)
{
    Q_D(QMorphingAnimation);
    if (d->m_method != method) {
        d->m_method = method;
        d->m_position = -1.0f;
        emit methodChanged(method);
    }
}

void QMorphingAnimation::setEasing(const QEasingCurve &easing)
{
    Q_D(QMorphingAnimation);
    if (d->m_easing != easing) {
        d->m_easing = easing;
        d->m_position = -1.0f;
        emit easingChanged(easing);
    }
}

void QMorphingAnimation::updateAnimation(float position)
{
    Q_D(QMorphingAnimation);
    d->updateAnimation(position);
}

} // Qt3DAnimation

QT_END_NAMESPACE
