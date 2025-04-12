#ifndef VECTORPATHSCENEITEM_H
#define VECTORPATHSCENEITEM_H

#include "sceneitem.h"

#include <QVector2D>
#include <QList>
#include <QColor>

class QRhiBuffer;

struct ColorVector2D {
    ColorVector2D(QVector2D point, float red, float green, float blue)
        : point(point)
        , red(red)
        , green(green)
        , blue(blue) {}

    ColorVector2D(QVector2D point, QColor color)
        : point(point)
        , red(color.redF())
        , green(color.greenF())
        , blue(color.blueF()) {}

    ColorVector2D operator+(QVector2D vec) {
        return ColorVector2D(point+vec, red, green, blue);
    }

    ColorVector2D operator-(QVector2D vec) {
        return ColorVector2D(point-vec, red, green, blue);
    }

    QVector2D point;
    float red, green, blue;
};

namespace VectorPath {
    struct Segment {
        virtual QList<ColorVector2D> generateVertices(QVector2D startPoint) const = 0;
        virtual QVector2D lastPoint(QVector2D startPoint) const = 0;

        virtual QVector2D inTangent() const = 0;
        virtual QVector2D outTangent() const = 0;
    };

    struct LineSegment : public Segment {
        LineSegment(QVector2D relEnd);

        QList<ColorVector2D> generateVertices(QVector2D startPoint) const override;
        QVector2D lastPoint(QVector2D startPoint) const override;

        QVector2D inTangent() const override;
        QVector2D outTangent() const override;

        QVector2D relEnd;
    };

    struct QuadCurveSegment : public Segment {
        QuadCurveSegment(QVector2D relB, QVector2D relC);

        QList<ColorVector2D> generateVertices(QVector2D startPoint) const override;
        QVector2D lastPoint(QVector2D startPoint) const override;

        QVector2D inTangent() const override;
        QVector2D outTangent() const override;

        QVector2D relB, relC;
    };

    struct CubicCurveSegment : public Segment {
        CubicCurveSegment(QVector2D relB, QVector2D relC, QVector2D relD);

        QList<ColorVector2D> generateVertices(QVector2D startPoint) const override;
        QVector2D lastPoint(QVector2D startPoint) const override;

        QVector2D inTangent() const override;
        QVector2D outTangent() const override;

        QVector2D relB, relC, relD;
    };
}

class VectorPathSceneItem : public SceneItem
{
public:
    VectorPathSceneItem(QVector2D startPoint, QList<VectorPath::Segment*> segments);
    ~VectorPathSceneItem();

    void synchronize(QRhi *rhi, QRhiResourceUpdateBatch *updateBatch) override;
    void render(QRhiCommandBuffer *cb) override;

    QRectF boundingRect() override;

    QVector2D startPoint;
    QList<VectorPath::Segment*> segments;

private:
    QList<ColorVector2D> generateVertices();

    QRhiBuffer *m_buffer = nullptr;
    int m_verticesCount = 0;
};

#endif // VECTORPATHSCENEITEM_H
