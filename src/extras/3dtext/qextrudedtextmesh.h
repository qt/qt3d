// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QEXTRUDEDTEXTMESH_H
#define QT3DEXTRAS_QEXTRUDEDTEXTMESH_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <QtCore/QString>
#include <QtGui/QFont>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class Q_3DEXTRASSHARED_EXPORT QExtrudedTextMesh : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(float depth READ depth WRITE setDepth NOTIFY depthChanged)

public:
    explicit QExtrudedTextMesh(Qt3DCore::QNode *parent = nullptr);
    ~QExtrudedTextMesh();

    QString text() const;
    QFont font() const;
    float depth() const;

public Q_SLOTS:
    void setText(const QString &text);
    void setFont(const QFont &font);
    void setDepth(float depth);

Q_SIGNALS:
    void textChanged(const QString &text);
    void fontChanged(const QFont &font);
    void depthChanged(float depth);
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QEXTRUDEDTEXTMESH_H
