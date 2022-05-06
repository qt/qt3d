/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QT3DRENDER_QSHADERNODESLOADER_P_H
#define QT3DRENDER_QSHADERNODESLOADER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/qt3drender_global_p.h>

#include <Qt3DRender/private/qshadergraph_p.h>

QT_BEGIN_NAMESPACE

class QIODevice;

namespace Qt3DRender
{
class QShaderNodesLoader
{
public:
    enum Status : char {
        Null,
        Waiting,
        Ready,
        Error
    };

    Q_3DRENDERSHARED_PRIVATE_EXPORT QShaderNodesLoader() noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT Status status() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT QHash<QString, QShaderNode> nodes() const noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QIODevice *device() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setDevice(QIODevice *device) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT void load();
    Q_3DRENDERSHARED_PRIVATE_EXPORT void load(const QJsonObject &prototypesObject);

private:
    Status m_status;
    QIODevice *m_device;
    QHash<QString, QShaderNode> m_nodes;
};


}
Q_DECLARE_TYPEINFO(Qt3DRender::QShaderNodesLoader, Q_RELOCATABLE_TYPE);
QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderNodesLoader)
Q_DECLARE_METATYPE(Qt3DRender::QShaderNodesLoader::Status)

#endif // QT3DRENDER_QSHADERNODESLOADER_P_H
