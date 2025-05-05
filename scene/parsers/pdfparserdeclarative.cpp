#include "pdfparserdeclarative.h"

#include <QDebug>
#include <QFile>


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

    m_parser.decompress_refs();
    m_parser.readfiles({"/Users/mac/pr/notemama/scene/parsers/pdfparser2/src/test3.pdf"});

    qDebug() << m_parser.items.size();

    for (const auto& item : m_parser.items)
    {
        m_scene->addItem(item);
    }

}
