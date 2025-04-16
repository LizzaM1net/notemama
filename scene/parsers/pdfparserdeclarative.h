#ifndef PDFPARSERDECLARATIVE_H
#define PDFPARSERDECLARATIVE_H

#include <QObject>
#include <QUrl>
#include <qqmlintegration.h>

#include "pdfparser/pdfparser.h"

class Scene;
class VectorPathSceneItem;

class PdfParserDeclarative : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PdfParser)
    Q_PROPERTY(Scene *scene READ scene WRITE setScene NOTIFY sceneChanged FINAL)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged FINAL)

public:
    PdfParserDeclarative();

    Scene *scene() const;
    void setScene(Scene *newScene);

    QString file() const;
    void setFile(const QString &newFile);

    void reparse();

signals:
    void sceneChanged();
    void fileChanged();

private:
    Scene *m_scene = nullptr;
    QVector2D m_lastPoint;
    VectorPathSceneItem *m_currentItem = nullptr;
    pdfparser m_parser;
    QString m_file;
};

#endif // PDFPARSERDECLARATIVE_H
