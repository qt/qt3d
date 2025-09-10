// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QFILTERKEY_H
#define QT3DRENDER_QFILTERKEY_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qnode.h>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFilterKeyPrivate;

class Q_3DRENDERSHARED_EXPORT QFilterKey : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
    explicit QFilterKey(Qt3DCore::QNode *parent = nullptr);
    ~QFilterKey();

    QVariant value() const;
    QString name() const;

public Q_SLOTS:
    void setValue(const QVariant &value);
    void setName(const QString &customType);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void valueChanged(const QVariant &value);

private:
    Q_DECLARE_PRIVATE(QFilterKey)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QFILTERKEY_H
