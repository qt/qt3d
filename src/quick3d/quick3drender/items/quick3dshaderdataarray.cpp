// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickRender/private/quick3dshaderdataarray_p.h>
#include <Qt3DRender/private/qshaderdata_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class Quick3DShaderDataArrayPrivate : public Qt3DCore::QNodePrivate
{
public:
    Quick3DShaderDataArrayPrivate()
        : QNodePrivate()
    {}

    QList<QShaderData *> m_values;
};

Quick3DShaderDataArray::Quick3DShaderDataArray(QNode *parent)
    : QNode(*new Quick3DShaderDataArrayPrivate, parent)
{
}

QQmlListProperty<QShaderData> Quick3DShaderDataArray::valuesList()
{
    using qt_size_type = qsizetype;
    using ListContentType = QShaderData;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *bar) {
        Quick3DShaderDataArray *self = static_cast<Quick3DShaderDataArray *>(list->object);
        static_cast<Quick3DShaderDataArrayPrivate *>(Quick3DShaderDataArrayPrivate::get(self))->m_values.append(bar);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DShaderDataArray *self = static_cast<Quick3DShaderDataArray *>(list->object);
        return static_cast<Quick3DShaderDataArrayPrivate *>(Quick3DShaderDataArrayPrivate::get(self))->m_values.size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DShaderDataArray *self = static_cast<Quick3DShaderDataArray *>(list->object);
        return static_cast<Quick3DShaderDataArrayPrivate *>(Quick3DShaderDataArrayPrivate::get(self))->m_values.at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DShaderDataArray *self = static_cast<Quick3DShaderDataArray *>(list->object);
        static_cast<Quick3DShaderDataArrayPrivate *>(Quick3DShaderDataArrayPrivate::get(self))->m_values.clear();
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QList<QShaderData *> Quick3DShaderDataArray::values() const
{
    Q_D(const Quick3DShaderDataArray);
    return d->m_values;
}

// TODO: Avoid cloning here
//void Quick3DShaderDataArray::copy(const QNode *ref)
//{
//    QNode::copy(ref);
//    const Quick3DShaderDataArray *dataArray = static_cast<const Quick3DShaderDataArray *>(ref);
//    Q_FOREACH (QShaderData *v, dataArray->d_func()->m_values)
//        d_func()->m_values.append(static_cast<QShaderData *>(QNode::clone(v)));
//}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3dshaderdataarray_p.cpp"
