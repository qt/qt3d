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

#include "qdispatchcompute.h"
#include <Qt3DRender/private/qframegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QDispatchComputePrivate : public QFrameGraphNodePrivate
{
public:
    QDispatchComputePrivate()
        : QFrameGraphNodePrivate()
        , m_workGroupX(1)
        , m_workGroupY(1)
        , m_workGroupZ(1)
    {}

    int m_workGroupX;
    int m_workGroupY;
    int m_workGroupZ;
};

QDispatchCompute::QDispatchCompute(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QDispatchComputePrivate(), parent)
{
}

QDispatchCompute::~QDispatchCompute()
{
    QFrameGraphNode::cleanup();
}

int QDispatchCompute::workGroupX() const
{
    Q_D(const QDispatchCompute);
    return d->m_workGroupX;
}

int QDispatchCompute::workGroupY() const
{
    Q_D(const QDispatchCompute);
    return d->m_workGroupY;
}

int QDispatchCompute::workGroupZ() const
{
    Q_D(const QDispatchCompute);
    return d->m_workGroupZ;
}

void QDispatchCompute::setWorkGroupX(int workGroupX)
{
    Q_D(QDispatchCompute);
    if (d->m_workGroupX != workGroupX) {
        d->m_workGroupX = workGroupX;
        emit workGroupXChanged();
    }
}

void QDispatchCompute::setWorkGroupY(int workGroupY)
{
    Q_D(QDispatchCompute);
    if (d->m_workGroupY != workGroupY) {
        d->m_workGroupY = workGroupY;
        emit workGroupYChanged();
    }
}

void QDispatchCompute::setWorkGroupZ(int workGroupZ)
{
    Q_D(QDispatchCompute);
    if (d->m_workGroupZ != workGroupZ) {
        d->m_workGroupZ = workGroupZ;
        emit workGroupZChanged();
    }

}

void QDispatchCompute::copy(const Qt3DCore::QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QDispatchCompute *dispatch = static_cast<const QDispatchCompute *>(ref);
    d_func()->m_workGroupX = dispatch->workGroupX();
    d_func()->m_workGroupY = dispatch->workGroupY();
    d_func()->m_workGroupZ = dispatch->workGroupZ();
}

} // Qt3DRender

QT_END_NAMESPACE

