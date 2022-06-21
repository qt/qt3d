// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qshaderdata.h"
#include "qshaderdata_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

PropertyReaderInterface::~PropertyReaderInterface()
{
}

QShaderDataPrivate::QShaderDataPrivate()
    : QComponentPrivate()
    , m_propertyReader(PropertyReaderInterfacePtr(new QShaderDataPropertyReader()))
{
}

QShaderDataPrivate::QShaderDataPrivate(PropertyReaderInterfacePtr reader)
    : QComponentPrivate()
    , m_propertyReader(reader)
{
}

/*!
 * \class Qt3DRender::QShaderData
 * \inheaderfile Qt3DRender/QShaderData
 * \inmodule Qt3DRender
 *
 * \brief Provides a way of specifying values of a Uniform Block or a shader
 * structure.
 *
 * \note When subclassing and adding properties to QShaderData, note that if
 * you need to nest an inner Qt3DRender::QShaderData, the data type of the
 * property should be Qt3DRender::QShaderData* instead of the name of your
 * subclass.
 *
 * \since 5.5
 */

/*!
  \fn Qt3DRender::PropertyReaderInterface::readProperty(const QVariant &v)
  \return the property identified by \a v.
 */

/*!
 *   Constructs a new QShaderData with the specified \a parent.
 */
QShaderData::QShaderData(QNode *parent)
    : QComponent(*new QShaderDataPrivate, parent)
{
}

/*! \internal */
QShaderData::~QShaderData()
{
}
/*!
 * \brief QShaderData::propertyReader
 * Returns the PropertyReaderInterfacePtr for this shader data
 */
PropertyReaderInterfacePtr QShaderData::propertyReader() const
{
    Q_D(const QShaderData);
    return d->m_propertyReader;
}

/*! \internal */
QShaderData::QShaderData(QShaderDataPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*! \internal */
bool QShaderData::event(QEvent *event)
{
    Q_D(QShaderData);

    if (event->type() == QEvent::DynamicPropertyChange) {
        auto e = static_cast<QDynamicPropertyChangeEvent*>(event);
        const auto propertyName = e->propertyName();

        const QVariant data = property(propertyName);
        if (data.canConvert<Qt3DCore::QNode*>()) {
            const auto node = data.value<Qt3DCore::QNode*>();
            const auto id = node ? node->id() : Qt3DCore::QNodeId();
            d->notifyDynamicPropertyChange(propertyName, QVariant::fromValue(id));
        } else {
            d->notifyDynamicPropertyChange(propertyName, data);
        }
    }

    return QComponent::event(event);
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qshaderdata.cpp"
