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

#ifndef QT3DRENDER_RENDER_FILTERENTITYBYCOMPONENTJOB_H
#define QT3DRENDER_RENDER_FILTERENTITYBYCOMPONENTJOB_H

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
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Entity;
class EntityManager;

template<typename T, typename ... Ts>
class FilterEntityByComponentJob : public Qt3DCore::QAspectJob
{
public:
    FilterEntityByComponentJob()
        : Qt3DCore::QAspectJob()
        , m_manager(nullptr)
    {
        SET_JOB_RUN_STAT_TYPE(this, JobTypes::EntityComponentTypeFiltering, 0)
    }

    inline void setManager(EntityManager *manager) Q_DECL_NOTHROW { m_manager = manager; }
    inline QVector<Entity *> &filteredEntities() Q_DECL_NOTHROW { return m_filteredEntities; }

    void run() override
    {
        m_filteredEntities.clear();
        const std::vector<HEntity> &handles = m_manager->activeHandles();
        m_filteredEntities.reserve(handles.size());
        for (const HEntity &handle : handles) {
            Entity *e = m_manager->data(handle);
            if (e->containsComponentsOfType<T, Ts...>())
                m_filteredEntities.push_back(e);
        }
    }

private:
    EntityManager *m_manager;
    QVector<Entity *> m_filteredEntities;
};

template<typename T, typename ... Ts>
using FilterEntityByComponentJobPtr = QSharedPointer<FilterEntityByComponentJob<T, Ts...>>;

} // Render

} // Qt3DRender

QT_END_NAMESPACE
#endif // QT3DRENDER_RENDER_FILTERENTITYBYCOMPONENTJOB_H
