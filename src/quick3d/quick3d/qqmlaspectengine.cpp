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

#include "qqmlaspectengine.h"
#include "qqmlaspectengine_p.h"
#include <QQmlComponent>
#include <QQmlContext>
#include <QDebug>
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

QQmlAspectEnginePrivate::QQmlAspectEnginePrivate(QQmlAspectEngine *qq)
    : QObjectPrivate()
    , q_ptr(qq)
    , m_qmlEngine(new QQmlEngine())
    , m_aspectEngine(new QAspectEngine())
    , m_component(Q_NULLPTR)
{
}

QQmlAspectEngine::QQmlAspectEngine(QObject *parent)
    : QObject(*new QQmlAspectEnginePrivate(this), parent)
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
    QObject::disconnect(m_component, SIGNAL(statusChanged(QQmlComponent::Status)),
               q_ptr, SLOT(_q_continueExecute()));

    QQmlAspectEngine *q = static_cast<QQmlAspectEngine *>(q_ptr);

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
