// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
