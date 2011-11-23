#ifndef QUICKSAVE_H
#define QUICKSAVE_H

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QFileDialog>
#include <QDebug>

class QuickSave : public QObject
{
    Q_OBJECT

    QString _filename;
    QString _data;

    Q_PROPERTY(QString filename READ filename WRITE setFilename)
    Q_PROPERTY(QString data READ data WRITE setData)

public:
    QuickSave(QObject *parent=0) : QObject(parent) {}

    Q_INVOKABLE QString save() const
    {
        QString filename = QFileDialog::getSaveFileName(0, tr("Save File"), "",tr("Files (*.qml)"));

        // FIXME: ensure fileName is appropriate as a qml Component
        if (!filename.endsWith(".qml"))
            filename.append(".qml");

        QString modelFilename = QUrl(_filename).toLocalFile();

        QDir outputDir = QFileInfo(filename).absoluteDir();
        QString relativeFilename = outputDir.relativeFilePath(modelFilename);

        QFile file(filename);

        qDebug("Attempting to write: %s", file.fileName().toAscii().constData());

        if (!file.open(QFile::WriteOnly))
            return file.errorString();

        QString dataToWrite = _data.arg(relativeFilename);

        file.write(dataToWrite.toUtf8());

        file.close();

        return QString();
    }

    QString filename() const { return _filename; }
    void setFilename(const QString filename) { if (_filename != filename) _filename = filename; emit filenameChanged(filename); }

    QString data() const { return _data; }
    void setData(const QString data) { if (_data != data) _data = data; emit dataChanged(data); }

signals:
    void filenameChanged(const QString newFilename);
    void dataChanged(const QString newData);
};

#endif // QUICKSAVE_H
