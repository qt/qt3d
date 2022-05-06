/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DCORE_QASPECTFACTORY_P_H
#define QT3DCORE_QASPECTFACTORY_P_H

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

#include <QtCore/QHash>
#include <QtCore/QStringList>

#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

struct QMetaObject;
class QObject;

namespace Qt3DCore {

class QAbstractAspect;

class Q_3DCORE_PRIVATE_EXPORT QAspectFactory
{
public:
    typedef QAbstractAspect *(*CreateFunction)(QObject *);

    QAspectFactory();
    QAspectFactory(const QAspectFactory &other);
    ~QAspectFactory();
    QAspectFactory &operator=(const QAspectFactory &other);
#ifdef Q_COMPILER_RVALUE_REFS
    QAspectFactory &operator=(QAspectFactory &&other) noexcept
    {
        m_factories.swap(other.m_factories);
        m_aspectNames.swap(other.m_aspectNames);
        return *this;
    }
#endif

    inline void swap(QAspectFactory &other) noexcept
    {
        m_factories.swap(other.m_factories);
        m_aspectNames.swap(other.m_aspectNames);
    }

    QList<QLatin1String> availableFactories() const;
    QAbstractAspect *createAspect(const QLatin1String &aspect, QObject *parent = 0) const;
    QLatin1String aspectName(QAbstractAspect *aspect) const;

private:
    QHash<QLatin1String, QAspectFactory::CreateFunction> m_factories;
    QHash<const QMetaObject*, QLatin1String> m_aspectNames;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QASPECTFACTORY_P_H
