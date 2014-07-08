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

#ifndef QT3D_QRENDERPASS_H
#define QT3D_QRENDERPASS_H

#include <Qt3DCore/qabstractrenderpass.h>
#include <Qt3DRenderer/qt3drenderer_global.h>

#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/drawstate.h>
#include <Qt3DRenderer/qrenderpasscriterion.h>

#include <QHash>
#include <QList>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QParameter;
class QParameterMapper;
typedef QList<QParameter*> ParameterList;

class QRenderPassPrivate;

class QT3DRENDERERSHARED_EXPORT QRenderPass : public QAbstractRenderPass
{
    Q_OBJECT

public:
    explicit QRenderPass(QNode *parent = 0);

    void addUniformBinding(QParameter* param, QString glslUniformName);

    void addAttributeBinding(QParameter* param, QString glslAttributeName);

    QString glslNameForParameter(QString paramName) const;

    ParameterList attributes() const;
    ParameterList uniforms() const;

    void setStateSet(Render::DrawStateSet* ss);
    Render::DrawStateSet* stateSet() const;

    void addCriterion(QRenderPassCriterion *criterion);
    void removeCriterion(QRenderPassCriterion *criterion);
    QList<QRenderPassCriterion *> criteria() const;

    void addBinding(QParameterMapper *binding);
    void removeBinding(QParameterMapper *binding);
    QList<QParameterMapper *> bindings() const;

protected:
    Q_DECLARE_PRIVATE(QRenderPass)
    QRenderPass(QRenderPassPrivate &dd, QNode *parent = 0);
};

}

QT_END_NAMESPACE

#endif // QT3D_QRENDERPASS_H
