/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QASPECTFACTORY_H
#define QT3D_QASPECTFACTORY_H

#include <QSharedDataPointer>
#include <QStringList>

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

class QObject;

namespace Qt3D {

class QAbstractAspect;
class QAspectFactoryPrivate;

class QT3DCORESHARED_EXPORT QAspectFactory
{
public:
    typedef QAbstractAspect *(*CreateFunction)(QObject *);

    template<class AspectType>
    static QAbstractAspect *functionHelper(QObject *parent)
    {
        return new AspectType(parent);
    }

    QAspectFactory();
    QAspectFactory(const QAspectFactory &other);
    ~QAspectFactory();

    QAspectFactory &operator=(const QAspectFactory &other);

    void addFactory(const QString &name, CreateFunction factory);
    QStringList availableFactories() const;

    QAbstractAspect *createAspect(const QString &aspect, QObject *parent = 0) const;

private:
    QSharedDataPointer<QAspectFactoryPrivate> d;
};

QT3DCORESHARED_EXPORT void _qt3d_QAspectFactoryPrivate_addDefaultFactory(const QString &name, QAspectFactory::CreateFunction);

} // namespace Qt3D

QT_END_NAMESPACE

#define QT3D_REGISTER_NAMESPACED_ASPECT(name, AspectNamespace, AspectType) \
    namespace { \
    using namespace QT_PREPEND_NAMESPACE(Qt3D); \
    \
    void _qt3d_ ## AspectNamespace ## _ ## AspectType ## _registerFunction() \
    { \
        _qt3d_QAspectFactoryPrivate_addDefaultFactory(QStringLiteral(name), &QAspectFactory::functionHelper<AspectNamespace::AspectType>); \
    } \
    \
    Q_CONSTRUCTOR_FUNCTION(_qt3d_ ## AspectNamespace ## _ ## AspectType ## _registerFunction) \
    }

#define QT3D_REGISTER_ASPECT(name, AspectType) \
    namespace { \
    void _qt3d_ ## AspectType ## _registerFunction() \
    { \
        QT_PREPEND_NAMESPACE(Qt3D::_qt3d_QAspectFactoryPrivate_addDefaultFactory)(QStringLiteral(name), &QAspectFactory::functionHelper< ::AspectType>); \
    } \
    \
    } \
    Q_CONSTRUCTOR_FUNCTION(_qt3d_ ## AspectType ## _registerFunction)

#endif // QT3D_QASPECTFACTORY_H
