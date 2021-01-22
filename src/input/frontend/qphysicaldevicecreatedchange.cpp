/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qphysicaldevicecreatedchange.h"
#include "qphysicaldevicecreatedchange_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qaxissetting.h>
#include <Qt3DCore/qnodeid.h>


QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QPhysicalDeviceCreatedChangeBasePrivate::QPhysicalDeviceCreatedChangeBasePrivate(const QAbstractPhysicalDevice *device)
    : Qt3DCore::QNodeCreatedChangeBasePrivate(device)
    , m_axisSettingIds(Qt3DCore::qIdsForNodes(device->axisSettings()))
{

}

/*!
    \class Qt3DInput::QPhysicalDeviceCreatedChangeBase
    \inmodule Qt3DInput
    \brief Base class for handling changes in physical devices.
*/
QPhysicalDeviceCreatedChangeBase::QPhysicalDeviceCreatedChangeBase(const QAbstractPhysicalDevice *device)
    : Qt3DCore::QNodeCreatedChangeBase(*new QPhysicalDeviceCreatedChangeBasePrivate(device), device)
{
}

/*! \internal */
QPhysicalDeviceCreatedChangeBase::~QPhysicalDeviceCreatedChangeBase()
{
}

/*!
    Returns a list of node ids.
*/
Qt3DCore::QNodeIdVector Qt3DInput::QPhysicalDeviceCreatedChangeBase::axisSettingIds() const
{
    Q_D(const QPhysicalDeviceCreatedChangeBase);
    return d->m_axisSettingIds;
}

QT_END_NAMESPACE

} // namespace Qt3DInput
