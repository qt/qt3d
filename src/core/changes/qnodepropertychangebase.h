/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DCORE_QNODEPROPERTYCHANGEBASE_H
#define QT3DCORE_QNODEPROPERTYCHANGEBASE_H

#include <Qt3DCore/qscenechange.h>
#include <Qt3DCore/qnodeupdatedchangebase.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNodePropertyChangeBasePrivate;

class QT3DCORESHARED_EXPORT QNodePropertyChangeBase : public QNodeUpdatedChangeBase
{
public:
    QNodePropertyChangeBase(ChangeFlag type, SenderType senderType, QNodeId subjectId, Priority priority = Standard);
    virtual ~QNodePropertyChangeBase();

    const char *propertyName() const;
    void setPropertyName(const char *name);

protected:
    Q_DECLARE_PRIVATE(QNodePropertyChangeBase)
    QNodePropertyChangeBase(QNodePropertyChangeBasePrivate &dd, ChangeFlag type, SenderType senderType, QNodeId subjectId, Priority priority = Standard);
};

typedef QSharedPointer<QNodePropertyChangeBase> QNodePropertyChangeBasePtr;

template<typename T>
class QTypedNodePropertyChange : public QNodePropertyChangeBase
{
public:
    explicit QTypedNodePropertyChange(QNodeId _subjectId, Priority _priority = QSceneChange::Standard)
        : QNodePropertyChangeBase(PropertyUpdated, Node, _subjectId, _priority)
        , data()
    {
    }

    T data;
};

#if defined(Q_COMPILER_TEMPLATE_ALIAS)
template<typename T>
using QTypedNodePropertyChangePtr = QSharedPointer<QTypedNodePropertyChange<T>>;
#else
template <typename T>
struct QTypedNodePropertyChangePtr {
    static QSharedPointer<QTypedNodePropertyChange<T> > create(QNodeId subjectId)
    {
        return QSharedPointer<QTypedNodePropertyChange<T> >::create(subjectId);
    }
};
#endif

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QNODEPROPERTYCHANGEBASE_H
