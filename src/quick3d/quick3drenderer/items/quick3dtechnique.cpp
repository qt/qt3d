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

#include "quick3dtechnique.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DTechnique::Quick3DTechnique(Node *parent)
    : Technique(parent)
{
}

QQmlListProperty<Qt3D::RenderPass> Quick3DTechnique::renderPassList()
{
    return QQmlListProperty<Qt3D::RenderPass>(this, 0,
                                              &Quick3DTechnique::appendRenderPass,
                                              &Quick3DTechnique::renderPassCount,
                                              &Quick3DTechnique::renderPassAt,
                                              &Quick3DTechnique::clearRenderPasses);
}

void Quick3DTechnique::appendRenderPass(QQmlListProperty<RenderPass> *list, RenderPass *renderPass)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        technique->addPass(renderPass);
        emit technique->renderPassesChanged();
    }
}

RenderPass *Quick3DTechnique::renderPassAt(QQmlListProperty<RenderPass> *list, int index)
{
    Technique *technique = qobject_cast<Technique *>(list->object);
    if (technique)
        return technique->renderPasses().at(index);
    return 0;
}

int Quick3DTechnique::renderPassCount(QQmlListProperty<RenderPass> *list)
{
    Technique *technique = qobject_cast<Technique *>(list->object);
    if (technique)
        return technique->renderPasses().size();
    return 0;
}

void Quick3DTechnique::clearRenderPasses(QQmlListProperty<RenderPass> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        Q_FOREACH (RenderPass *pass, technique->renderPasses())
            technique->removePass(pass);
        emit technique->renderPassesChanged();
    }
}

QQmlListProperty<Qt3D::Tag> Quick3DTechnique::tags()
{
    return QQmlListProperty<Qt3D::Tag>(this, 0,
                                       &Quick3DTechnique::appendTag,
                                       &Quick3DTechnique::tagCount,
                                       &Quick3DTechnique::tagAt,
                                       &Quick3DTechnique::clearTagsList);
}

void Quick3DTechnique::appendTag(QQmlListProperty<Tag> *list, Tag *tag)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        tag->setParent(technique);
        technique->addTag(tag->name(), QVariant::fromValue(tag));
        emit technique->tagsChanged();
    }
}

Tag *Quick3DTechnique::tagAt(QQmlListProperty<Tag> *list, int index)
{
    Technique *technique = qobject_cast<Technique *>(list->object);
    if (technique)
        return technique->tags().at(index);
    return 0;
}

int Quick3DTechnique::tagCount(QQmlListProperty<Tag> *list)
{
    Technique *technique = qobject_cast<Technique *>(list->object);
    if (technique)
        return technique->tags().size();
    return 0;
}

void Quick3DTechnique::clearTagsList(QQmlListProperty<Tag> *list)
{
    Quick3DTechnique *technique = qobject_cast<Quick3DTechnique *>(list->object);
    if (technique) {
        technique->clearTags();
        emit technique->tagsChanged();
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
