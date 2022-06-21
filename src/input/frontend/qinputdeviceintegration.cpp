/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
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
**
**
**
****************************************************************************/

#include "qinputdeviceintegration_p.h"
#include "qinputdeviceintegration_p_p.h"

#include <Qt3DInput/QInputAspect>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QInputDeviceIntegrationPrivate::QInputDeviceIntegrationPrivate()
    : QObjectPrivate()
    , m_aspect(nullptr)
{
}

/*!
    \class Qt3DInput::QInputDeviceIntegration
    \inmodule Qt3DInput
    \since 5.5
    \brief Abstract base class used to define new input methods such as game controllers.

*/

/*!
   Creates a new QInputDeviceIntegration with \a parent.
 */
QInputDeviceIntegration::QInputDeviceIntegration(QObject *parent)
    : QObject(*new QInputDeviceIntegrationPrivate, parent)
{
}

/*! \internal */
QInputDeviceIntegration::QInputDeviceIntegration(QInputDeviceIntegrationPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
   Registers a corresponding backend class for this front end implementation
   with \a metaObject and \a functor.
 */
void QInputDeviceIntegration::registerBackendType(const QMetaObject &metaObject, const Qt3DCore::QBackendNodeMapperPtr &functor)
{
    Q_D(QInputDeviceIntegration);
    d->m_aspect->registerBackendType(metaObject, functor);
}

/*!
   Called by the InputAspect object after the integration has been created with \a aspect.
 */
void QInputDeviceIntegration::initialize(QInputAspect *aspect)
{
    Q_D(QInputDeviceIntegration);
    d->m_aspect = aspect;
    onInitialize();
}

/*!
 * \brief QInputDeviceIntegration::inputAspect
 * \return the Input Aspect associated with the InputDeviceIntegration
 */
QInputAspect *QInputDeviceIntegration::inputAspect() const
{
    Q_D(const QInputDeviceIntegration);
    return d->m_aspect;
}

} // namespace Qt3DInput

/*!
  \fn Qt3DInput::QInputDeviceIntegration::createPhysicalDevice(const QString &name)

  Create the Physical device identified by \a name.

  If not recognized return nullptr
*/

/*!
  \fn Qt3DInput::QInputDeviceIntegration::physicalDevices() const

 Returns the list of node ids for physical devices associated with this QInputDeviceIntegration.
*/

/*!
  \fn Qt3DInput::QInputDeviceIntegration::physicalDevice(Qt3DCore::QNodeId id) const

  Returns the QAbstractPhysicalDevice identified by the given \a id if it is related to this QInputDeviceIntegration.
*/

QT_END_NAMESPACE

#include "moc_qinputdeviceintegration_p.cpp"
