/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DCORE_QNODECREATEDCHANGE_H
#define QT3DCORE_QNODECREATEDCHANGE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qscenechange.h>

#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNodeCreatedChangeBasePrivate;

class QT3DCORESHARED_EXPORT QNodeCreatedChangeBase : public QSceneChange
{
public:
    explicit QNodeCreatedChangeBase(const QNode *node, Priority priority = QSceneChange::Standard);
    ~QNodeCreatedChangeBase();

    QNodeId parentId() const Q_DECL_NOEXCEPT;
    const QMetaObject *metaObject() const Q_DECL_NOEXCEPT;
    bool isNodeEnabled() const Q_DECL_NOEXCEPT;

protected:
    QNodeCreatedChangeBase(QNodeCreatedChangeBasePrivate &dd, const QNode *node, Priority priority);

private:
    Q_DECLARE_PRIVATE(QNodeCreatedChangeBase)
};

typedef QSharedPointer<QNodeCreatedChangeBase> QNodeCreatedChangeBasePtr;

template<typename T>
class QNodeCreatedChange : public QNodeCreatedChangeBase
{
public:
    explicit QNodeCreatedChange(const QNode *_node, Priority _priority = QSceneChange::Standard)
        : QNodeCreatedChangeBase(_node, _priority)
        , data()
    {
    }

    T data;
};

#if defined(Q_COMPILER_TEMPLATE_ALIAS)
template<typename T>
using QNodeCreatedChangePtr = QSharedPointer<QNodeCreatedChange<T>>;
#else
template <typename T>
struct QNodeCreatedChangePtr {
    static QSharedPointer<QNodeCreatedChange<T> > create(const QNode *node)
    {
        return QSharedPointer<QNodeCreatedChange<T> >::create(node);
    }
};
#endif

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QNODECREATEDCHANGE_H
