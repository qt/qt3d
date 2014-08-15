/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#include "qcullface.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QCullFacePrivate : public QNodePrivate
{
public:
    QCullFacePrivate(QCullFace *qq)
        : QNodePrivate(qq)
        , m_mode(QCullFace::Back)
    {
    }

    Q_DECLARE_PUBLIC(QCullFace)
    QCullFace::CullingMode m_mode;
};

QCullFace::QCullFace(QNode *parent)
    : QRenderState(*new QCullFacePrivate(this), parent)
{
}

void QCullFace::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    Q_D(QCullFace);
    const QCullFace *refState = qobject_cast<const QCullFace *>(ref);
    if (refState != Q_NULLPTR) {
        d->m_mode = refState->mode();
    }
}

QCullFace::CullingMode QCullFace::mode() const
{
    Q_D(const QCullFace);
    return d->m_mode;
}

void QCullFace::setMode(QCullFace::CullingMode mode)
{
    Q_D(QCullFace);
    if (d->m_mode != mode) {
        d->m_mode = mode;
        emit modeChanged();
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeUpdated, this));
            propertyChange->setPropertyName(QByteArrayLiteral("mode"));
            propertyChange->setValue(d->m_mode);
            notifyObservers(propertyChange);
        }
    }
}

QNode *QCullFace::doClone(QNode *clonedParent) const
{
    return new QCullFace(clonedParent);
}

} // Qt3D

QT_END_NAMESPACE
