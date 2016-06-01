/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcomputecommand.h"
#include "qcomputecommand_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QComputeCommand
 * \brief The QComputerCommand class
 * \since 5.7
 * \inmodule Qt3DRender
 */

QComputeCommandPrivate::QComputeCommandPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_workGroupX(1)
    , m_workGroupY(1)
    , m_workGroupZ(1)
{
}

/*!
 * The constructor creates a new Qt3DRender::QComputeCommand instance with the
 * specified \a parent.
 * \param parent
 */
QComputeCommand::QComputeCommand(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QComputeCommandPrivate, parent)
{
}

/*! \internal */
QComputeCommand::~QComputeCommand()
{
}

/*!
 * \return the workgroup size for the first dimension.
 */
int QComputeCommand::workGroupX() const
{
    Q_D(const QComputeCommand);
    return d->m_workGroupX;
}

/*!
 * \return the workgroup size for the second dimension.
 */
int QComputeCommand::workGroupY() const
{
    Q_D(const QComputeCommand);
    return d->m_workGroupX;
}

/*!
 * \return the workgroup size for the third dimension.
 */
int QComputeCommand::workGroupZ() const
{
    Q_D(const QComputeCommand);
    return d->m_workGroupX;
}

/*!
 * Sets the workgroup for the first dimension to \a workGroupX.
 * \param workGroupX
 */
void QComputeCommand::setWorkGroupX(int workGroupX)
{
    Q_D(QComputeCommand);
    if (d->m_workGroupX != workGroupX) {
        d->m_workGroupX = workGroupX;
        emit workGroupXChanged();
    }
}

/*!
 * Sets the workgroup for the second dimension to \a workGroupY.
 * \param workGroupY
 */
void QComputeCommand::setWorkGroupY(int workGroupY)
{
    Q_D(QComputeCommand);
    if (d->m_workGroupY != workGroupY) {
        d->m_workGroupY = workGroupY;
        emit workGroupYChanged();
    }
}

/*!
 * Sets the workgroup for the third dimension to \a workGroupZ.
 * \param workGroupZ
 */
void QComputeCommand::setWorkGroupZ(int workGroupZ)
{
    Q_D(QComputeCommand);
    if (d->m_workGroupZ != workGroupZ) {
        d->m_workGroupZ = workGroupZ;
        emit workGroupZChanged();
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QComputeCommand::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QComputeCommandData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QComputeCommand);
    data.workGroupX = d->m_workGroupX;
    data.workGroupY = d->m_workGroupY;
    data.workGroupZ = d->m_workGroupZ;
    return creationChange;
}

} // Render

QT_END_NAMESPACE
