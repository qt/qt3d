/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DCORE_QCOLLISIONQUERYRESULT_H
#define QT3DCORE_QCOLLISIONQUERYRESULT_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnodeid.h>
#include <QVector>
#include <QSharedData>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

typedef int QQueryHandle;

class QCollisionQueryResultPrivate;

class QT3DCORESHARED_EXPORT QCollisionQueryResult
{
public:
    QCollisionQueryResult();
    QCollisionQueryResult(const QCollisionQueryResult &);
    ~QCollisionQueryResult();

    QCollisionQueryResult &operator=(const QCollisionQueryResult &);
#ifdef Q_COMPILER_RVALUE_REFS
    QCollisionQueryResult &operator=(QCollisionQueryResult &&other) Q_DECL_NOTHROW
    {
        swap(other);
        return *this;
    }
#endif

    void swap(QCollisionQueryResult &other) Q_DECL_NOTHROW
    {
        qSwap(d_ptr, other.d_ptr);
    }

    QQueryHandle handle() const;
    QVector<QNodeId> entitiesHit() const;

private:
    friend class QAbstractCollisionQueryService;

    explicit QCollisionQueryResult(QCollisionQueryResultPrivate &p);

    QSharedDataPointer<QCollisionQueryResultPrivate> d_ptr;
    // Q_DECLARE_PRIVATE equivalent for shared data pointers
    QCollisionQueryResultPrivate *d_func();
    inline const QCollisionQueryResultPrivate *d_func() const
    {
        return d_ptr.constData();
    }
};

} // Qt3D

Q_DECLARE_SHARED(Qt3DCore::QCollisionQueryResult)

QT_END_NAMESPACE

#endif // QT3DCORE_QCOLLISIONQUERYRESULT_H

