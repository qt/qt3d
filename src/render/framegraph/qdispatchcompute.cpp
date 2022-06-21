// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdispatchcompute.h"
#include "qdispatchcompute_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
/*!
    \class Qt3DRender::QDispatchCompute
    \inmodule Qt3DRender
    \since 5.7
    \ingroup framegraph
    \brief FrameGraph node to issue work for the compute shader on GPU.

    A Qt3DRender::QDispatchCompute allows work to be issued for the compute shader to
    run on the GPU. The workGroupX, workGroupY and workGroupZ properties specify the work group
    sizes for the compute shader invocation. QComputeCommand components need to be added
    to entities to instruct Qt3D to select the materials and geometry from the entities
    for the compute invocation. The work group sizes for the shader invocation will be
    the maximum of the work group sizes specified in QDispatchCompute and QComputeCommand.

 */

/*!
    \qmltype DispatchCompute
    \inqmlmodule Qt3D.Render
    \instantiates Qt3DRender::QDispatchCompute
    \inherits FrameGraphNode
    \since 5.7
    \brief FrameGraph node to issue work for the compute shader on GPU.

    A DispatchCompute allows work to be issued for the compute shader to run on the GPU.
    The workGroupX, workGroupY and workGroupZ properties specify the work group sizes for
    the compute shader invocation. ComputeCommand components need to be added
    to entities to instruct Qt3D to select the materials and geometry from the entities
    for the compute invocation. The work group sizes for the shader invocation will be
    the maximum of the work group sizes specified in DispatchCompute and ComputeCommand.
*/

/*!
    \qmlproperty int DispatchCompute::workGroupX
    Specifies X workgroup size.
 */

/*!
    \qmlproperty int DispatchCompute::workGroupY
    Specifies Y workgroup size.
 */

/*!
    \qmlproperty int DispatchCompute::workGroupZ
    Specifies Z workgroup size.
 */

/*!
    The constructor creates an instance with the specified \a parent.
 */
QDispatchCompute::QDispatchCompute(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QDispatchComputePrivate(), parent)
{
}

/*! \internal */
QDispatchCompute::~QDispatchCompute()
{
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

/*!
    \property Qt3DRender::QDispatchCompute::workGroupX
    Specifies X workgroup.
 */
void QDispatchCompute::setWorkGroupX(int workGroupX)
{
    Q_D(QDispatchCompute);
    if (d->m_workGroupX != workGroupX) {
        d->m_workGroupX = workGroupX;
        emit workGroupXChanged();
    }
}

/*!
    \property Qt3DRender::QDispatchCompute::workGroupY
    Specifies Y workgroup.
 */
void QDispatchCompute::setWorkGroupY(int workGroupY)
{
    Q_D(QDispatchCompute);
    if (d->m_workGroupY != workGroupY) {
        d->m_workGroupY = workGroupY;
        emit workGroupYChanged();
    }
}

/*!
    \property Qt3DRender::QDispatchCompute::workGroupZ
    Specifies Z workgroup.
 */
void QDispatchCompute::setWorkGroupZ(int workGroupZ)
{
    Q_D(QDispatchCompute);
    if (d->m_workGroupZ != workGroupZ) {
        d->m_workGroupZ = workGroupZ;
        emit workGroupZChanged();
    }

}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qdispatchcompute.cpp"

