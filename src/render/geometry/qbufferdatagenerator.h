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

#ifndef QT3DRENDER_QBUFFERDATAGENERATOR
#define QT3DRENDER_QBUFFERDATAGENERATOR

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qabstractfunctor.h>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_EXPORT QBufferDataGenerator : public QAbstractFunctor
{
public:
    virtual ~QBufferDataGenerator() {}
    virtual QByteArray operator()() = 0;
    virtual bool operator ==(const QBufferDataGenerator &other) const = 0;
};

typedef QSharedPointer<QBufferDataGenerator> QBufferDataGeneratorPtr;

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QBufferDataGeneratorPtr) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QBUFFERDATAGENERATOR

