// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qchannelmapping.h"
#include "qchannelmapping_p.h"

#include <QtCore/qmetaobject.h>
#include <QtCore/QMetaProperty>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

namespace {

template<typename T>
int componentCountForValue(const T &)
{
    return 0;
}

template<>
int componentCountForValue<QList<float>>(const QList<float> &v)
{
    return v.size();
}

template<>
int componentCountForValue<QVariantList>(const QVariantList &v)
{
    return v.size();
}


int componentCountForType(int type, const QVariant &value)
{
    const int vectorOfFloatTypeId = qMetaTypeId<QList<float>>();

    if (type == vectorOfFloatTypeId)
        return componentCountForValue<QList<float>>(value.value<QList<float>>());

    switch (type) {
    case QMetaType::Float:
    case QMetaType::Double:
        return 1;

    case QMetaType::QVector2D:
        return 2;

    case QMetaType::QVector3D:
    case QMetaType::QColor:
        return 3;

    case QMetaType::QVector4D:
    case QMetaType::QQuaternion:
        return 4;

    case QMetaType::QVariantList:
        return componentCountForValue<QVariantList>(value.toList());

    default:
        qWarning() << "Unhandled animation type";
        return 0;
    }
}

} // anonymous

QChannelMappingPrivate::QChannelMappingPrivate()
    : QAbstractChannelMappingPrivate()
    , m_channelName()
    , m_target(nullptr)
    , m_property()
    , m_propertyName(nullptr)
    , m_type(static_cast<int>(QMetaType::UnknownType))
    , m_componentCount(0)
{
    m_mappingType = ChannelMapping;
}

/*!
    \internal

    Find the type of the property specified on the target node
 */
void QChannelMappingPrivate::updatePropertyNameTypeAndComponentCount()
{
    int type;
    int componentCount = 0;
    const char *propertyName = nullptr;

    if (!m_target || m_property.isNull()) {
         type = QMetaType::UnknownType;
    } else {
        const QMetaObject *mo = m_target->metaObject();
        const int propertyIndex = mo->indexOfProperty(m_property.toLocal8Bit());
        QMetaProperty mp = mo->property(propertyIndex);
        propertyName = mp.name();
        type = mp.userType();
        const QVariant currentValue = m_target->property(mp.name());
        if (type == QMetaType::QVariant) {
            if (currentValue.isValid()) {
                type = currentValue.userType();
            } else {
                qWarning("QChannelMapping: Attempted to target QVariant property with no value set. "
                         "Set a value first in order to be able to determine the type.");
            }
        }
        componentCount = componentCountForType(type, currentValue);
    }

    if (m_type != type) {
        m_type = type;
        update();
    }

    if (m_componentCount != componentCount) {
        m_componentCount = componentCount;
        update();
    }

    if (qstrcmp(m_propertyName, propertyName) != 0) {
        m_propertyName = propertyName;
        update();
    }
}

/*!
    \class Qt3DAnimation::QChannelMapping
    \inherits Qt3DCore::QNode
    \inmodule Qt3DAnimation
    \brief Allows to map the channels within the clip onto properties of
           objects in the application.

*/

QChannelMapping::QChannelMapping(Qt3DCore::QNode *parent)
    : QAbstractChannelMapping(*new QChannelMappingPrivate, parent)
{
}

QChannelMapping::QChannelMapping(QChannelMappingPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractChannelMapping(dd, parent)
{
}

QChannelMapping::~QChannelMapping()
{
}

QString QChannelMapping::channelName() const
{
    Q_D(const QChannelMapping);
    return d->m_channelName;
}

Qt3DCore::QNode *QChannelMapping::target() const
{
    Q_D(const QChannelMapping);
    return d->m_target;
}

QString QChannelMapping::property() const
{
    Q_D(const QChannelMapping);
    return d->m_property;
}

void QChannelMapping::setChannelName(const QString &channelName)
{
    Q_D(QChannelMapping);
    if (d->m_channelName == channelName)
        return;

    d->m_channelName = channelName;
    emit channelNameChanged(channelName);
}

void QChannelMapping::setTarget(Qt3DCore::QNode *target)
{
    Q_D(QChannelMapping);
    if (d->m_target == target)
        return;

    if (d->m_target)
        d->unregisterDestructionHelper(d->m_target);

    if (target && !target->parent())
        target->setParent(this);
    d->m_target = target;

    // Ensures proper bookkeeping
    if (d->m_target)
        d->registerDestructionHelper(d->m_target, &QChannelMapping::setTarget, d->m_target);

    emit targetChanged(target);
    d->updatePropertyNameTypeAndComponentCount();
}

void QChannelMapping::setProperty(const QString &property)
{
    Q_D(QChannelMapping);
    if (d->m_property == property)
        return;

    d->m_property = property;

    // The backend uses propertyName instead of property
    const bool blocked = blockNotifications(true);
    emit propertyChanged(property);
    blockNotifications(blocked);

    d->updatePropertyNameTypeAndComponentCount();
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qchannelmapping.cpp"
