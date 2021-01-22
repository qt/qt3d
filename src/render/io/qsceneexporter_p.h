/****************************************************************************
**
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

#ifndef QT3DRENDER_QSCENE_EXPORTER_P_H
#define QT3DRENDER_QSCENE_EXPORTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qt3drender_global_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qloggingcategory.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {

Q_DECLARE_LOGGING_CATEGORY(SceneParsers)

class Q_3DRENDERSHARED_PRIVATE_EXPORT QSceneExporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList errors READ errors NOTIFY errorsChanged)

public:
    QSceneExporter();
    virtual ~QSceneExporter();

    virtual bool exportScene(Qt3DCore::QEntity *sceneRoot, const QString &outDir,
                             const QString &exportName, const QVariantHash &options) = 0;
    QStringList errors() const;

Q_SIGNALS:
    void errorsChanged(const QStringList &errors);

protected:
    void logError(const QString &error);
    void logInfo(const QString &info);

private:
    QStringList m_errors;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSCENE_EXPORTER_P_H
