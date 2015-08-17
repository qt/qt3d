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

#include "qabstractsceneloader.h"
#include "qabstractsceneloader_p.h"
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/private/abstractsceneparser_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

/*!
    \class Qt3D::Render::QAbstractSceneLoaderPrivate
    \internal
*/
QAbstractSceneLoaderPrivate::QAbstractSceneLoaderPrivate()
    : QComponentPrivate()
    , m_status(QAbstractSceneLoader::Loading)
{
    m_shareable = false;
}

/*! \internal */
QAbstractSceneLoader::QAbstractSceneLoader(QAbstractSceneLoaderPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

QAbstractSceneLoader::QAbstractSceneLoader(QNode *parent)
    : QComponent(*new QAbstractSceneLoaderPrivate, parent)
{
}

QAbstractSceneLoader::~QAbstractSceneLoader()
{
    Q_ASSERT_X(QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3D::QAbstractSceneLoader subclass didn't call QNode::cleanup in its destructor");
}

void QAbstractSceneLoader::copy(const QNode *ref)
{
    const QAbstractSceneLoader *s = static_cast<const QAbstractSceneLoader*>(ref);
    d_func()->m_source = s->d_func()->m_source;
}

QUrl QAbstractSceneLoader::source() const
{
    Q_D(const QAbstractSceneLoader);
    return d->m_source;
}

void QAbstractSceneLoader::setSource(const QUrl &arg)
{
    Q_D(QAbstractSceneLoader);
    if (d->m_source != arg) {
        d->m_source = arg;
        emit sourceChanged();
    }
}

QAbstractSceneLoader::Status QAbstractSceneLoader::status() const
{
    Q_D(const QAbstractSceneLoader);
    return d->m_status;
}

void QAbstractSceneLoader::setStatus(QAbstractSceneLoader::Status status)
{
    Q_D(QAbstractSceneLoader);
    if (d->m_status != status) {
        d->m_status = status;
        emit statusChanged();
    }
}

} // Render

} // namespace Qt3D

QT_END_NAMESPACE
