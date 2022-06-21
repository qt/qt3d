// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcomputecommand.h"
#include "qcomputecommand_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QComputeCommand
    \since 5.7
    \inmodule Qt3DRender
    \brief QComponent to issue work for the compute shader on GPU.

    A Qt3DRender::QComputeCommand is used to issue work for the compute shader.
    The compute shader is specified in the QMaterial component of the same entity the
    QComputeCommand is added to. The workGroupX, workGroupY and workGroupZ properties
    specify the work group sizes for the compute shader invocation. Qt3DRender::QDispatchCompute
    node needs to be present in the FrameGraph to actually issue the commands.

    \note If the rendering policy is set to Qt3DRender::QRenderSettings::OnDemand and there are no
    changes to the scene, the ComputeCommand will not be invoked repeatedly.
    The Qt3DRender::QRenderSettings::Always render policy must be set for the ComputeCommand
    to be repeatedly invoked if there are no other changes to the scene that triggers rendering a
    new frame.
 */

/*!
    \qmltype ComputeCommand
    \since 5.7
    \inqmlmodule Qt3D.Render
    \inherits Component3D
    \instantiates Qt3DRender::QComputeCommand
    \brief Component to issue work for the compute shader on GPU.

    A ComputeCommand is used to issue work for the compute shader.
    The compute shader is specified in the Material component of the same entity the
    ComputeCommand is added to. The workGroupX, workGroupY and workGroupZ properties
    specify the work group sizes for the compute shader invocation. DispatchCompute
    node needs to be present in the FrameGraph to actually issue the commands. The execution behavior
    of the compute command can be controlled with the run type property.

    \note If the rendering policy is set to RenderSettings.OnDemand, the run
    type is set to Continuous and there are no changes to the scene, the
    ComputeCommand will not be invoked repeatedly. The RenderSettings.Always
    render policy must be set for the ComputeCommand to be repeatedly invoked
    if there are no other changes to the scene that triggers rendering a new
    frame.
 */

/*!
    \qmlproperty int ComputeCommand::workGroupX
    Specifies X workgroup size.
 */

/*!
    \qmlproperty enumeration ComputeCommand::runType

    Specifies whether the compute command should be performed every frame or
    manually triggered.

    \value ComputeCommand.Continuous Compute command is executed everyframe. This is the
    default.

    \value ComputeCommand.Manual CompouteCommand is executed for a given number of frames and
    then the component disables itself.
 */

/*!
    \qmlproperty int ComputeCommand::workGroupY
    Specifies Y workgroup size.
 */

/*!
    \qmlproperty int ComputeCommand::workGroupZ
    Specifies Z workgroup size.
 */

/*!
    \property QComputeCommand::workGroupX
    Specifies X workgroup size.
 */

/*!
    \property QComputeCommand::workGroupY
    Specifies Y workgroup size.
 */

/*!
    \property QComputeCommand::workGroupZ
    Specifies Z workgroup size.
 */

/*!
    \property QComputeCommand::runType

    Specifies whether the compute command should be performed every frame or
    manually triggered.

    If set to Continuous, Compute command is executed everyframe. This is the
    default.

    If set to Manual CompouteCommand is executed for a given number of frames
    and then the component disables itself.
 */

QComputeCommandPrivate::QComputeCommandPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_workGroupX(1)
    , m_workGroupY(1)
    , m_workGroupZ(1)
    , m_runType(QComputeCommand::Continuous)
    , m_frameCount(0)
{
}

void QComputeCommandPrivate::setFrameCount(int frameCount)
{
    m_frameCount = frameCount;
    update();
}

/*!
    The constructor creates a new Qt3DRender::QComputeCommand instance with the
    specified \a parent.
 */
QComputeCommand::QComputeCommand(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QComputeCommandPrivate, parent)
{
}

/*! \internal */
QComputeCommand::~QComputeCommand()
{
}

int QComputeCommand::workGroupX() const
{
    Q_D(const QComputeCommand);
    return d->m_workGroupX;
}

int QComputeCommand::workGroupY() const
{
    Q_D(const QComputeCommand);
    return d->m_workGroupY;
}

int QComputeCommand::workGroupZ() const
{
    Q_D(const QComputeCommand);
    return d->m_workGroupZ;
}

QComputeCommand::RunType QComputeCommand::runType() const
{
    Q_D(const QComputeCommand);
    return d->m_runType;
}

/*!
    Sets the workgroup for the first dimension to \a workGroupX.
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
    Sets the workgroup for the second dimension to \a workGroupY.
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
    Sets the workgroup for the third dimension to \a workGroupZ.
 */
void QComputeCommand::setWorkGroupZ(int workGroupZ)
{
    Q_D(QComputeCommand);
    if (d->m_workGroupZ != workGroupZ) {
        d->m_workGroupZ = workGroupZ;
        emit workGroupZChanged();
    }
}

void QComputeCommand::setRunType(QComputeCommand::RunType runType)
{
    Q_D(QComputeCommand);
    if (d->m_runType != runType) {
        d->m_runType = runType;
        emit runTypeChanged();
    }
}

/*!
    When the run type is set to Manual, calling trigger will make the compute
    command be executed for the next \a frameCount frames. Upon completion of
    the execution, the enabled property will be set to false.
 */
void QComputeCommand::trigger(int frameCount)
{
    if (isEnabled())
        qWarning() << Q_FUNC_INFO << "is triggered while it hasn't finished executing";

    Q_D(QComputeCommand);
    d->setFrameCount(frameCount);
    setEnabled(true);
}

/*!
    When the run type is set to Manual, calling trigger will make the compute
    command be executed for the next \a frameCount frames. Upon completion of
    the execution, the enabled property will be set to false. The size of the
    workgroup previously set will be overridden with \a workGroupX, \a
    workGroupY, \a workGroupZ.
 */
void QComputeCommand::trigger(int workGroupX, int workGroupY, int workGroupZ, int frameCount)
{
    if (isEnabled())
        qWarning() << Q_FUNC_INFO << "is triggered while it hasn't finished executing";

    setWorkGroupX(workGroupX);
    setWorkGroupY(workGroupY);
    setWorkGroupZ(workGroupZ);
    Q_D(QComputeCommand);
    d->setFrameCount(frameCount);
    setEnabled(true);
}

} // Render

QT_END_NAMESPACE

#include "moc_qcomputecommand.cpp"
