/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire
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

#ifndef QT3DRENDER_RENDER_LIGHTGATHERER_P_H
#define QT3DRENDER_RENDER_LIGHTGATHERER_P_H

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

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/lightsource_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class EntityManager;
class EnvironmentLight;

class Q_3DRENDERSHARED_PRIVATE_EXPORT LightGatherer : public Qt3DCore::QAspectJob
{
public:
    LightGatherer();

    inline void setManager(EntityManager *manager) Q_DECL_NOTHROW { m_manager = manager; }
    inline QVector<LightSource> &lights() { return m_lights; }
    inline EnvironmentLight *environmentLight() const { return m_environmentLight; }

    void run() override;

private:
    EntityManager *m_manager;
    QVector<LightSource> m_lights;
    EnvironmentLight *m_environmentLight;
};

typedef QSharedPointer<LightGatherer> LightGathererPtr;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_LIGHTGATHERER_P_H
