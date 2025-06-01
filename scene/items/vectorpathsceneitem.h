#ifndef VECTORPATHSCENEITEM_H
#define VECTORPATHSCENEITEM_H

#include "sceneitem.h"

#include <QVector2D>
#include <QList>
#include <QColor>

class QRhiBuffer;

struct ColorVector2D {
    ColorVector2D(QVector2D point, float red, float green, float blue, float alpha = 1)
        : point(point)
        , red(red)
        , green(green)
        , blue(blue)
        , alpha(alpha) {}

    ColorVector2D(QVector2D point, QColor color)
        : point(point)
        , red(color.redF())
        , green(color.greenF())
        , blue(color.blueF())
        , alpha(color.alphaF()){}

    ColorVector2D operator+(QVector2D vec) {
        return ColorVector2D(point+vec, red, green, blue);
    }

    ColorVector2D operator-(QVector2D vec) {
        return ColorVector2D(point-vec, red, green, blue);
    }

    QVector2D point;
    float red, green, blue, alpha;
};

namespace VectorPath {
    struct Segment {
        Segment();
        virtual ~Segment();

        virtual QList<ColorVector2D> generateVertices(QVector2D startPoint, float lineWidth) const = 0;
        virtual QVector2D lastPoint() const = 0;

        virtual QVector2D inTangent(QVector2D startPoint) const = 0;
        virtual QVector2D outTangent(QVector2D startPoint) const = 0;
    };

    struct LineSegment : public Segment {
        LineSegment(QVector2D end);

        QList<ColorVector2D> generateVertices(QVector2D startPoint, float lineWidth) const override;
        QVector2D lastPoint() const override;

        QVector2D inTangent(QVector2D startPoint) const override;
        QVector2D outTangent(QVector2D startPoint) const override;

        QVector2D end;
    };

    struct QuadCurveSegment : public Segment {
        QuadCurveSegment(QVector2D b, QVector2D c);

        QList<ColorVector2D> generateVertices(QVector2D startPoint, float lineWidth) const override;
        QVector2D lastPoint() const override;

        QVector2D inTangent(QVector2D startPoint) const override;
        QVector2D outTangent(QVector2D startPoint) const override;

        QVector2D b, c;
    };

    struct CubicCurveSegment : public Segment {
        CubicCurveSegment(QVector2D b, QVector2D c, QVector2D d);

        QList<ColorVector2D> generateVertices(QVector2D startPoint, float lineWidth) const override;
        QVector2D lastPoint() const override;

        QVector2D inTangent(QVector2D startPoint) const override;
        QVector2D outTangent(QVector2D startPoint) const override;

        QVector2D b, c, d;
    };
}

class VectorPathSceneItem : public SceneItem
{
    friend class CurvePenTool;
public:
    VectorPathSceneItem(QVector2D startPoint, QList<VectorPath::Segment*> segments, float lineWidth = 1);
    ~VectorPathSceneItem();

    void synchronize(QRhi *rhi, QRhiResourceUpdateBatch *updateBatch) override;
    void render(QRhiCommandBuffer *cb) override;

    QRectF boundingRect() override;

    QVector2D startPoint;
    QList<VectorPath::Segment*> segments;

    float lineWidth;

private:
    QList<ColorVector2D> generateVertices();

    QRhiBuffer *m_buffer = nullptr;
    QRectF m_boundingRect;
    int m_verticesCount = 0;
};

#endif // VECTORPATHSCENEITEM_H
