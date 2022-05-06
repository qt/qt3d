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

#ifndef QT3DRENDER_QPARAMETER_H
#define QT3DRENDER_QPARAMETER_H

#include <QtCore/QVariant>
#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QParameterPrivate;
class QAbstractTexture;

class Q_3DRENDERSHARED_EXPORT QParameter : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit QParameter(Qt3DCore::QNode *parent = nullptr);
    explicit QParameter(const QString &name, const QVariant& value, Qt3DCore::QNode *parent = nullptr);
    explicit QParameter(const QString &name, QAbstractTexture *texture, Qt3DCore::QNode *parent = nullptr);
    ~QParameter();

    QString name() const;
    QVariant value() const;

public Q_SLOTS:
    void setName(const QString &name);
    void setValue(const QVariant &dv);

Q_SIGNALS:
    void valueChanged(const QVariant &value);
    void nameChanged(const QString &name);

protected:
    QParameter(QParameterPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QParameter)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_PARAMETER_H
