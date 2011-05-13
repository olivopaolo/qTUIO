/*
    qTUIO - TUIO Interface for Qt

    Original Version by Martin Blankenburg <martin.blankenburg@imis.uni-luebeck.de>
    Integrated into qTUIO by x29a <0.x29a.0@gmail.com>
    Some modifications by Paolo Olivo <olivopaolo@tiscali.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qtuio.h"

#include <QEvent>
#include <QTouchEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QMainWindow>

#include <QtDebug>

#include "TuioClient.h"
#include "TuioCursor.h"

#ifdef WIN32
#define OFFSETX 5
#define OFFSETY 35
#else
#define OFFSETX 0
#define OFFSETY 0
#endif

QTuio::QTuio(QObject *parent) : id(0) {
    theWidget = qobject_cast<QWidget *>(parent);
    theView = qobject_cast<QGraphicsView *>(parent);
    if (theView)
        theScene = theView->scene();
    else
        theScene = qobject_cast<QGraphicsScene *>(parent);
    // Default calibration has screen size
    QRect screen = QApplication::desktop()->rect();
    qreal data[] = { screen.width(), 0, 0, 
                     0, screen.height(), 0, 
                     0, 0, 1.0 } ; 
    calibration = QMatrix3x3(data) ; 
}

QTuio::~QTuio() {
    if (running) {
        tuioClient->disconnect();
        delete tuioClient;
        delete qTouchPointMap;
        running = false;
        wait();
    }
}

void 
QTuio::run() {
    running = true;
    tuioClient = new TUIO::TuioClient(3333);
    tuioClient->addTuioListener(this);
    tuioClient->connect();
    qTouchPointMap = new QMap<int, QTouchEvent::TouchPoint>();
}

void 
QTuio::setTuioCalibration(QMatrix3x3 &calibration) {
  this->calibration = calibration ;
}

/*
bool QTuio::tuioToQt(TUIO::TuioCursor *tcur, QEvent::Type eventType)
{
    const QPointF normPos(tcur->getX(), tcur->getY());
    const QPointF screenPos(screenRect.width() * normPos.x() - OFFSETX, screenRect.height() * normPos.y() - OFFSETY);

    QTouchEvent::TouchPoint touchPoint(tcur->getSessionID());

    touchPoint.setNormalizedPos(normPos);
    touchPoint.setRect(QRectF());

    touchPoint.setPressure(1.0);
    touchPoint.setScreenRect(screenRect);
    touchPoint.setScreenPos(screenPos);

    if (theScene) {
        touchPoint.setSceneRect(theScene->sceneRect());
        if (theView) {
            const QPoint pos((int)screenPos.x() - theView->geometry().x(),
                             (int)screenPos.y() - theView->geometry().y());
            touchPoint.setPos(pos);
            touchPoint.setScenePos(theView->mapToScene(pos));
        } else {
            foreach (QGraphicsView *view, theScene->views()) {
                if (view->isActiveWindow()) {
                    const QPoint pos((int)screenPos.x() - view->geometry().x(),
                                     (int)screenPos.y() - view->geometry().y());
                    touchPoint.setPos(pos);
                    touchPoint.setScenePos(view->mapToScene(pos));
                }
            }
        }
    } else {
        const QPoint pos((int)screenPos.x() - theMainWindow->geometry().x(),
                         (int)screenPos.y() - theMainWindow->geometry().y());
        touchPoint.setPos(pos);
        touchPoint.setSceneRect(QRectF());
        touchPoint.setScenePos(pos);
    }

    Qt::TouchPointStates touchPointStates;

    switch (eventType) {
    case QEvent::TouchBegin: {
	    touchPointStates = Qt::TouchPointPressed;

            touchPoint.setState(Qt::TouchPointPressed);
            touchPoint.setStartNormalizedPos(normPos);
            touchPoint.setStartPos(touchPoint.pos());
            touchPoint.setStartScreenPos(screenPos);
            touchPoint.setStartScenePos(touchPoint.scenePos());

            touchPoint.setLastNormalizedPos(normPos);
            touchPoint.setLastPos(touchPoint.pos());
            touchPoint.setLastScreenPos(screenPos);
            touchPoint.setLastScenePos(touchPoint.scenePos());

            qTouchPointMap->insert(tcur->getSessionID(), touchPoint);
            break;
        }
    case QEvent::TouchUpdate: {
            if (tcur->getMotionSpeed() > 0)
	    {
                touchPointStates = Qt::TouchPointMoved;

                touchPoint.setState(Qt::TouchPointMoved);
	    }
            else
	    {
                touchPointStates = Qt::TouchPointStationary;

                touchPoint.setState(Qt::TouchPointStationary);
	    }

            touchPoint.setStartNormalizedPos(qTouchPointMap->value(tcur->getSessionID()).startNormalizedPos());
            touchPoint.setStartPos(qTouchPointMap->value(tcur->getSessionID()).startPos());
            touchPoint.setStartScreenPos(qTouchPointMap->value(tcur->getSessionID()).startScreenPos());
            touchPoint.setStartScenePos(qTouchPointMap->value(tcur->getSessionID()).startScenePos());

            touchPoint.setLastNormalizedPos(qTouchPointMap->value(tcur->getSessionID()).normalizedPos());
            touchPoint.setLastPos(qTouchPointMap->value(tcur->getSessionID()).pos());
            touchPoint.setLastScreenPos(qTouchPointMap->value(tcur->getSessionID()).screenPos());
            touchPoint.setLastScenePos(qTouchPointMap->value(tcur->getSessionID()).scenePos());

            qTouchPointMap->insert(tcur->getSessionID(), touchPoint);
            break;
        }
    case QEvent::TouchEnd: {
            touchPointStates = Qt::TouchPointReleased;

            touchPoint.setState(Qt::TouchPointReleased);

            touchPoint.setStartNormalizedPos(qTouchPointMap->value(tcur->getSessionID()).startNormalizedPos());
            touchPoint.setStartPos(qTouchPointMap->value(tcur->getSessionID()).startPos());
            touchPoint.setStartScreenPos(qTouchPointMap->value(tcur->getSessionID()).startScreenPos());
            touchPoint.setStartScenePos(qTouchPointMap->value(tcur->getSessionID()).startScenePos());

            touchPoint.setLastNormalizedPos(qTouchPointMap->value(tcur->getSessionID()).normalizedPos());
            touchPoint.setLastPos(qTouchPointMap->value(tcur->getSessionID()).pos());
            touchPoint.setLastScreenPos(qTouchPointMap->value(tcur->getSessionID()).screenPos());
            touchPoint.setLastScenePos(qTouchPointMap->value(tcur->getSessionID()).scenePos());

            qTouchPointMap->insert(tcur->getSessionID(), touchPoint);
            break;
        }
    default: {}
    }

    QEvent *touchEvent = new QTouchEvent(eventType, QTouchEvent::TouchScreen, Qt::NoModifier, touchPointStates, qTouchPointMap->values());
*/
/**********************************************************
 * Old Code doesn't work with QGraphicsView
 * it doesn't send events to the viewport
 * and which make the pinchzoom example doesn't work
***********************************************************/
/*    if (theScene)
        qApp->postEvent(theScene, touchEvent);
    else if (theView)
        qApp->postEvent(theView->scene(), touchEvent);
    else
        qApp->postEvent(theMainWindow->centralWidget(), touchEvent);*/

/************************************************
 * New code fixing the issue with QGraphicsViw
*************************************************/
/*
	 if (theView && theView->viewport())
        qApp->postEvent(theView->viewport(), touchEvent);
    else if (theScene)
        qApp->postEvent(theScene, touchEvent);
    else
        qApp->postEvent(theMainWindow->centralWidget(), touchEvent);


    if (eventType == QEvent::TouchEnd)
        qTouchPointMap->remove(tcur->getSessionID());

    return true;
}
*/

void QTuio::addTuioCursor(TUIO::TuioCursor *tcur) {
  // qDebug("addTuioCursor") ;
  QWidget *target ;
  QPoint screenPos = tuioToScreen(QPointF(tcur->getX(), tcur->getY())) ;
  target = theWidget->childAt((int)screenPos.x() - theWidget->geometry().x(),
			      (int)screenPos.y() - theWidget->geometry().y()) ;
  if (target == 0) target = theWidget ;

  QTouchEvent::TouchPoint touchPoint(id++) ;
  touchPoint.setRect(QRectF()) ;
  touchPoint.setPressure(1.0) ;
  touchPoint.setState(Qt::TouchPointPressed) ;

  struct Tuio2Qt qts = {touchPoint, target} ;
  tuio2Qt.insert(tcur->getSessionID(), qts) ;
  // Associate the new touchPoint to the correspondent widget
  QList<long> widgetTouches = widgets.value(target) ;
  widgetTouches.append(tcur->getSessionID()) ;
  widgets.insert(target, widgetTouches) ;
  updateTouch(tcur) ;
}

void QTuio::updateTuioCursor(TUIO::TuioCursor *tcur) {
  // qDebug("updateTuioCursor") ;
  struct Tuio2Qt qts = tuio2Qt.value(tcur->getSessionID()) ;
  QTouchEvent::TouchPoint &tPoint = qts.tPoint ;
  if (tPoint.state() != Qt::TouchPointPressed) {
    if (tcur->getMotionSpeed() > 0) {
      tPoint.setState(Qt::TouchPointMoved);
    } else {
      tPoint.setState(Qt::TouchPointStationary);
    }
  }
  tuio2Qt.insert(tcur->getSessionID(), qts) ;
  updateTouch(tcur) ;
}

void QTuio::removeTuioCursor(TUIO::TuioCursor *tcur) {
  // qDebug("removeTuioCursor") ;
  struct Tuio2Qt qts = tuio2Qt.value(tcur->getSessionID()) ;
  qts.tPoint.setState(Qt::TouchPointReleased) ;
  updateTouch(tcur) ;
  tuio2Qt.insert(tcur->getSessionID(), qts) ;
}

void QTuio::addTuioObject(TUIO::TuioObject * /*tobj*/) {}

void QTuio::updateTuioObject(TUIO::TuioObject * /*tobj*/) {}

void QTuio::removeTuioObject(TUIO::TuioObject * /*tobj*/) {}

void  QTuio::refresh(TUIO::TuioTime /*frameTime*/) {
  //  qDebug("refresh") ;
  QList<QWidget *> keys = widgets.keys() ;
  for (int i = 0; i<keys.size(); ++i) {
    QWidget *currWidget = keys[i] ;
    QList<long> touches = widgets.value(currWidget) ;
    bool touchBegin = true ;
    bool touchEnd = true ;
    bool sendEvent = false ;
    Qt::TouchPointStates tps ;
    QList<QTouchEvent::TouchPoint> tPts ;
    
    QMutableListIterator<long> tIt(touches) ;
    while (tIt.hasNext()) {
      long id = tIt.next() ;
      struct Tuio2Qt qts = tuio2Qt.value(id) ;
      if (qts.tPoint.state() != Qt::TouchPointPressed)
	touchBegin = false ;
      if (qts.tPoint.state() != Qt::TouchPointReleased)
	touchEnd = false ;
      if (qts.tPoint.state() != Qt::TouchPointStationary)
	sendEvent = true ;
      tps |= qts.tPoint.state() ;
      tPts.append(qts.tPoint) ;

      if (qts.tPoint.state() == Qt::TouchPointReleased) {
	tuio2Qt.remove(id) ;
	tIt.remove() ;
      } else {
	qts.tPoint.setState(Qt::TouchPointStationary) ;
	tuio2Qt.insert(id, qts) ;
      }
    }
    // update the old value associated to this widget
    widgets.insert(currWidget, touches) ;
    if (sendEvent) {
      QEvent::Type eventType ;
      if (touchBegin)
	eventType = QEvent::TouchBegin ;
      else if (touchEnd)
	eventType = QEvent::TouchEnd ;
      else
	eventType = QEvent::TouchUpdate ;

      QEvent *touchEvent = new QTouchEvent(eventType, QTouchEvent::TouchScreen, Qt::NoModifier, tps, tPts);
      if (theScene)
	qApp->postEvent(theScene, touchEvent);
      else if (theView)
	qApp->postEvent(theView->scene(), touchEvent);
      else
	qApp->postEvent(currWidget, touchEvent);
    }
  }

  // if a widget has an empty list it can be dropped
  QList<QWidget *> touchedWidgets = widgets.keys() ;
  for (int i = 0; i<touchedWidgets.size(); ++i) 
    if (widgets.value(touchedWidgets[i]).isEmpty())
      widgets.remove(touchedWidgets[i]) ;
    
  // if there is no more touchPoints id can be reset to zero
  if (widgets.empty()) id = 0 ;
}

void
QTuio::updateTouch(TUIO::TuioCursor *tcur) {
  struct Tuio2Qt qts = tuio2Qt.value(tcur->getSessionID()) ;
  QTouchEvent::TouchPoint &tPoint = qts.tPoint ;

  // --- Normalized coordinates ---
  tPoint.setLastNormalizedPos(tPoint.normalizedPos()) ;
  tPoint.setNormalizedPos(QPointF(tcur->getX(), tcur->getY())) ;
  
  // --- Screen coordinates ---
  // TODO: why it is necessary to pass the screenRect if it is available from QApplication?
  // The following method modifies the screenPos
  // tPoint.setScreenRect(screenRect);
  tPoint.setLastScreenPos(tPoint.screenPos()) ;
  tPoint.setScreenPos(tuioToScreen(tPoint.normalizedPos())) ;
  
  // --- Screen coordinates ---
  tPoint.setLastPos(tPoint.pos()) ;
  tPoint.setPos(mapWidgetToGlobal(qts.widget, tPoint.screenPos())) ;
  //  qDebug() << tPoint.normalizedPos() << tPoint.screenPos() << tPoint.pos() ;
  // --- Start coordinates ---
  if (tPoint.state() == Qt::TouchPointPressed) {
    tPoint.setStartNormalizedPos(tPoint.normalizedPos());
    tPoint.setStartPos(tPoint.pos());
    tPoint.setStartScreenPos(tPoint.screenPos());
    tPoint.setStartScenePos(tPoint.scenePos());

    tPoint.setLastNormalizedPos(tPoint.normalizedPos());
    tPoint.setLastPos(tPoint.pos());
    tPoint.setLastScreenPos(tPoint.screenPos());
    tPoint.setLastScenePos(tPoint.scenePos());
  }

  tuio2Qt.insert(tcur->getSessionID(), qts) ;
}

QPoint 
QTuio::tuioToScreen(const QPointF &normalized) const {
  qreal pos[] = { normalized.x(), 
		  normalized.y(), 
		  1 } ;
  QGenericMatrix<1,3,qreal> result = calibration * QGenericMatrix<1,3,qreal>(pos) ;
  return QPoint(result.data()[0], result.data()[1]) ;
}

QPointF
QTuio::mapWidgetToGlobal(QWidget *widget, const QPointF &pos) const {
  //  qDebug() << widget << pos ;
  QPointF upLevel(pos.x() - widget->geometry().x(),
		  pos.y() - widget->geometry().y());
  if (widget->parentWidget())
    return mapWidgetToGlobal(widget->parentWidget(), upLevel) ;
  else
    return upLevel ;
}
