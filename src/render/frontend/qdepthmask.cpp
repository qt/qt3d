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

#include "qdepthmask.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QDepthMaskPrivate : public QRenderStatePrivate
{
public:
    QDepthMaskPrivate(QDepthMask *qq)
        : QRenderStatePrivate(qq)
        , m_mask(false)
    {
    }

    void copy(const QNodePrivate *ref) Q_DECL_OVERRIDE;

    Q_DECLARE_PUBLIC(QDepthMask)
    bool m_mask;
};

QDepthMask::QDepthMask(QNode *parent)
    : QRenderState(*new QDepthMaskPrivate(this), parent)
{
}

void QDepthMaskPrivate::copy(const QNodePrivate *ref)
{
    QRenderStatePrivate::copy(ref);
    const QDepthMaskPrivate *refState = static_cast<const QDepthMaskPrivate *>(ref);
    m_mask = refState->m_mask;
}

bool QDepthMask::mask() const
{
    Q_D(const QDepthMask);
    return d->m_mask;
}

void QDepthMask::setMask(bool mask)
{
    Q_D(QDepthMask);
    if (d->m_mask != mask) {
        d->m_mask = mask;
        emit maskChanged();
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeUpdated, this));
            propertyChange->setPropertyName(QByteArrayLiteral("mask"));
            propertyChange->setValue(d->m_mask);
            d->notifyObservers(propertyChange);
        }
    }
}

QNode *QDepthMask::doClone() const
{
    QDepthMask *clone = new QDepthMask();
    clone->d_func()->copy(d_func());
    return clone;
}

} // Qt3D

QT_END_NAMESPACE
