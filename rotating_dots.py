import math
import random
from PyQt5 import QtCore, QtGui, QtWidgets

class Dot(QtWidgets.QGraphicsItem):
    CenterPoint = QtCore.QPointF(0.0,0.0)
    def __init__(self):
        super(Dot, self).__init__()

        self.angle = random.random()*math.pi*2.0
        self.speed = 1.0 + random.random()*10
        self.mass  = 1.0 + random.random()
        self.radius= 5.0 + random.random()
        self.angular= random.random()*math.pi*2.0

        self.color = QtCore.Qt.green

        self.newPos = None

        self.calcBounding()

    def calcBounding(self):
        adjust = 0.5
        self.BoundingRect = QtCore.QRectF(-self.radius - adjust, -self.radius - adjust, 2*self.radius + adjust, 2*self.radius + adjust)
        self.BoundingShape = QtGui.QPainterPath()
        self.BoundingShape.addEllipse(Dot.CenterPoint, self.radius, self.radius)

    def boundingRect(self):
        return self.BoundingRect

    def shape(self):
        return self.BoundingShape

    def paint(self, painter, option, widget):
        painter.setBrush(self.color)
        painter.drawEllipse(Dot.CenterPoint, self.radius, self.radius)

        painter.drawLine(QtCore.QPointF(0,0), QtCore.QPointF(self.speed*math.cos(self.angle), self.speed*math.sin(self.angle)))

    def distance(self, item):
        d= self.pos()-item.pos()
        return math.sqrt(d.x()**2 + d.y()**2)


    # todo:
    #   more correct simulation when for each particle the next step ( t = t + dt )
    #   is calculated simultaneously.
    #   also for each particle any intersecting path is calculated, and the exact moment of collision
    #   is used as reference point.
    def calcNewpos(self):
        for other in self.collidingItems():
            self.collide(other)

        sceneRect = self.scene().sceneRect()
        
        newpos = self.mapToParent(self.speed*math.cos(self.angle), self.speed*math.sin(self.angle))
        border= 0

        if newpos.x() <= sceneRect.left():
            border |= 1
        elif newpos.x() >= sceneRect.right():
            border |= 2
        elif newpos.y() <= sceneRect.top():
            border |= 4
        elif newpos.y() >= sceneRect.bottom():
            border |= 8

        if border & 3:
            # top/bottom bounce -> PI - angle
            self.angle= math.pi - self.angle
        elif border & 12:
            # left/right bounce -> negate angle
            self.angle=  - self.angle

        if border:
            newpos = self.mapToParent(self.speed*math.cos(self.angle), self.speed*math.sin(self.angle))

        newpos.setX(min(max(newpos.x(), sceneRect.left()), sceneRect.right()))
        newpos.setY(min(max(newpos.y(), sceneRect.top()), sceneRect.bottom()))

        self.newPos= newpos

    def updatePos(self):
        self.setPos(self.newPos)
        self.newPos= None

    def collide(a, b):
        #   m = mass
        #   I = moment of inertia : integral(r^2 * dm, over entire mass)
        #         ring:  m*r^2,   disk: 1/4*m*r^2
        # conserve linear momentum:
        #   momentum = m*v
        # conserve angular momentum:
        #   angular_momentum = I*v
        # conserve energy:
        #   kinetic_energy = 1/2 * m * norm(v)^2
        #   rotational_energy = 1/2 * I * norm(v)^2
        #   sum(kinetic + rotational)[t] == sum(kinetic + rotational)[t+dt]
        
        
        diff = a.pos() - b.pos()
        phi = math.atan2(diff.y(), diff.x())
        
        v1x = ( a.speed*math.cos(a.angle-phi)*(a.mass-b.mass)+2.0*b.mass*b.speed*math.cos(b.angle-phi) ) / (a.mass+b.mass) * math.cos(phi) \
                + a.speed*math.sin(a.angle-phi)*math.cos(phi+math.pi/2.0)

        v1y = ( a.speed*math.cos(a.angle-phi)*(a.mass-b.mass)+2.0*b.mass*b.speed*math.cos(b.angle-phi) ) / (a.mass+b.mass) * math.sin(phi) \
                + a.speed*math.sin(a.angle-phi)*math.cos(phi+math.pi/2.0)

        v2x = ( b.speed*math.cos(b.angle-phi)*(b.mass-a.mass)+2.0*a.mass*a.speed*math.cos(a.angle-phi) ) / (b.mass+a.mass) * math.cos(phi) \
                + b.speed*math.sin(b.angle-phi)*math.cos(phi+math.pi/2.0)

        v2y = ( b.speed*math.cos(b.angle-phi)*(b.mass-a.mass)+2.0*a.mass*a.speed*math.cos(a.angle-phi) ) / (b.mass+a.mass) * math.sin(phi) \
                + b.speed*math.sin(b.angle-phi)*math.cos(phi+math.pi/2.0)

        a.speed = math.sqrt(v1x**2+v1y**2)
        a.angle = math.atan2(v1y, v1x)

        b.speed = math.sqrt(v2x**2+v2y**2)
        b.angle = math.atan2(v2y, v2x)

        angular = ( a.mass * a.radius * a.angular + b.mass * b.radius * b.angular ) / (a.mass * a.radius + b.mass * b.radius)
        a.angular = angular * a.mass * a.radius
        b.angular = angular * b.mass * b.radius



class GraphWidget(QtWidgets.QGraphicsView):
    def __init__(self):
        QtWidgets.QGraphicsView.__init__(self)

        self.colors= [QtCore.Qt.red, QtCore.Qt.blue, QtCore.Qt.green]
        self.colorcycle= 0

        self.paramfilter = None
        self.colorfilter = None

        scene = QtWidgets.QGraphicsScene(self)
        scene.setSceneRect(-600, -600, 1200, 900)
        scene.setItemIndexMethod(QtWidgets.QGraphicsScene.NoIndex)

        self.timerId = self.startTimer(1000 // 25)

        self.setScene(scene)
        self.setRenderHint(QtGui.QPainter.Antialiasing)
        self.setCacheMode(QtWidgets.QGraphicsView.CacheBackground)
        self.setViewportUpdateMode(QtWidgets.QGraphicsView.BoundingRectViewportUpdate)
        #self.setTransformationAnchor(QtWidgets.QGraphicsView.AnchorUnderMouse)
        #self.setResizeAnchor(QtWidgets.QGraphicsView.AnchorViewCenter)
        #self.setDragMode(QtWidgets.QGraphicsView.ScrollHandDrag)
        self.setWindowTitle("Dots")
        self.resize(1250, 950)
        self.show()

    def startCircle(self):
        self.colorcycle += 1
        dotCount = 48
        scene = self.scene()
        for i in range(dotCount):
            dot = Dot()
            dot.setPos(math.sin((i * 6.28) / dotCount) * 200,
                         math.cos((i * 6.28) / dotCount) * 200)
            dot.color = self.colors[self.colorcycle % len(self.colors)]
            scene.addItem(dot)

    def keyPressEvent(self, event):
        key = event.key()

        if event.modifiers() & QtCore.Qt.ShiftModifier:
            delta = 10
        elif event.modifiers() & QtCore.Qt.AltModifier:
            delta = 50
        else:
            delta = 1
        print("key: %x, mod=%x" % (event.key(), event.modifiers()))

        if key == QtCore.Qt.Key_1:
            self.startCircle()
        elif key == QtCore.Qt.Key_2:
            self.scene().clear()
        elif key == QtCore.Qt.Key_R:
            self.colorfilter = QtCore.Qt.red
        elif key == QtCore.Qt.Key_G:
            self.colorfilter = QtCore.Qt.green
        elif key == QtCore.Qt.Key_B:
            self.colorfilter = QtCore.Qt.blue
        elif key == QtCore.Qt.Key_M:
            self.paramfilter= "mass"
        elif key == QtCore.Qt.Key_D:
            self.paramfilter= "radius"
        elif key in (QtCore.Qt.Key_Plus, QtCore.Qt.Key_Equal):
            self.modifyItems(delta)
        elif key in (QtCore.Qt.Key_Minus, QtCore.Qt.Key_Underscore):
            self.modifyItems(-delta)

    def modifyItems(self, delta):
        if self.paramfilter is None:
            return
        for item in self.scene().items():
            if isinstance(item, Dot):
                if self.colorfilter is None or item.color == self.colorfilter:
                    setattr(item, self.paramfilter, getattr(item, self.paramfilter) + delta)
                    item.calcBounding()

    def timerEvent(self, event):
        # first calc all updates
        for item in self.scene().items():
            if isinstance(item, Dot):
                item.calcNewpos()
        # then perform all updates
        for item in self.scene().items():
            if isinstance(item, Dot):
                item.updatePos()



if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    QtCore.qsrand(QtCore.QTime(0,0,0).secsTo(QtCore.QTime.currentTime()))

    widget = GraphWidget()
    widget.show()
    widget.raise_()

    print("""
1 - start circle
2 - clear scene
r,g,b - select dots by color: red,green,blue
m - select mass
d - select radius
+.-  - modify selected param for selected dots
""")
    sys.exit(app.exec_())
