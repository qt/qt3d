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

#ifndef QT3D_TECHNIQUE_H
#define QT3D_TECHNIQUE_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DCore/node.h>
#include <Qt3DRenderer/tag.h>
#include <Qt3DRenderer/renderpass.h>
#include <Qt3DCore/qabstracttechnique.h>
#include <Qt3DRenderer/techniquecriterion.h>
#include <QList>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class Parameter;

class QT3DRENDERERSHARED_EXPORT Technique : public QAbstractTechnique
{
    Q_OBJECT

public:
    explicit Technique(Node *parent = 0);

    void addCriterion(TechniqueCriterion *criterion);
    void removeCriterion(TechniqueCriterion *criterion);
    QVariant criterionValue(const QString &customTypeName) const;
    QVariant criterionValue(TechniqueCriterion::CriterionType type);
    QList<TechniqueCriterion *> criteria() const;
    void clearCriteria();
    bool containsCriterion(const QString &customTypeName) const;
    bool containsCriterion(TechniqueCriterion::CriterionType type) const;

    void addPass(QAbstractRenderPass* pass) Q_DECL_OVERRIDE;

    void addParameter(Parameter *p);
    void removeParameter(Parameter *p);
    QList<Parameter *> parameters() const
    { return m_parameters; }

    Parameter* parameterByName(QString name) const;

private:

    QList<TechniqueCriterion *> m_criteriaList;
    QList<Parameter *> m_parameters;
};

}

QT_END_NAMESPACE

#endif // QT3D_TECHNIQUE_H
