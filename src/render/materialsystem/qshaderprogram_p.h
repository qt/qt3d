// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSHADERPROGRAM_P_H
#define QT3DRENDER_QSHADERPROGRAM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QShaderProgramPrivate : public Qt3DCore::QNodePrivate
{
public:
    QShaderProgramPrivate();

    Q_DECLARE_PUBLIC(QShaderProgram)
    QByteArray m_vertexShaderCode;
    QByteArray m_tessControlShaderCode;
    QByteArray m_tessEvalShaderCode;
    QByteArray m_geometryShaderCode;
    QByteArray m_fragmentShaderCode;
    QByteArray m_computeShaderCode;
    QString m_log;
    QShaderProgram::Status m_status;
    QShaderProgram::Format m_format;

    void setLog(const QString &log);
    void setStatus(QShaderProgram::Status status);

    static QByteArray deincludify(const QByteArray &contents, const QString &filePath);
    static QByteArray deincludify(const QString &filePath);
    static QByteArray resolveAutoBindingIndices(const QByteArray &content,
                                                int &currentBinding,
                                                int &currentInputLocation,
                                                int &currentOutputLocation);
    static QByteArray resolveAutoBindingIndices(const QByteArray &content);
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSHADERPROGRAM_P_H
