#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QKeyEvent>
#include <cmath>

// from https://stackoverflow.com/questions/17512547/finding-the-point-of-intersection-between-a-line-and-a-qpainterpath

/*!
    Returns the closest element (position) in \a sourcePath to \a target,
    using \l{QPoint::manhattanLength()} to determine the distances.
*/
QPointF closestPointTo(const QPointF &target, const QPainterPath &sourcePath)
{
    Q_ASSERT(!sourcePath.isEmpty());
    QPointF shortestDistance = sourcePath.elementAt(0) - target;
    qreal shortestLength = shortestDistance.manhattanLength();
    for (int i = 1; i < sourcePath.elementCount(); ++i) {
        const QPointF distance(sourcePath.elementAt(i) - target);
        const qreal length = distance.manhattanLength();
        if (length < shortestLength) {
            shortestDistance = sourcePath.elementAt(i);
            shortestLength = length;
        }
    }
    return shortestDistance;
}

/*!
    Returns \c true if \a projectilePath intersects with any items in \a scene,
    setting \a hitPos to the position of the intersection.
*/
bool hit(const QPainterPath &projectilePath, QGraphicsScene *scene, QPointF &hitPos)
{
    const QList<QGraphicsItem *> itemsInPath = scene->items(projectilePath, Qt::IntersectsItemBoundingRect);
    if (!itemsInPath.isEmpty()) {
        const QPointF projectileStartPos = projectilePath.elementAt(0);
        float shortestDistance = std::numeric_limits<float>::max();
        QGraphicsItem *closest = 0;
        foreach (QGraphicsItem *item, itemsInPath) {
            QPointF distanceAsPoint = item->pos() - projectileStartPos;
            float distance = fabs(distanceAsPoint.x() + distanceAsPoint.y());
            if (distance < shortestDistance) {
                shortestDistance = distance;
                closest = item;
            }
        }

        QPainterPath targetShape = closest->mapToScene(closest->shape());
        // QLineF has normalVector(), which is useful for extending our path to a rectangle.
        // The path needs to be a rectangle, as QPainterPath::intersected() only accounts
        // for intersections between fill areas, which projectilePath doesn't have.
        QLineF pathAsLine(projectileStartPos, projectilePath.elementAt(1));
        // Extend the first point in the path out by 1 pixel.
        QLineF startEdge = pathAsLine.normalVector();
        startEdge.setLength(1);
        // Swap the points in the line so the normal vector is at the other end of the line.
        pathAsLine.setPoints(pathAsLine.p2(), pathAsLine.p1());
        QLineF endEdge = pathAsLine.normalVector();
        // The end point is currently pointing the wrong way; move it to face the same
        // direction as startEdge.
        endEdge.setLength(-1);
        // Now we can create a rectangle from our edges.
        QPainterPath rectPath(startEdge.p1());
        rectPath.lineTo(startEdge.p2());
        rectPath.lineTo(endEdge.p2());
        rectPath.lineTo(endEdge.p1());
        rectPath.lineTo(startEdge.p1());
        // Visualize the rectangle that we created.
        scene->addPath(rectPath, QPen(QBrush(Qt::blue), 2));
        // Visualize the intersection of the rectangle with the item.
        scene->addPath(targetShape.intersected(rectPath), QPen(QBrush(Qt::cyan), 2));
        // The hit position will be the element (point) of the rectangle that is the
        // closest to where the projectile was fired from.
        hitPos = closestPointTo(projectileStartPos, targetShape.intersected(rectPath));

        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGraphicsView view;
    QGraphicsScene *scene = new QGraphicsScene;
    view.setScene(scene);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsItem *target = scene->addRect(0, 0, 25, 25);
    target->setTransformOriginPoint(QPointF(12.5, 12.5));
    target->setRotation(35);
    target->setPos(100, 100);

    QPainterPath projectilePath;
    projectilePath.moveTo(200, 200);
    projectilePath.lineTo(0, 0);
    projectilePath.lineTo(200, 200);

    QPointF hitPos;
    if (hit(projectilePath, scene, hitPos)) {
        scene->addEllipse(hitPos.x() - 2, hitPos.y() - 2, 4, 4, QPen(Qt::red));
    }

    scene->addPath(projectilePath, QPen(Qt::DashLine));
    scene->addText("start")->setPos(180, 150);
    scene->addText("end")->setPos(20, 0);

    view.show();

    return app.exec();
}
