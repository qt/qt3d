/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DEXTRAS_QEXTRUDEDTEXTGEOMETRY_H
#define QT3DEXTRAS_QEXTRUDEDTEXTGEOMETRY_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/qgeometry.h>
#include <QtCore/QString>
#include <QtGui/QFont>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;

} // namespace Qt3DCore

namespace Qt3DExtras {

class QExtrudedTextGeometryPrivate;

class Q_3DEXTRASSHARED_EXPORT QExtrudedTextGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(float extrusionLength READ extrusionLength WRITE setDepth NOTIFY depthChanged)
    Q_PROPERTY(Qt3DCore::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit QExtrudedTextGeometry(Qt3DCore::QNode *parent = nullptr);
    ~QExtrudedTextGeometry();

    Qt3DCore::QAttribute *positionAttribute() const;
    Qt3DCore::QAttribute *normalAttribute() const;
    Qt3DCore::QAttribute *indexAttribute() const;
    QString text() const;
    QFont font() const;
    float extrusionLength() const;

public Q_SLOTS:
    void setText(const QString &text);
    void setFont(const QFont &font);
    void setDepth(float extrusionLength);

Q_SIGNALS:
    void textChanged(const QString &text);
    void fontChanged(const QFont &font);
    void depthChanged(float extrusionLength);

protected:
    explicit QExtrudedTextGeometry(QExtrudedTextGeometryPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QExtrudedTextGeometry)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QEXTRUDEDTEXTGEOMETRY_H
