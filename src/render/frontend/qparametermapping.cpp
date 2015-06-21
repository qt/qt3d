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

#include "qparametermapping.h"
#include <private/qparametermapping_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QParameterMappingPrivate
    \internal
*/
QParameterMappingPrivate::QParameterMappingPrivate()
    : QNodePrivate()
    , m_bindingType(QParameterMapping::Uniform)
{
}
/*!
    \class QParameterMapping
    \inmodule Qt3DRenderer
    \since 5.5
    \brief The QParameterMapping class provides ...
*/

/*!
    \qmltype ParameterMapping
    \instantiates Qt3D::QParameterMapping
    \inherits Node
    \inqmlmodule Qt3D.Renderer
    \since 5.5
    \brief For ...
*/

/*! \fn void Qt3D::QParameterMapping::copy(const QNode *ref)
  Copies the \a ref instance into this one.
 */
void QParameterMapping::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QParameterMapping *mapper = static_cast<const QParameterMapping*>(ref);
    d_func()->m_parameterName = mapper->d_func()->m_parameterName;
    d_func()->m_shaderVariableName = mapper->d_func()->m_shaderVariableName;
    d_func()->m_bindingType = mapper->d_func()->m_bindingType;
}

/*! \fn Qt3D::QParameterMapping::QParameterMapping(QNode *parent)
  Constructs a new QParameterMapping with the specified \a parent.
 */
QParameterMapping::QParameterMapping(QNode *parent)
    : QNode(*new QParameterMappingPrivate, parent)
{
}

/*!
  \enum Qt3D::QParameterMapping::Binding

  \value Uniform
  \value Attribute
  \value StandardUniform
  \value FragmentOutput

*/

/*! \internal */
QParameterMapping::QParameterMapping(QParameterMappingPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
  This constructor takes a \a parameterName, a \a shaderParameterName,
  a \a bindingType, and a \a parent.
 */
QParameterMapping::QParameterMapping(const QString &parameterName, const QString &shaderParameterName, QParameterMapping::Binding bindingType, QNode *parent)
    : QNode(*new QParameterMappingPrivate, parent)
{
    Q_D(QParameterMapping);
    d->m_parameterName = parameterName;
    d->m_shaderVariableName = shaderParameterName;
    d->m_bindingType = bindingType;
}

QParameterMapping::~QParameterMapping()
{
    QNode::cleanup();
}

/*!
  \property Qt3D::QParameterMapping::parameterName

*/

/*!
  \qmlproperty string Qt3D.Renderer::ParameterMapping::parameterName

*/

void QParameterMapping::setParameterName(const QString &name)
{
    Q_D(QParameterMapping);
    if (d->m_parameterName != name) {
        d->m_parameterName = name;
        emit parameterNameChanged();
    }
}

/*!
  \property Qt3D::QParameterMapping::shaderVariableName

*/

/*!
  \qmlproperty string Qt3D.Renderer::ParameterMapping::shaderVariableName

*/

void QParameterMapping::setShaderVariableName(const QString &name)
{
    Q_D(QParameterMapping);
    if (d->m_shaderVariableName != name) {
        d->m_shaderVariableName = name;
        emit shaderVariableNameChanged();
    }
}

/*!
  \property Qt3D::QParameterMapping::bindingType

*/

/*!
  \qmlproperty  BindingType Qt3D.Renderer::ParameterMapping::bindingType

  \value Uniform
  \value Attribute
  \value StandardUniform
  \value FragmentOutput
*/
void QParameterMapping::setBindingType(QParameterMapping::Binding type)
{
    Q_D(QParameterMapping);
    if (d->m_bindingType != type) {
        d->m_bindingType = type;
        emit bindingTypeChanged();
    }
}

QString QParameterMapping::parameterName() const
{
    Q_D(const QParameterMapping);
    return d->m_parameterName;
}

QString QParameterMapping::shaderVariableName() const
{
    Q_D(const QParameterMapping);
    return d->m_shaderVariableName;
}

QParameterMapping::Binding QParameterMapping::bindingType() const
{
    Q_D(const QParameterMapping);
    return d->m_bindingType;
}

} // Qt3D

QT_END_NAMESPACE
