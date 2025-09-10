// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
using ParameterList = QList<QParameter *>;

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
    QList<QFilterKey *> filterKeys() const;

    void addRenderState(QRenderState *state);
    void removeRenderState(QRenderState *state);
    QList<QRenderState *> renderStates() const;

    void addParameter(QParameter *p);
    void removeParameter(QParameter *p);
    QList<QParameter *> parameters() const;

public Q_SLOTS:
    void setShaderProgram(QShaderProgram *shaderProgram);

Q_SIGNALS:
    void shaderProgramChanged(QShaderProgram *shaderProgram);

protected:
    explicit QRenderPass(QRenderPassPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QRenderPass)
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERPASS_H
