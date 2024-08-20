// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qfilterkey.h"
#include "qfilterkey_p.h"
#include <private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {


QFilterKeyPrivate::QFilterKeyPrivate()
    : QNodePrivate()
{
}

/*!
    \class Qt3DRender::QFilterKey
    \inmodule Qt3DRender
    \inherits Qt3DCore::QNode
    \since 5.5
    \brief The QFilterKey class provides storage for filter keys and their values.

    Filter keys are used by QTechnique and QRenderPass to specify at which stage of rendering the
    technique or the render pass is used.

    \note QFilterKey node can not be disabled.
*/

/*!
    \qmltype FilterKey
    \nativetype Qt3DRender::QFilterKey
    \inherits Node
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief Stores filter keys and their values.

    A FilterKey is a storage type for filter key and value pair.
    Filter keys are used by Technique and RenderPass to specify at which stage of rendering the
    technique or the render pass is used.

    \note FilterKey node can not be disabled.
*/

QFilterKey::QFilterKey(QNode *parent)
    : QNode(*new QFilterKeyPrivate, parent)
{
}

QFilterKey::~QFilterKey()
{
}

void QFilterKey::setValue(const QVariant &value)
{
    Q_D(QFilterKey);
    if (value != d->m_value) {
        d->m_value = value;
        emit valueChanged(value);
    }
}

void QFilterKey::setName(const QString &name)
{
    Q_D(QFilterKey);
    if (name != d->m_name) {
        d->m_name = name;
        emit nameChanged(name);
    }
}

/*!
    \property Qt3DRender::QFilterKey::value

    Holds the value of the filter key.
*/

/*!
    \qmlproperty variant FilterKey::value

    Holds the value of the filter key.
*/

QVariant QFilterKey::value() const
{
    Q_D(const QFilterKey);
    return d->m_value;
}

/*!
    \property Qt3DRender::QFilterKey::name

    Holds the name of the filter key.
*/

/*!
    \qmlproperty string FilterKey::name

    Holds the name of the filter key.
*/

QString QFilterKey::name() const
{
    Q_D(const QFilterKey);
    return d->m_name;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qfilterkey.cpp"
