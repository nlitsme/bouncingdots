/*
   c++ variant of rotating_dots.py

   press '1'  to start.
 */
#include <random>

#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QKeyEvent>

const double PI = 3.14159265358979323846264338328;

double randomfloat(double low, double high)
{
    static std::random_device rd;
    std::uniform_real_distribution<> dist(low, high);

    return dist(rd);
}

class Dot : public QGraphicsItem
{
public:
    QColor color;

    double angle= randomfloat(0, 2*PI);
    double speed= randomfloat(1.0, 2.0);
    double mass= randomfloat(1.0, 2.0);
    double radius= randomfloat(2.0, 8.0);
    double angular= randomfloat(0.0, 10.0);

private:
    QPointF _wall;    // velocity transferred to wall
    QPointF _newpos;
    QRectF _boundingRect;
    //QPainterPath _boundingShape;
public:
    Dot(const QColor& color)
        : color(color)
    {
        calcBounding();
    }

    void calcBounding()
    {
        _boundingRect= QRectF(-radius-0.5, -radius-0.5, 2*radius+0.5, 2*radius+0.5);
        //_boundingShape = QPainterPath();
        //_boundingShape.addEllipse(QPointF(), radius, radius);
    }
    QRectF boundingRect() const { return _boundingRect; }
    QPainterPath shape() const  
    {
        // bar shaped, so we detect all colisions 
        QPointF  end= speedvector();
        QPointF  side= QPointF(sin(angle), -cos(angle))*radius;
        QPainterPath shape;
        shape.setFillRule(Qt::WindingFill);
        shape.addEllipse(QPointF(), radius, radius);

        QPolygonF poly;
        poly << side << end+side << end-side << -side;
        shape.addPolygon(poly);
        shape.closeSubpath();

        shape.addEllipse(speedvector(), radius, radius);
        return shape;
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        painter->setBrush(color);
        painter->drawEllipse(QPointF(), radius, radius);
        painter->drawLine(0,0, speed*cos(angle), speed*sin(angle));
    }

    double distance(const Dot& item)
    {
        auto d = pos() - item.pos();
        return sqrt(d.x()*d.x() + d.y()*d.y());
    }

    void calcNewpos()
    {
        for (auto item : collidingItems())
        {
            auto dot = dynamic_cast<Dot*>(item);
            if (dot)
                collide(dot);
        }

        auto newpos = mapToParent(speed*cos(angle), speed*sin(angle));

        handle_wall(newpos);

        _newpos = newpos;
    }
    void handle_wall(QPointF& pos)
    {
        auto sr = scene()->sceneRect();
        bool verticalbounce= pos.x() <= sr.left() || pos.x() >= sr.right();
        bool horizontalbounce= pos.y() <= sr.top() || pos.y() >= sr.bottom();
        if (verticalbounce) {
            auto v0= speedvector();
            angle = PI - angle;
            auto v1= speedvector();
            _wall += QPointF(-2.0*speed*cos(angle), 0.0);
        }
        else if (horizontalbounce) {
            auto v0= speedvector();
            angle = -angle;
            auto v1= speedvector();
            _wall += QPointF(0.0, -2.0*speed*sin(angle));
        }
        if (verticalbounce || horizontalbounce)
            pos = mapToParent(speed*cos(angle), speed*sin(angle));

        pos.setX(std::min(std::max(pos.x(), sr.left()), sr.right()));
        pos.setY(std::min(std::max(pos.y(), sr.top()), sr.bottom()));
    }
    void updatePos()
    {
        setPos(_newpos);
    }

    void collide(Dot* other)
    {
        Dot& a= *this;
        Dot& b= *other;


        /*
         *  energy:   ma*va0^2 + mb*vb0^2 = ma*va1^2 + mb*vb1^2
         *  momentum: ma*va0*dir(aa0)   + mb*vb0*dir(ab0)   = ma*va1*dir(aa1)   + mb*vb1*dir(ab1)
         *
         *  solve for va1, vb1, aa1, ab1  ... hmm 3 equations, 4 variables.
         *
         */
        auto diff = a.pos() - b.pos();
        double phi = atan2(diff.y(), diff.x());

        QPointF newva(a.speed*cos(a.angle-phi), a.speed*sin(a.angle-phi));
        QPointF newvb(b.speed*cos(b.angle-phi), b.speed*sin(b.angle-phi));

        QPointF va( ((a.mass-b.mass)*newva.x() + (a.mass+b.mass)*newvb.x())/(a.mass+b.mass),  newva.y() );
        QPointF vb( ((b.mass-a.mass)*newvb.x() + (a.mass+b.mass)*newva.x())/(a.mass+b.mass),  newvb.y() );

        a.setspeed(cos(phi)*va.x()+cos(phi+PI/2.0)*va.y(),  sin(phi)*va.x()+sin(phi+PI/2.0)*va.y());
        b.setspeed(cos(phi)*vb.x()+cos(phi+PI/2.0)*vb.y(),  sin(phi)*vb.x()+sin(phi+PI/2.0)*vb.y());
    }

    void setspeed(double x, double y)
    {
        angle= atan2(y,x);
        speed= sqrt(x*x+y*y);
    }

    double energy() const
    {
        return mass*speed*speed/2.0;
    }
    QPointF speedvector() const
    {
        return QPointF(speed*cos(angle), speed*sin(angle));
    }
    QPointF momentum() const
    {
        return mass*speedvector();
    }
    QPointF wallmomentum() const
    {
        return mass*_wall;
    }
    double normalizedangle() const
    {
        double a= angle - 2*PI*int(angle/PI/2);
        if (a<0)
            a += 2*PI;
        return a;
    }
};

class StaticArrow : public QGraphicsItem
{
public:
    QPointF pointa;
    QPointF pointb;
    QRectF _boundingRect;
public:
    StaticArrow()
    {
    }

    QRectF boundingRect() const { return _boundingRect; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        painter->setBrush(Qt::black);
        painter->drawLine(QPointF(), 4.0*(pointa+pointb));
        painter->setBrush(Qt::red);
        painter->drawLine(QPointF(), 4.0*pointa);
        painter->setBrush(Qt::blue);
        painter->drawLine(QPointF(), 4.0*pointb);
    }
};

class Histogram : public QGraphicsItem
{
public:
    std::map<int,int> _data;
    QRectF _boundingRect;
    QColor _color;
    double _barwidth;
public:
    Histogram(QColor color, double width)
        : _color(color), _barwidth(width)
    {
    }

    QRectF boundingRect() const { return _boundingRect; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        QPointF w(_barwidth, 0);
        for (auto &kv : _data)
            painter->fillRect(QRectF(kv.first*w, (kv.first+1)*w+QPointF(0, kv.second*5)), _color);
    }

    void updateStats(const std::map<int,int>& stats)
    {
        _data= stats;
        update();
    }
};



class DotView : public QGraphicsView {
public:
    QColor colors[3]= { Qt::red, Qt::blue, Qt::green };
    int colorcycle= 0;
    QColor colorfilter= Qt::red;
    int paramfilter= 0;

    int timerId= 0;
    QGraphicsTextItem *_txt;
    StaticArrow *_arrow;
    Histogram *_speedhisto;
    Histogram *_anglehisto;

    DotView()
    {
        QGraphicsScene *scene= new QGraphicsScene();
        scene->setSceneRect(-600, -600, 1200, 900);
        scene->setItemIndexMethod(QGraphicsScene::NoIndex);

        timerId = startTimer(1000/25);

        setScene(scene);

        addLabels();

        setRenderHint(QPainter::Antialiasing);
        setCacheMode(QGraphicsView::CacheBackground);
        setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        //setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        //setResizeAnchor(QGraphicsView::AnchorViewCenter);
        //setDragMode(QGraphicsView::ScrollHandDrag);
        setWindowTitle("Dots");
        resize(1250, 950);
        show();
    }

    void addLabels()
    {
        _txt = new QGraphicsTextItem();
        scene()->addItem(_txt);

        _arrow = new StaticArrow();
        scene()->addItem(_arrow);

        _speedhisto = new Histogram(Qt::red, 5.0);
        _speedhisto->setPos(-600, -600);
        scene()->addItem(_speedhisto);

        _anglehisto = new Histogram(Qt::black, 5.0);
        _anglehisto->setPos(-600, -400);
        scene()->addItem(_anglehisto);
    }

    void startCircle()
    {
        colorcycle ++;

        int total= 48;

        for (int i=0 ; i<total ; i++)
        {
            Dot *dot= new Dot(colors[colorcycle%3]);
            dot->setPos(200.0 * cos( i * 2.0 * PI / total ), 200.0 * sin( i * 2.0 * PI / total ));
            scene()->addItem(dot);
        }
    }
    template<typename T>
    void removeItems()
    {
        for (auto item : scene()->items()) {
            if (dynamic_cast<T*>(item))
                scene()->removeItem(item);
        }
    }

    void keyPressEvent(QKeyEvent *ev)
    {
        double delta = 1.0;
        if (ev->modifiers() & Qt::ShiftModifier)
            delta = 10.0;
        else if (ev->modifiers() & Qt::AltModifier)
            delta = 20.0;

        switch(ev->key())
        {
            case Qt::Key_1: startCircle(); break;
            case Qt::Key_2: 
                            removeItems<Dot>();
                            removeItems<QGraphicsPathItem>();
                            break;
            case Qt::Key_3: scene()->addItem(new Dot(Qt::yellow)); break;
            case Qt::Key_4: {
                                Dot *a= new Dot(Qt::red);
                                Dot *b= new Dot(Qt::green);

                                a->setPos(-200, -200);
                                a->radius= 5.0;
                                a->angle= PI/4;
                                a->speed= 4.0;
                                a->mass= 1.0;

                                b->setPos(200, -200);
                                b->radius= 5.0;
                                b->angle= 3*PI/4;
                                b->speed= 4.0;
                                b->mass= 1.0;

                                scene()->addItem(a);
                                scene()->addItem(b);
                                
                            }
                            break;
            case Qt::Key_R: colorfilter = Qt::red; break;
            case Qt::Key_G: colorfilter = Qt::green; break;
            case Qt::Key_B: colorfilter = Qt::blue; break;

            case Qt::Key_M: paramfilter = 0; break;
            case Qt::Key_D: paramfilter = 1; break;
            case Qt::Key_Plus:
            case Qt::Key_Equal:
                            modifyItems(delta);
                            break;
            case Qt::Key_Minus:
            case Qt::Key_Underscore:
                            modifyItems(-delta);
                            break;
        }
    }

    void modifyItems(double delta)
    {
        for (auto item : scene()->items()) {
            auto dot = dynamic_cast<Dot*>(item);
            if (dot) {
                if (dot->color == colorfilter) {
                    switch(paramfilter)
                    {
                        case 0: dot->mass += delta; break;
                        case 1: dot->radius += delta; break;
                    }
                }
            }
        }
    }
    void timerEvent(QTimerEvent *ev)
    {
        for (auto item : scene()->items()) {
            auto dot = dynamic_cast<Dot*>(item);
            if (dot)
                dot->calcNewpos();
        }
        for (auto item : scene()->items()) {
            auto dot = dynamic_cast<Dot*>(item);
            if (dot)
                dot->updatePos();
        }
        QPointF ballmomentum;
        QPointF wallmomentum;
        double totalenergy;
        std::map<int, int> speeds;
        std::map<int, int> angles;
        for (auto item : scene()->items()) {
            auto dot = dynamic_cast<Dot*>(item);
            if (dot) {
                ballmomentum += dot->momentum();
                wallmomentum += dot->wallmomentum();
                totalenergy += dot->energy();

                speeds[round(dot->speed*10)]++;
                angles[round(dot->normalizedangle()*48)]++;
            }
        }
        _arrow->pointa = ballmomentum;
        _arrow->pointb = wallmomentum;
        auto totalmomentum = ballmomentum + wallmomentum;
        _txt->setPlainText(QString("momentum: %1,%2  energy: %3").arg(totalmomentum.x()).arg(totalmomentum.y()).arg(totalenergy));

        _speedhisto->updateStats(speeds);
        _anglehisto->updateStats(angles);
    }
};
void print_help()
{
	qInfo("press these keys:\n");
    qInfo("  1 - start circle\n");
    qInfo("  2 - clear scene\n");
	qInfo("  3 - add yellow dot\n");
	qInfo("  4 - add red+green dots\n");
    qInfo("  r,g,b - select dots by color: red,green,blue\n");
    qInfo("  m - select mass\n");
    qInfo("  d - select radius\n");
    qInfo("  +.-  - modify selected param for selected dots\n");

}
int main(int argc, char *argv[])
{
    qsrand(time(NULL));
    QApplication a(argc, argv);

	print_help();

    DotView w;
    w.show();
    w.raise();

    return a.exec();
}
