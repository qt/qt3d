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

#include "qfrontface.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QFrontFacePrivate : public QNodePrivate
{
public:
    QFrontFacePrivate(QFrontFace *qq)
        : QNodePrivate(qq)
        , m_direction(QFrontFace::ClockWise)
    {
    }

    Q_DECLARE_PUBLIC(QFrontFace)
    QFrontFace::FaceDir m_direction;
};

QFrontFace::QFrontFace(QNode *parent)
    : QRenderState(*new QFrontFacePrivate(this), parent)
{
}

void QFrontFace::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    Q_D(QFrontFace);
    const QFrontFace *refState = qobject_cast<const QFrontFace *>(ref);
    if (refState != Q_NULLPTR) {
        d->m_direction = refState->direction();
    }
}

QFrontFace::FaceDir QFrontFace::direction() const
{
    Q_D(const QFrontFace);
    return d->m_direction;
}

void QFrontFace::setDirection(QFrontFace::FaceDir direction)
{
    Q_D(QFrontFace);
    if (d->m_direction != direction) {
        d->m_direction = direction;
        emit directionChanged();
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeUpdated, this));
            propertyChange->setPropertyName(QByteArrayLiteral("direction"));
            propertyChange->setValue(d->m_direction);
            notifyObservers(propertyChange);
        }
    }
}

QNode *QFrontFace::doClone(QNode *clonedParent) const
{
    return new QFrontFace(clonedParent);
}

} // Qt3D

QT_END_NAMESPACE
