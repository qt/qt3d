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

#include "qparametermapper.h"
#include <private/qparametermapper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QParameterMapperPrivate::QParameterMapperPrivate(QParameterMapper *qq)
    : QNodePrivate(qq)
    , m_bindingType(QParameterMapper::Uniform)
{
}

void QParameterMapper::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QParameterMapper *mapper = static_cast<const QParameterMapper*>(ref);
    d_func()->m_parameterName = mapper->d_func()->m_parameterName;
    d_func()->m_shaderVariableName = mapper->d_func()->m_shaderVariableName;
    d_func()->m_bindingType = mapper->d_func()->m_bindingType;
}

QParameterMapper::QParameterMapper(QNode *parent)
    : QNode(*new QParameterMapperPrivate(this), parent)
{
}

QParameterMapper::QParameterMapper(QParameterMapperPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QParameterMapper::QParameterMapper(const QString &parameterName, const QString &shaderParameterName, QParameterMapper::Binding bindingType, QNode *parent)
    : QNode(*new QParameterMapperPrivate(this), parent)
{
    Q_D(QParameterMapper);
    d->m_parameterName = parameterName;
    d->m_shaderVariableName = shaderParameterName;
    d->m_bindingType = bindingType;
}

void QParameterMapper::setParameterName(const QString &name)
{
    Q_D(QParameterMapper);
    if (d->m_parameterName != name) {
        d->m_parameterName = name;
        emit parameterNameChanged();
    }
}

void QParameterMapper::setShaderVariableName(const QString &name)
{
    Q_D(QParameterMapper);
    if (d->m_shaderVariableName != name) {
        d->m_shaderVariableName = name;
        emit shaderVariableNameChanged();
    }
}

void QParameterMapper::setBindingType(QParameterMapper::Binding type)
{
    Q_D(QParameterMapper);
    if (d->m_bindingType != type) {
        d->m_bindingType = type;
        emit bindingTypeChanged();
    }
}

QString QParameterMapper::parameterName() const
{
    Q_D(const QParameterMapper);
    return d->m_parameterName;
}

QString QParameterMapper::shaderVariableName() const
{
    Q_D(const QParameterMapper);
    return d->m_shaderVariableName;
}

QParameterMapper::Binding QParameterMapper::bindingType() const
{
    Q_D(const QParameterMapper);
    return d->m_bindingType;
}

} // Qt3D

QT_END_NAMESPACE
