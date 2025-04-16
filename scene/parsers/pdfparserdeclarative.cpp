#include "pdfparserdeclarative.h"

#include <QDebug>

#include "scene/items/vectorpathsceneitem.h"
#include "scene/scene.h"

PdfParserDeclarative::PdfParserDeclarative() {}

Scene *PdfParserDeclarative::scene() const {
    return m_scene;
}

void PdfParserDeclarative::setScene(Scene *newScene) {
    if (m_scene == newScene)
        return;

    m_scene = newScene;
    emit sceneChanged();
    reparse();
}

QString PdfParserDeclarative::file() const {
    return m_file;
}

void PdfParserDeclarative::setFile(const QString &newFile) {
    if (m_file == newFile || !QFile::exists(newFile))
        return;

    m_file = newFile;
    emit fileChanged();
    reparse();
}

void PdfParserDeclarative::reparse() {
    if (m_file.isEmpty() || m_scene == nullptr)
        return;

    m_parser.readfiles({m_file.toStdString()});
    for (DrawingType *type : std::as_const(m_parser.drawingcommands)) {
        qDebug() << (char)type->getType();
        switch (type->getType()) {
        case MoveTo: {
            Move *move = static_cast<Move*>(type);
            m_currentItem = new VectorPathSceneItem(move->m_point, {});
            m_scene->addItem(m_currentItem);
            m_lastPoint = move->m_point;
        }
        case LineTo: {
            Line *line = static_cast<Line*>(type);
            m_currentItem->segments << new VectorPath::LineSegment(line->m_point - m_lastPoint);
            m_lastPoint = line->m_point;
        }
        case QuitTo: {
            // m_currentItem = nullptr;
        }
        default: {

        }
        }
    }
}
