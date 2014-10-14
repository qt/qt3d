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

#include "qabstractsceneloader.h"
#include "qabstractsceneloader_p.h"
#include "renderlogging.h"
#include <Qt3DRenderer/private/abstractsceneparser_p.h>
#include <Qt3DRenderer/private/gltfparser_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

QAbstractSceneLoaderPrivate::QAbstractSceneLoaderPrivate(QAbstractSceneLoader *qq)
    : QComponentPrivate(qq)
    , m_status(QAbstractSceneLoader::Loading)
{
}

QAbstractSceneLoader::QAbstractSceneLoader(QAbstractSceneLoaderPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

QAbstractSceneLoader::QAbstractSceneLoader(QNode *parent)
    : QComponent(*new QAbstractSceneLoaderPrivate(this), parent)
{
}

void QAbstractSceneLoaderPrivate::copy(const QNodePrivate *ref)
{
    const QAbstractSceneLoaderPrivate *s = static_cast<const QAbstractSceneLoaderPrivate *>(ref);
    m_source = s->m_source;
}

QString QAbstractSceneLoader::source() const
{
    Q_D(const QAbstractSceneLoader);
    return d->m_source;
}

void QAbstractSceneLoader::setSource(QString arg)
{
    Q_D(QAbstractSceneLoader);
    if (d->m_source != arg) {
        d->m_source = arg;
        emit sourceChanged(arg);
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
        change->setPropertyName(QByteArrayLiteral("source"));
        change->setValue(d->m_source);
        d->notifyObservers(change);
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
