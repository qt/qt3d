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

#include "qparameter.h"
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <private/qparameter_p.h>
#include <Qt3DRenderer/qtexture.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {


/*!
    \class Qt3D::QParameterPrivate
    \internal
*/
QParameterPrivate::QParameterPrivate()
    : QNodePrivate()
{
}

void QParameterPrivate::setValue(const QVariant &v)
{
    m_value = v;
}

void QParameter::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QParameter *param = static_cast<const QParameter*>(ref);
    d_func()->m_name = param->d_func()->m_name;
    d_func()->m_value = param->d_func()->m_value;
}

/*! \internal */
QParameter::QParameter(QParameterPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QParameter::QParameter(QNode *parent)
    : QNode(*new QParameterPrivate, parent)
{
}

QParameter::~QParameter()
{
    QNode::cleanup();
}

QParameter::QParameter(const QString &name, const QVariant &value, QNode *parent)
    : QNode(*new QParameterPrivate, parent)
{
    Q_D(QParameter);
    d->m_name = name;
    setValue(value);
}

QParameter::QParameter(const QString &name, QAbstractTextureProvider *texture, QNode *parent)
    : QNode(*new QParameterPrivate, parent)
{
    Q_D(QParameter);
    d->m_name = name;
    setValue(QVariant::fromValue(texture));
}


void QParameter::setName(const QString &name)
{
    Q_D(QParameter);
    if (d->m_name != name) {
        d->m_name = name;
        emit nameChanged();
    }
}

QString QParameter::name() const
{
    Q_D(const QParameter);
    return d->m_name;
}

void QParameter::setValue(const QVariant &dv)
{
    Q_D(QParameter);
    if (d->m_value != dv) {
        d->setValue(dv);
        emit valueChanged();

        // In case texture are declared inline
        QNode *txt = dv.value<QNode *>();
        if (txt != Q_NULLPTR && !txt->parent())
           txt->setParent(this);

        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, id()));
        change->setPropertyName(d->m_name.toUtf8().data());
        if (txt != Q_NULLPTR)
            change->setValue(QVariant::fromValue(QNode::clone(txt)));
        else
            change->setValue(d->m_value);

        d->notifyObservers(change);
    }
}

QVariant QParameter::value() const
{
    Q_D(const QParameter);
    return d->m_value;
}

} // Qt3D

QT_END_NAMESPACE
