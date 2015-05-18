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

#include "qqmlaspectengine.h"
#include "qqmlaspectengine_p.h"
#include <QQmlComponent>
#include <QQmlContext>
#include <QDebug>
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

/*!
    \class Qt3D::Quick::QQmlAspectEnginePrivate
    \internal
*/
QQmlAspectEnginePrivate::QQmlAspectEnginePrivate()
    : QObjectPrivate()
    , m_qmlEngine(new QQmlEngine())
    , m_aspectEngine(new QAspectEngine())
    , m_component(Q_NULLPTR)
{
}

QQmlAspectEngine::QQmlAspectEngine(QObject *parent)
    : QObject(*new QQmlAspectEnginePrivate, parent)
{
}

QQmlAspectEngine::Status QQmlAspectEngine::status() const
{
    Q_D(const QQmlAspectEngine);

    if (!d->m_component)
        return Null;

    return Status(d->m_component->status());
}

void QQmlAspectEngine::setSource(const QUrl &source)
{
    Q_D(QQmlAspectEngine);

    if (d->m_component) {
        d->m_aspectEngine->shutdown();
        d->m_aspectEngine->setRootEntity(Q_NULLPTR);
        d->m_component = Q_NULLPTR;
    }

    if (!source.isEmpty()) {
        d->m_component = new QQmlComponent(d->m_qmlEngine.data(), source);
        if (!d->m_component->isLoading()) {
            d->_q_continueExecute();
        } else {
            QObject::connect(d->m_component, SIGNAL(statusChanged(QQmlComponent::Status)),
                             this, SLOT(_q_continueExecute()));
        }
    }
}

QQmlEngine *QQmlAspectEngine::qmlEngine() const
{
    Q_D(const QQmlAspectEngine);
    return d->m_qmlEngine.data();
}

QAspectEngine *QQmlAspectEngine::aspectEngine() const
{
    Q_D(const QQmlAspectEngine);
    return d->m_aspectEngine.data();
}

void QQmlAspectEnginePrivate::_q_continueExecute()
{
    Q_Q(QQmlAspectEngine);

    QObject::disconnect(m_component, SIGNAL(statusChanged(QQmlComponent::Status)),
                        q, SLOT(_q_continueExecute()));

    if (m_component->isError()) {
        QList<QQmlError> errorList = m_component->errors();
        Q_FOREACH ( const QQmlError& error, errorList ) {
            QMessageLogger(error.url().toString().toLatin1().constData(), error.line(), 0).warning()
                << error;
        }
        emit q->statusChanged(q->status());
        return;
    }

    QObject* obj = m_component->create();

    if (m_component->isError()) {
        QList<QQmlError> errorList = m_component->errors();
        Q_FOREACH ( const QQmlError& error, errorList ) {
            QMessageLogger(error.url().toString().toLatin1().constData(), error.line(), 0).warning()
                << error;
        }
        emit q->statusChanged(q->status());
        return;
    }

    m_aspectEngine->setRootEntity(qobject_cast<QEntity *>(obj));
    emit q->statusChanged(q->status());
}

} // Quick

} // Qt3D

QT_END_NAMESPACE

#include "moc_qqmlaspectengine.cpp"
