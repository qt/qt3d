#ifndef QUICKSAVE_H
#define QUICKSAVE_H

#include <QObject>
#include <QFile>
#include <QUrl>

class QuickSave : public QObject {
    Q_OBJECT

    QString _filename;
    QString _data;

    Q_PROPERTY(QString save READ save)
    Q_PROPERTY(QString filename READ filename WRITE setFilename)
    Q_PROPERTY(QString data READ data WRITE setData)
public:
    QuickSave(QObject *parent=0) : QObject(parent) {}

    QString save() const {
        QFile file(QUrl(_filename + QLatin1String(".qml")).toLocalFile());

        qDebug("Attempting to write: %s", file.fileName().toAscii().constData());

        if (!file.open(QFile::WriteOnly))
            return file.errorString();

        file.write(_data.toUtf8());

        file.close();

        return QString();
    }

    //TODO: these really should be emitting signals upon changing;
    //      but we don't display them so it doesn't technically matter
    QString filename() const { return _filename; }
    void setFilename(const QString filename) { if (_filename != filename) _filename = filename; }

    QString data() const { return _data; }
    void setData(const QString data) { if (_data != data) _data = data; }

};

#endif // QUICKSAVE_H
