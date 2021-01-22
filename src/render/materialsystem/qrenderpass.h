/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
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
****************************************************************************/

#ifndef QT3DRENDER_QRENDERPASS_H
#define QT3DRENDER_QRENDERPASS_H

#include <Qt3DRender/qt3drender_global.h>

#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/qfilterkey.h>

#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QParameter;
class QRenderState;
typedef QVector<QParameter*> ParameterList;

class QRenderPassPrivate;

class Q_3DRENDERSHARED_EXPORT QRenderPass : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QShaderProgram *shaderProgram READ shaderProgram WRITE setShaderProgram NOTIFY shaderProgramChanged)

public:
    explicit QRenderPass(Qt3DCore::QNode *parent = nullptr);
    ~QRenderPass();

    QShaderProgram *shaderProgram() const;

    void addFilterKey(QFilterKey *filterKey);
    void removeFilterKey(QFilterKey *filterKey);
    QVector<QFilterKey*> filterKeys() const;

    void addRenderState(QRenderState *state);
    void removeRenderState(QRenderState *state);
    QVector<QRenderState *> renderStates() const;

    void addParameter(QParameter *p);
    void removeParameter(QParameter *p);
    QVector<QParameter *> parameters() const;

public Q_SLOTS:
    void setShaderProgram(QShaderProgram *shaderProgram);

Q_SIGNALS:
    void shaderProgramChanged(QShaderProgram *shaderProgram);

protected:
    explicit QRenderPass(QRenderPassPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QRenderPass)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERPASS_H
