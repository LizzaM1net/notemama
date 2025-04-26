#include "tool.h"

#include "canvas.h"

Tool::Tool(Canvas *canvas)
    : m_canvas(canvas) {}

QString Tool::name() {
    return metaObject()->className();
}

QVariantList Tool::properties() {
    const QMetaObject *toolMeta = metaObject();
    if (toolMeta->className() == QStringLiteral("Tool"))
        return {};

    QVariantList properties;
    for (int i = toolMeta->propertyOffset(); i < toolMeta->propertyCount(); i++) {
        QVariantMap property;
        property["type"] = QString(toolMeta->property(i).metaType().name());
        property["name"] = toolMeta->property(i).name();
        properties << property;
    }
    return properties;
}
