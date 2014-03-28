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

#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include <qt3drenderer_global.h>
#include <Qt3DCore/node.h>

#include "tag.h"
#include "renderpass.h"

#include <QList>
#include <QMap>
#include <QQmlListProperty>
#include <QSharedPointer>

// FIXME - move enum somewhere common so don't need to include this here
#include <quniformvalue.h>

namespace Qt3D {

class QT3DRENDERERSHARED_EXPORT Parameter : public QObject
{
    Q_OBJECT

public:
    // FIXME - sort this by frequency, to minimize the size of the
    // vector in RenderShader. (We want to use compact storage, but we index
    // by this enum, and resize to the largest value)
    enum StandardUniform
    {
        None = -1,

        ModelMatrix = 0,
        ViewMatrix,
        ProjectionMatrix,
        ModelView,
        ModelViewProjection,

        ModelInverse,
        ViewInverse,
        ProjectionInverse,
        ModelViewInverse,
        ModelViewProjectionInverse,

        ModelNormal,
        ModelViewNormal
    };

    Parameter(QObject* parent, QString name, int ty);

    QString name() const
    { return m_name; }

    // permit one extra level of indrection in mesh naming of
    // attributes (glTf at least does this)
    void setMeshAttributeName(QString name);
    QString meshAttributeName() const
    { return m_meshName; }

    bool isStandardUniform() const;

    void setStandardUniform(StandardUniform su);
    StandardUniform standardUniform() const;

    /**
     * @brief setDefaultValue - for non-texture uniform parameters
     * @param dv
     */
    void setDefaultValue(QVariant dv);

    // setUniformValue();

    QVariant value() const;

    int datatype() const
    { return m_type; }

    bool isTextureType() const;

    /**
     * @brief uniformType - map the data type to the primitive uniform type
     * @return
     */
    Render::QUniformValue::Type uniformType() const;
signals:
    void valueChanged();

private:
    QString m_name;
    int m_type;
    QVariant m_value;
    QVariant m_defaultValue;
    QString m_meshName;
    StandardUniform m_standardUniform;
};

class QT3DRENDERERSHARED_EXPORT Technique : public Node
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<Qt3D::Tag> tags READ tags NOTIFY tagsChanged)
    Q_PROPERTY(QQmlListProperty<Qt3D::RenderPass> renderPasses READ renderPassList NOTIFY renderPassesChanged)

public:
    explicit Technique(Node *parent = 0);

    QQmlListProperty<Qt3D::Tag> tags();
    void addTag( const QString& name, const QVariant& value );
    void removeTag( const QString& name );
    void setTagValue( const QString& name, const QVariant& value );
    QVariant tagValue( const QString& name ) const;
    bool containsTag( const QString& name ) const;


    void addPass(RenderPass* pass);
    QList<RenderPass *> renderPasses() const;

    QQmlListProperty<Qt3D::RenderPass> renderPassList();



    // QQmlListProperty<Qt3D::Parameter> parameters();

    void addParameter(Parameter* p);
    QList<Parameter *> parameters() const
    { return m_parameters; }

    Parameter* parameterByName(QString name) const;
signals:
    void tagsChanged();
    void renderPassesChanged();

private:
    static void appendTag(QQmlListProperty<Tag> *list, Tag *bar);
    static Tag *tagAt(QQmlListProperty<Tag> *list, int index);
    static int tagCount(QQmlListProperty<Tag> *list);
    static void clearTags(QQmlListProperty<Tag> *list);

    static void appendRenderPass(QQmlListProperty<RenderPass> *list, RenderPass* renderPass);
    static RenderPass *renderPassAt(QQmlListProperty<RenderPass> *list, int index);
    static int renderPassCount(QQmlListProperty<RenderPass> *list);
    static void clearRenderPasses( QQmlListProperty<RenderPass> *list);

    QList<Tag *> m_tagList;
    QMap<QString, Tag *> m_tags;

    QList<Parameter *> m_parameters;

    QList<RenderPass *> m_renderPasses;
};

}

#endif // TECHNIQUE_H
