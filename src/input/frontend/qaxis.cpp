/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
****************************************************************************/

#include "qaxis.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DInput/qaxisinput.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisPrivate : public Qt3DCore::QNodePrivate
{
public:
    QAxisPrivate()
        : Qt3DCore::QNodePrivate()
        , m_value(0.0f)
    {}

    Q_DECLARE_PUBLIC(QAxis)

    QString m_name;
    QVector<QAxisInput *> m_inputs;
    float m_value;

    void setValue(float value)
    {
        if (value != m_value) {
            m_value = value;
            q_func()->valueChanged(m_value);
        }
    }
};

/*!
 * \qmltype Axis
 * \inqmlmodule Qt3D.Input
 * \since 5.5
 * \TODO
 *
 */

/*!
 * \class Qt3DInput::QAxis
 * \inmodule Qt3DInput
 * \since 5.5
 * \TODO
 *
 */

QAxis::QAxis(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QAxisPrivate(), parent)
{
}

QAxis::~QAxis()
{
    QNode::cleanup();
}

void QAxis::setName(const QString &name)
{
    Q_D(QAxis);
    if (d->m_name != name) {
        d->m_name = name;
        emit nameChanged(name);
    }
}

QString QAxis::name() const
{
    Q_D(const QAxis);
    return d->m_name;
}

void QAxis::addInput(QAxisInput *input)
{
    Q_D(QAxis);
    if (!d->m_inputs.contains(input)) {
        d->m_inputs.push_back(input);

        if (!input->parent())
            input->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            Qt3DCore::QScenePropertyChangePtr change(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeAdded, Qt3DCore::QSceneChange::Node, id()));
            change->setPropertyName("input");
            change->setValue(QVariant::fromValue(input->id()));
            d->notifyObservers(change);
        }
    }
}

void QAxis::removeInput(QAxisInput *input)
{
    Q_D(QAxis);
    if (d->m_inputs.contains(input)) {

        if (d->m_changeArbiter != Q_NULLPTR) {
            Qt3DCore::QScenePropertyChangePtr change(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeRemoved, Qt3DCore::QSceneChange::Node, id()));
            change->setPropertyName("input");
            change->setValue(QVariant::fromValue(input->id()));
            d->notifyObservers(change);
        }

        d->m_inputs.removeOne(input);
    }
}

QVector<QAxisInput *> QAxis::inputs() const
{
    Q_D(const QAxis);
    return d->m_inputs;
}

float QAxis::value() const
{
    Q_D(const QAxis);
    return d->m_value;
}

void QAxis::copy(const Qt3DCore::QNode *ref)
{
    QNode::copy(ref);
    const QAxis *axis = static_cast<const QAxis *>(ref);
    d_func()->m_name = axis->d_func()->m_name;
    Q_FOREACH (QAxisInput *input, axis->inputs())
        d_func()->m_inputs.append(qobject_cast<QAxisInput *>(QNode::clone(input)));

}

void QAxis::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QAxis);
    Qt3DCore::QScenePropertyChangePtr e = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(change);
    if (e->type() == Qt3DCore::NodeUpdated && e->propertyName() == QByteArrayLiteral("value")) {
        d->setValue(e->value().toFloat());
    }
}

} // Qt3DInput

QT_END_NAMESPACE
