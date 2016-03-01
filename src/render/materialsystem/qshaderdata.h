/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QSHADERDATA_H
#define QT3DRENDER_QSHADERDATA_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QShaderDataPrivate;

class QT3DRENDERSHARED_EXPORT PropertyReaderInterface
{
public:
    virtual ~PropertyReaderInterface() {}
    virtual QVariant readProperty(const QVariant &v) = 0;
};

typedef QSharedPointer<PropertyReaderInterface> PropertyReaderInterfacePtr;

class QT3DRENDERSHARED_EXPORT QShaderData : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit QShaderData(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QShaderData();

    enum TransformType {
        ModelToEye = 0,
        ModelToWorld
    };
    Q_ENUM(TransformType)

    PropertyReaderInterfacePtr propertyReader() const;

protected:
    QShaderData(QShaderDataPrivate &dd, Qt3DCore::QNode *parent = Q_NULLPTR);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;


private:
    Q_DECLARE_PRIVATE(QShaderData)
    QT3D_CLONEABLE(QShaderData)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderData*)
Q_DECLARE_METATYPE(QList<Qt3DRender::QShaderData*>)

#endif // QT3DRENDER_QSHADERDATA_H
