/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qboundingvolumespecifier.h"
#include <private/qobject_p.h>
#include <Qt3DRender/qabstractattribute.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QBoundingVolumeSpecifierPrivate : public QObjectPrivate
{
public:
    QBoundingVolumeSpecifierPrivate()
        : QObjectPrivate()
        , m_positionAttribute(Q_NULLPTR)
    {}

    QAbstractAttribute *m_positionAttribute;
};

/*!
 * \qmltype BoundingVolumeSpecifier
 * \instantiates Qt3DRender::QBoundingVolumeSpecifier
 * \inqmlmodule Qt3D.Render
 * \brief Uncreatable
 */

/*!
 * \qmlproperty AbstractAttribute BoundingVolumeSpecifier::positionAttribute
 *
 * Holds the position attribute.
 */

/*!
 * \class Qt3DRender::QBoundingVolumeSpecifier
 * \inmodule Qt3DRender
 *
 * \inherits QObject
 */

/*!
 * Constructs a new QBoundingVolumeSpecifier with \a parent.
 */
QBoundingVolumeSpecifier::QBoundingVolumeSpecifier(QObject *parent)
    : QObject(*new QBoundingVolumeSpecifierPrivate(), parent)
{
}

/*!
 * \property QBoundingVolumeSpecifier::positionAttribute
 *
 * Holds the position attribute.
 */
QAbstractAttribute *QBoundingVolumeSpecifier::positionAttribute() const
{
    Q_D(const QBoundingVolumeSpecifier);
    return d->m_positionAttribute;
}

void QBoundingVolumeSpecifier::setPositionAttribute(QAbstractAttribute *positionAttribute)
{
    Q_D(QBoundingVolumeSpecifier);
    if (positionAttribute != d->m_positionAttribute) {
        d->m_positionAttribute = positionAttribute;
        emit positionAttributeChanged(positionAttribute);
    }
}

} // Qt3DRender

QT_END_NAMESPACE
