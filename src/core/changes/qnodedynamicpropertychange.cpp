/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
**
****************************************************************************/

#include "qnodedynamicpropertychange.h"
#include "qnodedynamicpropertychange_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QNodeDynamicPropertyChangePrivate::QNodeDynamicPropertyChangePrivate()
    : QPropertyUpdatedChangeBasePrivate()
{
}

QNodeDynamicPropertyChangePrivate::~QNodeDynamicPropertyChangePrivate()
{
}


QNodeDynamicPropertyChange::QNodeDynamicPropertyChange(QNodeId subjectId)
    : QPropertyUpdatedChangeBase(*new QNodeDynamicPropertyChangePrivate, subjectId)
{
}

QNodeDynamicPropertyChange::QNodeDynamicPropertyChange(QNodeDynamicPropertyChangePrivate &dd,
                                                       QNodeId subjectId)
    : QPropertyUpdatedChangeBase(dd, subjectId)
{
}

QNodeDynamicPropertyChange::~QNodeDynamicPropertyChange()
{
}

/*!
 * \return name of the property.
 */
QByteArray QNodeDynamicPropertyChange::propertyName() const
{
    Q_D(const QNodeDynamicPropertyChange);
    return d->m_propertyName;
}

/*!
 * Sets the property change \a name.
 */
void QNodeDynamicPropertyChange::setPropertyName(const QByteArray &name)
{
    Q_D(QNodeDynamicPropertyChange);
    d->m_propertyName = name;
}

/*!
 * \return property value.
 */
QVariant QNodeDynamicPropertyChange::value() const
{
    Q_D(const QNodeDynamicPropertyChange);
    return d->m_value;
}

/*!
 * Set the property change \a value.
 */
void QNodeDynamicPropertyChange::setValue(const QVariant &value)
{
    Q_D(QNodeDynamicPropertyChange);
    d->m_value = value;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
