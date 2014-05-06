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

#include "quickwindow.h"
#include <Qt3DCore/qaspectengine.h>

#include <QQmlComponent>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

QuickWindow::QuickWindow(QScreen *screen)
    : Window(screen)
    , m_engine(new QQmlEngine)
{
}

QuickWindow::~QuickWindow()
{
}

QuickWindow::Status QuickWindow::status() const
{
    if (!m_engine)
        return Error;

    if (!m_component)
        return Null;

    return Status(m_component->status());
}

void QuickWindow::setSource(const QUrl& source)
{
    if (!m_engine) {
        qWarning() << "Window: invalid qml engine.";
        return;
    }

    if (m_root) {
        m_aspectEngine->shutdown();
        m_aspectEngine->setRoot(0);
        m_root = QSharedPointer<QObject>();
    }

    if (m_component)
        m_component = QSharedPointer<QQmlComponent>();

    if (!source.isEmpty()) {
        m_component = QSharedPointer<QQmlComponent>(new QQmlComponent(m_engine.data(), source, this));
        if (!m_component->isLoading()) {
            continueExecute();
        } else {
            QObject::connect(m_component.data(), SIGNAL(statusChanged(QQmlComponent::Status)),
                             this, SLOT(continueExecute()));
        }
    }
}

void QuickWindow::continueExecute()
{
    qDebug() << Q_FUNC_INFO;

    QObject::disconnect(m_component.data(), SIGNAL(statusChanged(QQmlComponent::Status)),
               this, SLOT(continueExecute()));

    if (m_component->isError()) {
        QList<QQmlError> errorList = m_component->errors();
        Q_FOREACH ( const QQmlError& error, errorList ) {
            QMessageLogger(error.url().toString().toLatin1().constData(), error.line(), 0).warning()
                << error;
        }
        emit statusChanged(status());
        return;
    }

    QObject* obj = m_component->create();

    if (m_component->isError()) {
        QList<QQmlError> errorList = m_component->errors();
        Q_FOREACH ( const QQmlError& error, errorList ) {
            QMessageLogger(error.url().toString().toLatin1().constData(), error.line(), 0).warning()
                << error;
        }
        emit statusChanged(status());
        return;
    }

    setRootObject(obj);
    emit statusChanged(status());
}

} // Quick

} // Qt3D

QT_END_NAMESPACE
