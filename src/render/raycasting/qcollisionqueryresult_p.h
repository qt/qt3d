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

#ifndef QT3DRENDER_QCOLLISIONQUERYRESULT_P_H
#define QT3DRENDER_QCOLLISIONQUERYRESULT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qnodeid.h>
#include <QVector>
#include <QSharedData>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

typedef int QQueryHandle;

class QCollisionQueryResultPrivate : public QSharedData
{
public:
    explicit QCollisionQueryResultPrivate();
    explicit QCollisionQueryResultPrivate(const QCollisionQueryResultPrivate &copy);

    void setHandle(const QQueryHandle &handle);
    void addEntityHit(const Qt3DCore::QNodeId &entity);

    QQueryHandle m_handle;
    QVector<Qt3DCore::QNodeId> m_entitiesHit;
};

class QT3DRENDERSHARED_EXPORT QCollisionQueryResult
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
    QVector<Qt3DCore::QNodeId> entitiesHit() const;

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

} // Qt3DRender

Q_DECLARE_SHARED(Qt3DRender::QCollisionQueryResult)

QT_END_NAMESPACE

#endif // QT3DRENDER_QCOLLISIONQUERYRESULT_P_H
