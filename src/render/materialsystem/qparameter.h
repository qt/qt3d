// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
