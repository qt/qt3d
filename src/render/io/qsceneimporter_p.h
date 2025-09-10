// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSCENE_IMPORTER_P_H
#define QT3DRENDER_QSCENE_IMPORTER_P_H

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

#include <QObject>
#include <QStringList>
#include <QLoggingCategory>
#include <QUrl>
#include <private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {

Q_DECLARE_LOGGING_CATEGORY(SceneParsers)

class Q_3DRENDERSHARED_PRIVATE_EXPORT QSceneImporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ParserStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QStringList errors READ errors NOTIFY errorsChanged)

public:
    enum ParserStatus {
        Empty,
        Loading,
        Loaded,
        Error
    };
    Q_ENUM(ParserStatus) // LCOV_EXCL_LINE

    QSceneImporter();
    virtual ~QSceneImporter();

    virtual void setSource(const QUrl &source) = 0;
    virtual void setData(const QByteArray& data, const QString &basePath) = 0;
    virtual bool areFileTypesSupported(const QStringList &extensions) const = 0;
    virtual Qt3DCore::QEntity *scene(const QString &id = QString()) = 0;
    virtual Qt3DCore::QEntity *node(const QString &id) = 0;

    ParserStatus status() const;
    QStringList errors() const;

Q_SIGNALS:
    void statusChanged(ParserStatus status);
    void errorsChanged(const QStringList &errors);

protected:
    void setStatus(ParserStatus status);
    void logError(const QString &error);
    void logInfo(const QString &info);

private:
    ParserStatus m_status;
    QStringList m_errors;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSCENE_IMPORTER_P_H
