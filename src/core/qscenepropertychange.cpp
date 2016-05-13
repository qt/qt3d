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

#include "qscenepropertychange.h"
#include "qscenepropertychange_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*!
    \class Qt3DCore::QScenePropertyChange
    \inmodule Qt3DCore
*/

QScenePropertyChangePrivate::QScenePropertyChangePrivate()
    : QSceneChangePrivate()
{
}

QScenePropertyChangePrivate::~QScenePropertyChangePrivate()
{

}

QScenePropertyChange::QScenePropertyChange(ChangeFlag type, ObservableType observableType, const QNodeId &subjectId, QSceneChange::Priority priority)
    : QSceneChange(*new QScenePropertyChangePrivate, type, observableType, subjectId, priority)
{
}

/*! \internal */
QScenePropertyChange::QScenePropertyChange(QScenePropertyChangePrivate &dd)
    : QSceneChange(dd)
{
}

/*! \internal */
QScenePropertyChange::QScenePropertyChange(QScenePropertyChangePrivate &dd, ChangeFlag type, ObservableType observableType, const QNodeId &subjectId, QSceneChange::Priority priority)
    : QSceneChange(dd, type, observableType, subjectId, priority)
{
}

QScenePropertyChange::~QScenePropertyChange()
{
}

const char *QScenePropertyChange::propertyName() const
{
    Q_D(const QScenePropertyChange);
    return d->m_propertyName;
}

QVariant QScenePropertyChange::value() const
{
    Q_D(const QScenePropertyChange);
    return d->m_value;
}

void QScenePropertyChange::setPropertyName(const char *name)
{
    Q_D(QScenePropertyChange);
    d->m_propertyName = name;
}

void QScenePropertyChange::setValue(const QVariant &value)
{
    Q_D(QScenePropertyChange);
    d->m_value = value;
}

} // Qt3D

QT_END_NAMESPACE
