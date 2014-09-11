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

#include "qabstractrenderpass.h"
#include "qabstractrenderpass_p.h"
#include "qabstractshader.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QAbstractRenderPassPrivate::QAbstractRenderPassPrivate(QAbstractRenderPass *qq)
    : QNodePrivate(qq)
    , m_shader(Q_NULLPTR)
{
}

QAbstractRenderPass::QAbstractRenderPass(QNode *parent)
    : QNode(*new QAbstractRenderPassPrivate(this), parent)
{
}

QAbstractRenderPass::QAbstractRenderPass(QAbstractRenderPassPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QAbstractRenderPass::setName(const QString &name)
{
    Q_D(QAbstractRenderPass);
    if (d->m_name != name) {
        d->m_name = name;
        emit nameChanged();
    }
}

QString QAbstractRenderPass::name() const
{
    Q_D(const QAbstractRenderPass);
    return d->m_name;
}

/*!
 * Sets the pass's \a shaderProgram. This posts a ComponentUpdated
 * QScenePropertyChange to the QChangeArbiter. The value is set to
 * the \a ShaderProgram and the property name to "shaderProgram".
 */
void QAbstractRenderPass::setShaderProgram(QAbstractShader *shaderProgram)
{
    Q_D(QAbstractRenderPass);
    if (d->m_shader != shaderProgram) {

        if (d->m_shader != Q_NULLPTR && d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeRemoved, this));
            e->setPropertyName(QByteArrayLiteral("shaderProgram"));
            e->setValue(QVariant::fromValue(d->m_shader->uuid()));
            notifyObservers(e);
        }

        d->m_shader = shaderProgram;
        emit shaderProgramChanged();

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!shaderProgram->parent() || shaderProgram->parent() == this)
            QNode::addChild(shaderProgram);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr e(new QScenePropertyChange(NodeAdded, this));
            e->setPropertyName(QByteArrayLiteral("shaderProgram"));
            e->setValue(QVariant::fromValue(shaderProgram->uuid()));
            notifyObservers(e);
        }
    }
}

QAbstractShader *QAbstractRenderPass::shaderProgram() const
{
    Q_D(const QAbstractRenderPass);
    return d->m_shader;
}

} // Qt3D

QT_END_NAMESPACE
