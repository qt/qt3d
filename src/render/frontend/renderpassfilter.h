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

#ifndef QT3D_RENDERPASSFILTER_H
#define QT3D_RENDERPASSFILTER_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DRenderer/framegraphitem.h>
#include <Qt3DRenderer/renderpasscriterion.h>

#include <Qt3DCore/node.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <QString>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class RenderPassFilter;

// TO DO: Move that to a proper cpp file
class RenderPassFilterPrivate
{
public:
    RenderPassFilterPrivate(RenderPassFilter *qq)
        : q_ptr(qq)
    {}

    Q_DECLARE_PUBLIC(RenderPassFilter)
    RenderPassFilter *q_ptr;
    QString m_renderPassName;
    QList<RenderPassCriterion *> m_criteriaList;
};

class QT3DRENDERERSHARED_EXPORT RenderPassFilter : public Node, public FrameGraphItem
{
    Q_OBJECT
    Q_INTERFACES(Qt3D::FrameGraphItem)
    Q_PROPERTY(QString renderPassName READ renderPassName WRITE setRenderPassName NOTIFY renderPassNameChanged)

public:
    explicit RenderPassFilter(Node *parent = 0)
        : Node(parent)
        , d_ptr(new RenderPassFilterPrivate(this))
    {}

    ~RenderPassFilter() {}

    void setRenderPassName(const QString &renderpassName)
    {
        Q_D(RenderPassFilter);
        if (renderpassName != d->m_renderPassName) {
            d->m_renderPassName = renderpassName;
            emit renderPassNameChanged();
        }
    }

    QString renderPassName() const
    {
        Q_D(const RenderPassFilter);
        return d->m_renderPassName;
    }

    QList<RenderPassCriterion *> criteria() const
    {
        Q_D(const RenderPassFilter);
        return d->m_criteriaList;
    }

    void addCriterion(RenderPassCriterion *criterion)
    {
        Q_D(RenderPassFilter);
        if (!d->m_criteriaList.contains(criterion)) {
            d->m_criteriaList.append(criterion);
            QScenePropertyChangePtr propertyChange(new QScenePropertyChange(ComponentAdded, this));
            propertyChange->m_propertyName = QByteArrayLiteral("renderPassCriteria");
            propertyChange->m_value = QVariant::fromValue(criterion);
            notifyObservers(propertyChange);
        }
    }

    void removeCriterion(RenderPassCriterion *criterion)
    {
        Q_D(RenderPassFilter);
        d->m_criteriaList.removeOne(criterion);
        QScenePropertyChangePtr propertyChange(new QScenePropertyChange(ComponentRemoved, this));
        propertyChange->m_propertyName = QByteArrayLiteral("renderPassCriteria");
        propertyChange->m_value = QVariant::fromValue(criterion);
        notifyObservers(propertyChange);

    }

Q_SIGNALS:
    void renderPassNameChanged();
    void enabledChanged() Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(RenderPassFilter)
    RenderPassFilterPrivate *d_ptr;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDERPASSFILTER_H
