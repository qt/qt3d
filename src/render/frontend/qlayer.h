// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QLAYER_H
#define QT3DRENDER_QLAYER_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QLayerPrivate;

class Q_3DRENDERSHARED_EXPORT QLayer : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(bool recursive READ recursive WRITE setRecursive NOTIFY recursiveChanged)
public:
    explicit QLayer(Qt3DCore::QNode *parent = nullptr);
    ~QLayer();

    bool recursive() const;
    void setRecursive(bool recursive);

Q_SIGNALS:
    void recursiveChanged();

protected:
    explicit QLayer(QLayerPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QLayer)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QLAYER_H
