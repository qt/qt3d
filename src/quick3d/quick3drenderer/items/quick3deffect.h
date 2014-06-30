/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_RENDER_QUICK_QUICK3DEFFECT_H
#define QT3D_RENDER_QUICK_QUICK3DEFFECT_H

#include <Qt3DQuickRenderer/qt3dquickrenderer_global.h>
#include <Qt3DCore/qabstracteffect.h>
#include <QQmlListProperty>
#include <Qt3DRenderer/qtechnique.h>


QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

class QT3DQUICKRENDERERSHARED_EXPORT Quick3DEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3D::QTechnique> techniques READ techniqueList)
    Q_PROPERTY(QQmlListProperty<Qt3D::Parameter> parameters READ parameterList)

public:
    explicit Quick3DEffect(QObject *parent = 0);

    inline QAbstractEffect *parentEffect() const { return qobject_cast<QAbstractEffect*>(parent()); }

    QQmlListProperty<Qt3D::QTechnique> techniqueList();
    QQmlListProperty<Qt3D::Parameter> parameterList();

private:
    static void appendTechnique(QQmlListProperty<QTechnique> *list, QTechnique *bar);
    static QTechnique *techniqueAt(QQmlListProperty<QTechnique> *list, int index);
    static int techniqueCount(QQmlListProperty<QTechnique> *list);
    static void clearTechniqueList(QQmlListProperty<QTechnique> *list);

    static void appendParameter(QQmlListProperty<Parameter> *list, Parameter *param);
    static Parameter *parameterAt(QQmlListProperty<Parameter> *list, int index);
    static int parametersCount(QQmlListProperty<Parameter> *list);
    static void clearParameterList(QQmlListProperty<Parameter> *list);
};

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QUICK_QUICK3DEFFECT_H
