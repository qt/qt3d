// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QGRAPHICSAPIFILTER_P_H
#define QT3DRENDER_QGRAPHICSAPIFILTER_P_H

#include <private/qobject_p.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

struct Q_3DRENDERSHARED_PRIVATE_EXPORT GraphicsApiFilterData
{
    GraphicsApiFilterData();

    QGraphicsApiFilter::Api m_api;
    QGraphicsApiFilter::OpenGLProfile m_profile;
    int m_minor;
    int m_major;
    QStringList m_extensions;
    QString m_vendor;

    QString toString() const;

    bool operator ==(const GraphicsApiFilterData &other) const;
    bool operator !=(const GraphicsApiFilterData &other) const;
    bool operator <(const GraphicsApiFilterData &other) const;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT QGraphicsApiFilterPrivate : public QObjectPrivate
{
public:
    QGraphicsApiFilterPrivate()
        : QObjectPrivate()
    {
    }

    static QGraphicsApiFilterPrivate *get(QGraphicsApiFilter *q);
    static const QGraphicsApiFilterPrivate *get(const QGraphicsApiFilter *q);

    Q_DECLARE_PUBLIC(QGraphicsApiFilter)
    GraphicsApiFilterData m_data;
};

} // Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::GraphicsApiFilterData); // LCOV_EXCL_LINE

#endif // QT3DRENDER_QGRAPHICSAPIFILTER_P_H
