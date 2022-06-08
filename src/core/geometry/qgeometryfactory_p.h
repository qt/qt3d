// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
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
#include <private/qglobal_p.h>

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
