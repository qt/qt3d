/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

#ifndef QT3DCORE_QGEOMETRYFACTORY_P_H
#define QT3DCORE_QGEOMETRYFACTORY_P_H

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

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qabstractfunctor.h>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QGeometry;

class Q_3DCORESHARED_EXPORT QGeometryFactory : public QAbstractFunctor
{
public:
    virtual ~QGeometryFactory();
    virtual QGeometry *operator()() = 0;
    virtual bool equals(const QGeometryFactory &other) const = 0;
    friend bool operator==(const QGeometryFactory &lhs, const QGeometryFactory &rhs)
    { return lhs.equals(rhs); }
};

typedef QSharedPointer<QGeometryFactory> QGeometryFactoryPtr;

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DCore::QGeometryFactoryPtr) // LCOV_EXCL_LINE

#endif // QT3DCORE_QGEOMETRYFACTORY_P_H
