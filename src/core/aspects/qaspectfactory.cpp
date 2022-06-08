// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaspectfactory_p.h"

#include <Qt3DCore/QAbstractAspect>
#include <QtCore/QDebug>
#include <QtCore/QtGlobal>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

typedef QHash<QLatin1String, QAspectFactory::CreateFunction> defaultFactories_t;
Q_GLOBAL_STATIC(defaultFactories_t, defaultFactories)
typedef QHash<const QMetaObject*, QLatin1String> defaultAspectNames_t;
Q_GLOBAL_STATIC(defaultAspectNames_t, defaultAspectNames)

Q_3DCORESHARED_EXPORT void qt3d_QAspectFactory_addDefaultFactory(const QLatin1String &name,
                                                                 const QMetaObject *metaObject,
                                                                 QAspectFactory::CreateFunction factory)
{
    defaultFactories->insert(name, factory);
    defaultAspectNames->insert(metaObject, name);
}

QAspectFactory::QAspectFactory()
    : m_factories(*defaultFactories),
      m_aspectNames(*defaultAspectNames)
{
}

QAspectFactory::QAspectFactory(const QAspectFactory &other)
    : m_factories(other.m_factories),
      m_aspectNames(other.m_aspectNames)
{
}

QAspectFactory::~QAspectFactory()
{
}

QAspectFactory &QAspectFactory::operator=(const QAspectFactory &other)
{
    m_factories = other.m_factories;
    m_aspectNames = other.m_aspectNames;
    return *this;
}

QList<QLatin1String> QAspectFactory::availableFactories() const
{
    return m_factories.keys();
}

QAbstractAspect *QAspectFactory::createAspect(const QLatin1String &aspect, QObject *parent) const
{
    if (m_factories.contains(aspect)) {
        return m_factories.value(aspect)(parent);
    } else {
        qWarning() << "Unsupported aspect name:" << aspect << "please check registrations";
        return nullptr;
    }
}

QLatin1String QAspectFactory::aspectName(QAbstractAspect *aspect) const
{
    return m_aspectNames.value(aspect->metaObject());
}

} // namespace Qt3DCore

QT_END_NAMESPACE
