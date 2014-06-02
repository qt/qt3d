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

#ifndef QT3D_TECHNIQUEFILTER_H
#define QT3D_TECHNIQUEFILTER_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DRenderer/framegraphitem.h>
#include <Qt3DRenderer/techniquecriterion.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class TechniqueFilter;

class TechniqueFilterPrivate
{
public :
    TechniqueFilterPrivate(TechniqueFilter *qq) :
        q_ptr(qq)
    {}

    Q_DECLARE_PUBLIC(TechniqueFilter)
    TechniqueFilter *q_ptr;
    QList<TechniqueCriterion *> m_criteriaList;
};

class QT3DRENDERERSHARED_EXPORT TechniqueFilter : public Node, public FrameGraphItem
{
    Q_OBJECT
    Q_INTERFACES(Qt3D::FrameGraphItem)
public:
    explicit TechniqueFilter(Node *parent = 0)
        : Node(parent)
        , d_ptr(new TechniqueFilterPrivate(this))
    {}

    virtual ~TechniqueFilter()
    {}

    QList<TechniqueCriterion *> criteria() const
    {
        Q_D(const TechniqueFilter);
        return d->m_criteriaList;
    }

    void addCriterion(TechniqueCriterion *criterion)
    {
        Q_D(TechniqueFilter);
        d->m_criteriaList.append(criterion);
        emit criteriaChanged();
    }

    void removeCriterion(TechniqueCriterion *criterion)
    {
        Q_D(TechniqueFilter);
        d->m_criteriaList.removeOne(criterion);
        emit criteriaChanged();
    }

Q_SIGNALS:
    void criteriaChanged();
    void enabledChanged() Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(TechniqueFilter)
    TechniqueFilterPrivate *d_ptr;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_TECHNIQUEFILTER_H
