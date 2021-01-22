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
****************************************************************************/

#ifndef QT3DRENDER_QENVIRONMENTLIGHT_P_H
#define QT3DRENDER_QENVIRONMENTLIGHT_P_H

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

#include <private/qcomponent_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <qshaderdata.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTexture;
class QEnvironmentLight;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QEnvironmentLightPrivate : public Qt3DCore::QComponentPrivate
{
public:
    explicit QEnvironmentLightPrivate();
    ~QEnvironmentLightPrivate();

    Q_DECLARE_PUBLIC(QEnvironmentLight)
    QShaderData *m_shaderData;
    QAbstractTexture *m_irradiance;
    QAbstractTexture *m_specular;

private:
    void _q_updateEnvMapsSize();
};

struct QEnvironmentLightData
{
    Qt3DCore::QNodeId shaderDataId;
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QENVIRONMENTLIGHT_P_H
