/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QSHADERDATA_H
#define QT3DRENDER_QSHADERDATA_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QShaderDataPrivate;

class Q_3DRENDERSHARED_EXPORT PropertyReaderInterface
{
public:
    virtual ~PropertyReaderInterface();
    virtual QVariant readProperty(const QVariant &v) = 0;
};

typedef QSharedPointer<PropertyReaderInterface> PropertyReaderInterfacePtr;

class Q_3DRENDERSHARED_EXPORT QShaderData : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit QShaderData(Qt3DCore::QNode *parent = nullptr);
    ~QShaderData();

    PropertyReaderInterfacePtr propertyReader() const;

    bool event(QEvent *event) override;

protected:
    explicit QShaderData(QShaderDataPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QShaderData)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderData*) // LCOV_EXCL_LINE
Q_DECLARE_METATYPE(QList<Qt3DRender::QShaderData*>) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QSHADERDATA_H
