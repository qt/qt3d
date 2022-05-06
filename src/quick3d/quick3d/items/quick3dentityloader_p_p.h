/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

#ifndef QT3D_QUICK_QUICK3DENTITYLOADER_P_P_H
#define QT3D_QUICK_QUICK3DENTITYLOADER_P_P_H

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

#include <QtQml/QQmlComponent>

#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DQuick/private/quick3dentityloader_p.h>

QT_BEGIN_NAMESPACE

class QQmlIncubator;
class QQmlContext;

namespace Qt3DCore {

class QEntity;

namespace Quick {

class Quick3DEntityLoaderIncubator;

class Quick3DEntityLoaderPrivate : public QEntityPrivate
{
public:
    Quick3DEntityLoaderPrivate();

    Q_DECLARE_PUBLIC(Quick3DEntityLoader)

    void clear();
    void loadFromSource();
    void loadComponent(const QUrl &source);
    void loadComponent(QQmlComponent *component);

    void _q_componentStatusChanged(QQmlComponent::Status status);
    void setStatus(Quick3DEntityLoader::Status status);

    static inline Quick3DEntityLoaderPrivate *get(Quick3DEntityLoader *q) { return q->d_func(); }

    QUrl m_source;
    Quick3DEntityLoaderIncubator *m_incubator;
    QQmlContext *m_context;
    QQmlComponent *m_component;
    QQmlComponent *m_sourceComponent;
    QEntity *m_entity;
    Quick3DEntityLoader::Status m_status;
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK_QUICK3DENTITYLOADER_P_P_H
