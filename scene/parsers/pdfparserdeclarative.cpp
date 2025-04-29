#include "pdfparserdeclarative.h"

#include <QDebug>

#include "scene/items/vectorpathsceneitem.h"
#include "scene/scene.h"

PdfParserDeclarative::PdfParserDeclarative() {

}

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

    m_parser.readfiles({"/Users/mac/project/notemama/scene/parsers/pdfparser/test1.pdf"});
    for (const auto& item : m_parser.m_parseritem)
    {
        m_scene->addItem(item);
    }

}
