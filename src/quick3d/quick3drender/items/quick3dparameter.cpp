// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtQml/QJSValue>
#include <QtQml/QJSValueIterator>

#include <Qt3DQuickRender/private/quick3dparameter_p_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DParameterPrivate::Quick3DParameterPrivate()
    : QParameterPrivate()
{
}

void Quick3DParameterPrivate::setValue(const QVariant &value)
{
    static const int qjsValueTypeId = qMetaTypeId<QJSValue>();

    if (value.userType() == qjsValueTypeId) {
        QJSValue v = value.value<QJSValue>();
        if (v.isArray())
            QParameterPrivate::setValue(value.value<QVariantList>());
    } else {
        QParameterPrivate::setValue(value);
    }
}

Quick3DParameter::Quick3DParameter(QNode *parent)
    : QParameter(*new Quick3DParameterPrivate, parent)
{
}

/*! \internal */
Quick3DParameter::Quick3DParameter(Quick3DParameterPrivate &dd, QNode *parent)
    : QParameter(dd, parent)
{
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3dparameter_p.cpp"


