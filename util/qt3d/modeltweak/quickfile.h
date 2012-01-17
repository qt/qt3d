#ifndef QUICKFILE_H
#define QUICKFILE_H

#include <QObject>

class QuickFile : public QObject
{
    Q_OBJECT

    QString _filename;
    QString _data;

    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString data READ data WRITE setData NOTIFY dataChanged)

public:
    explicit QuickFile(QObject *parent = 0);

    QString filename() const;
    void setFilename(const QString filename);

    QString data() const;
    void setData(const QString data);

    Q_INVOKABLE QString save() const;
    Q_INVOKABLE void load();
    Q_INVOKABLE QString promptSave() const;

signals:
    void filenameChanged(const QString newFilename);
    void dataChanged(const QString newData);
};

#endif // QUICKFILE_H
