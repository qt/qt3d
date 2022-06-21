/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
    \instantiates Qt3DRender::QFilterKey
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
    \property QFilterKey::value

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
    \property QFilterKey::name

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

Qt3DCore::QNodeCreatedChangeBasePtr QFilterKey::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QFilterKeyData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QFilterKey);
    data.name = d->m_name;
    data.value = d->m_value;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qfilterkey.cpp"
