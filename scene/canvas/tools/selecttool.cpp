#include "selecttool.h"

#include <QJsonArray>

#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
// #include "tensorflow/lite/c/c_api.h"

#include "canvas.h"
#include "items/vectorpathsceneitem.h"

bool pointInRect(QVector2D rectA, QVector2D rectB, QVector2D C) {
    return C.x() >= qMin(rectA.x(), rectB.x())
           && C.x() <= qMax(rectA.x(), rectB.x())
           && C.y() >= qMin(rectA.y(), rectB.y())
           && C.y() <= qMax(rectA.y(), rectB.y());
}

SelectTool::SelectTool(Canvas *canvas)
    : Tool(canvas) {}

void SelectTool::mousePress(QVector2D position) {
    m_firstPoint = position;
    m_secondPoint = position;
    if (m_item) {
        m_canvas->currentScene()->removeItem(m_item);
        delete m_item;
    }
    m_item = new VectorPathSceneItem(position, {new VectorPath::LineSegment(position),
                                                new VectorPath::LineSegment(position),
                                                new VectorPath::LineSegment(position),
                                                new VectorPath::LineSegment(position)}, 1);
    m_canvas->currentScene()->addItem(m_item);
}

void SelectTool::mouseMove(QVector2D position) {
    m_secondPoint = position;
    static_cast<VectorPath::LineSegment*>(m_item->segments[0])->end.setX(position.x());
    static_cast<VectorPath::LineSegment*>(m_item->segments[1])->end = position;
    static_cast<VectorPath::LineSegment*>(m_item->segments[2])->end.setY(position.y());
    m_item->setNeedsSync();
}

QString svgPoint(QVector2D point) {
    QString ret;
    ret += QString::number(point.x()) + ", " + QString::number(point.y()) + " ";
    return ret;
}

void printSvg(QList<VectorPathSceneItem*> items) {
    QString svg;
    svg += "<svg>";
    if (!items.isEmpty()) {
        for (VectorPathSceneItem *item : items) {
            svg += "<path style=\"fill:none;stroke:#000000;stroke-width:1\" d=\"";
            svg += "M " + svgPoint(item->startPoint);
            for (VectorPath::Segment *segment : std::as_const(item->segments)) {
                if (VectorPath::CubicCurveSegment *cubicSegment = dynamic_cast<VectorPath::CubicCurveSegment*>(segment)) {
                    svg += "C " + svgPoint(cubicSegment->b) + svgPoint(cubicSegment->c) + svgPoint(cubicSegment->d);
                }
            }
            svg += "\" />";
        }
    }
    svg += "</svg>";
    qDebug() << qPrintable(svg);
}

QJsonArray extractBezierFeatures(const QVector2D& p0, const QVector2D& p1, const QVector2D& p2, const QVector2D& p3, double startLength, bool penLift) {
    QVector2D mainVector = p3 - p0;
    double mainAngle = std::atan2(mainVector.y(), mainVector.x());
    double mainLength = std::hypot(mainVector.x(), mainVector.y());

    auto relPoint = [](const QVector2D& from, const QVector2D& to) {
        return QVector2D(to.x() - from.x(), to.y() - from.y());
    };

    auto length = [](const QVector2D& v) {
        return std::hypot(v.x(), v.y());
    };

    auto angleToMain = [mainAngle](const QVector2D& v) {
        return std::atan2(v.y(), v.x()) - mainAngle;
    };

    QVector2D handle1 = relPoint(p0, p1);
    QVector2D handle2 = relPoint(p3, p2); // обратное направление

    QJsonArray features;
    features << mainVector.x()/startLength;
    features << mainVector.y()/startLength;
    features << length(handle1) / mainLength;
    features << length(handle2) / mainLength;
    features << angleToMain(handle1);
    features << angleToMain(handle2);
    features << double(penLift);

    return features;
}

std::vector<char> loadModel(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Не удалось открыть файл модели");

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) throw std::runtime_error("Ошибка чтения модели");
    return buffer;
}


// int recognizeText(const QJsonArray &features) {
//     std::vector<char> model_data = loadModel("model.tflite");

//     TfLiteModel* model = TfLiteModelCreate(model_data.data(), model_data.size());
//     if (!model)
//         return 1;

//     TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
//     TfLiteInterpreterOptionsSetNumThreads(options, 1);

//     TfLiteInterpreter* interpreter = TfLiteInterpreterCreate(model, options);
//     TfLiteInterpreterOptionsDelete(options);

//     if (!interpreter) {
//         TfLiteModelDelete(model);
//         return 1;
//     }

//     if (TfLiteInterpreterAllocateTensors(interpreter) != kTfLiteOk) {
//         TfLiteInterpreterDelete(interpreter);
//         TfLiteModelDelete(model);
//         return 1;
//     }

//     int input_index = 0;
//     TfLiteTensor* input_tensor = TfLiteInterpreterGetInputTensor(interpreter, input_index);

//     if (TfLiteTensorType(input_tensor) != kTfLiteFloat32)
//         return 1;

//     std::vector<float> input_data(1 * features.size() * 7);
//     for (int t = 0; t < features.size(); ++t) {
//         for (int j = 0; j < 7; j++)
//             input_data[t * 7 + j] = features[t][j].toInt();
//     }

//     if (TfLiteTensorCopyFromBuffer(input_tensor, &input_data, sizeof(input_data)) != kTfLiteOk)
//         return 1;

//     if (TfLiteInterpreterInvoke(interpreter) != kTfLiteOk)
//         return 1;

//     const TfLiteTensor* output_tensor = TfLiteInterpreterGetOutputTensor(interpreter, 0);
//     float output_data[42];
//     if (TfLiteTensorCopyToBuffer(output_tensor, output_data, sizeof(output_data)) != kTfLiteOk)
//         return 1;

//     std::cout << "Result: " << output_data[0] << std::endl;

//     TfLiteInterpreterDelete(interpreter);
//     TfLiteModelDelete(model);

//     return 0;
// }

void exportData(QList<VectorPathSceneItem*> items) {
    QJsonArray features;
    for (VectorPathSceneItem *item : items) {
        QVector2D startPoint = item->startPoint;
        double startLength = (item->segments.first()->lastPoint()-startPoint).length();
        for (VectorPath::Segment *segment : std::as_const(item->segments)) {
            if (VectorPath::CubicCurveSegment *cubicSegment = dynamic_cast<VectorPath::CubicCurveSegment*>(segment)) {
                features += extractBezierFeatures(startPoint, cubicSegment->b, cubicSegment->c, cubicSegment->d, startLength, segment == item->segments.last());
                startPoint = segment->lastPoint();
            }
        }
    }
    qDebug() << features;

    // recognizeText(features);
}

void SelectTool::mouseRelease() {
    QList<VectorPathSceneItem*> selectedItems;
    QRectF rect = QRectF(m_firstPoint.toPointF(), m_secondPoint.toPointF()).normalized();
    QRectF tightenRect;
    for (SceneItem *item : std::as_const(m_canvas->currentScene()->items)) {
        if (item == m_item)
            continue;

        if (VectorPathSceneItem *pathItem = dynamic_cast<VectorPathSceneItem*>(item)) {
            if (rect.contains(item->boundingRect())) {
                selectedItems << pathItem;
                tightenRect = tightenRect.united(item->boundingRect());
            }
        }
    }
    m_item->startPoint = QVector2D(tightenRect.topLeft());
    static_cast<VectorPath::LineSegment*>(m_item->segments[0])->end = QVector2D(tightenRect.topRight());
    static_cast<VectorPath::LineSegment*>(m_item->segments[1])->end = QVector2D(tightenRect.bottomRight());
    static_cast<VectorPath::LineSegment*>(m_item->segments[2])->end = QVector2D(tightenRect.bottomLeft());
    static_cast<VectorPath::LineSegment*>(m_item->segments[3])->end = QVector2D(tightenRect.topLeft());
    m_item->setNeedsSync();

    exportData(selectedItems);
}

void SelectTool::toolDeactivated() {
    if (m_item) {
        m_canvas->currentScene()->removeItem(m_item);
        delete m_item;
        m_item = nullptr;
    }
}
