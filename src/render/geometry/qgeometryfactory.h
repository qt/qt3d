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
****************************************************************************/

#ifndef QT3DRENDER_QGEOMETRYFACTORY
#define QT3DRENDER_QGEOMETRYFACTORY

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qabstractfunctor.h>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QGeometry;

class Q_3DRENDERSHARED_EXPORT QGeometryFactory : public QAbstractFunctor
{
public:
    virtual ~QGeometryFactory();
    virtual QGeometry *operator()() = 0;
    virtual bool operator ==(const QGeometryFactory &other) const = 0;
};

typedef QSharedPointer<QGeometryFactory> QGeometryFactoryPtr;

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QGeometryFactoryPtr) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QGEOMETRYFACTORY

