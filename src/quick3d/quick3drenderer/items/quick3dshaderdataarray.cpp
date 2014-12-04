/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quick3dshaderdataarray.h"
#include <private/qshaderdata_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

class Quick3DShaderDataArrayPrivate : public QNodePrivate
{
public:
    Quick3DShaderDataArrayPrivate(Quick3DShaderDataArray *qq)
        : QNodePrivate(qq)
    {}

    QList<Quick3DShaderData *> m_values;
};

Quick3DShaderDataArray::Quick3DShaderDataArray(QNode *parent)
    : QNode(*new Quick3DShaderDataArrayPrivate(this), parent)
{
}

QQmlListProperty<Quick3DShaderData> Quick3DShaderDataArray::valuesList()
{
    return QQmlListProperty<Quick3DShaderData>(this, 0,
                                               &Quick3DShaderDataArray::appendValue,
                                               &Quick3DShaderDataArray::valueCount,
                                               &Quick3DShaderDataArray::valueAt,
                                               &Quick3DShaderDataArray::clearValues);
}

QList<Quick3DShaderData *> Quick3DShaderDataArray::values() const
{
    Q_D(const Quick3DShaderDataArray);
    return d->m_values;
}

void Quick3DShaderDataArray::copy(const QNode *ref)
{
    QNode::copy(ref);
    const Quick3DShaderDataArray *dataArray = static_cast<const Quick3DShaderDataArray *>(ref);
    Q_FOREACH (Quick3DShaderData *v, dataArray->d_func()->m_values)
        d_func()->m_values.append(static_cast<Quick3DShaderData *>(QNode::clone(v)));
}

void Quick3DShaderDataArray::appendValue(QQmlListProperty<Quick3DShaderData> *list, Quick3DShaderData *bar)
{
    Quick3DShaderDataArray *self = static_cast<Quick3DShaderDataArray *>(list->object);
    static_cast<Quick3DShaderDataArrayPrivate *>(Quick3DShaderDataArrayPrivate::get(self))->m_values.append(bar);
}

Quick3DShaderData *Quick3DShaderDataArray::valueAt(QQmlListProperty<Quick3DShaderData> *list, int index)
{
    Quick3DShaderDataArray *self = static_cast<Quick3DShaderDataArray *>(list->object);
    return static_cast<Quick3DShaderDataArrayPrivate *>(Quick3DShaderDataArrayPrivate::get(self))->m_values.at(index);
}

int Quick3DShaderDataArray::valueCount(QQmlListProperty<Quick3DShaderData> *list)
{
    Quick3DShaderDataArray *self = static_cast<Quick3DShaderDataArray *>(list->object);
    return static_cast<Quick3DShaderDataArrayPrivate *>(Quick3DShaderDataArrayPrivate::get(self))->m_values.count();
}

void Quick3DShaderDataArray::clearValues(QQmlListProperty<Quick3DShaderData> *list)
{
    Quick3DShaderDataArray *self = static_cast<Quick3DShaderDataArray *>(list->object);
    static_cast<Quick3DShaderDataArrayPrivate *>(Quick3DShaderDataArrayPrivate::get(self))->m_values.clear();
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
