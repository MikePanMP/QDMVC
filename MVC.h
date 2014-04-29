#ifndef MVC_H
#define MVC_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtGui/QPainter>
#include <QtGui/QColor>
#include <QtGui/qevent.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsSceneEvent>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMainwindow>
#include <QtWidgets/QWidget>

#include "MVCMacro.h"

class MVCApp;
class MVCModel;
class MVCView;
class MVCController;

class MVCPixmap;
/*
 * MVCApp
 */
class MVCApp {
public:
	MVCApp(int argc, char *argv[]) : a(argc, argv), flag(false) {
		view = new QGraphicsView(&scene);
		view->setScene(&scene);
		window.setCentralWidget(view);
	}
	virtual ~MVCApp() {
	}
public:
	void addItem(QGraphicsItem* item) {
		if (flag) 
			return;
		scene.addItem(item);
		flag = true;
	}
	void show() {
		window.show();
	}
	void setFixedSize(int w, int h) {
		window.setFixedSize(w, h);
	}
	int exec() {
		return a.exec();
	}
	void exit() {
		a.quit();
	}
private:
	QApplication a;
	QMainWindow window;
	QGraphicsScene scene;
	QGraphicsView* view;
	bool flag;
};
/*
 * MVCModel
 */
class MVCModel : public QObject {
	Q_OBJECT
public:
	bool addDependent(MVCView* view);
	void selfChanged();
public:
	MVCModel();
	virtual ~MVCModel();
signals:
	void changed();
private:
	QList<MVCView*> dependents;
};
/*
 * MVCController
 */
class MVCController : public QObject {
	Q_OBJECT
public:
	MVCController(MVCView* _view);
	virtual ~MVCController();
	MVCModel* getModel();
	MVCView* getView();
public:
	virtual void redButtonPressedEvent();
	virtual void redButtonReleasedEvent();
	virtual void redButtonDoubleClickEvent();
	virtual void blueButtonPressedEvent();
	virtual void blueButtonReleasedEvent();
	virtual void blueButtonDoubleClickEvent();
	virtual void yellowButtonPressedEvent();
	virtual void yellowButtonReleasedEvent();
	virtual void yellowButtonDoubleClickEvent();
	virtual void keyPressEvent(char key);
	virtual void keyReleaseEvent(char key);
public slots: // Methods for slots
	void eventCaptured(MVCMacro::EventId eid, QGraphicsSceneMouseEvent* e);
	void eventCaptured(MVCMacro::EventId eid, QKeyEvent* e);
signals:// Methods for signals
private:// Methods for internal use
	void listenToView();
private:
	MVCModel* model;
	MVCView* view;
};
/*
 * MVCView
 */
class MVCView : public QGraphicsObject {
	Q_OBJECT
public:// Methods for override
	virtual void selfUpdate();
	virtual MVCController* defaultController();
public:// Methods for functional use
	void init(bool focus = true);// called in subclass's constructor
	void changeToType(MVCMacro::ViewType _viewType);
	void displayImage(QString png);
public:// Mechods of basics
	MVCView(MVCModel* _model, 
		MVCMacro::ViewType _viewType = MVCMacro::VIEW_TYPE_PIXMAP, 
		MVCView* _parent = 0);
	virtual ~MVCView();
public:
	MVCModel* getModel();
	MVCView* getMVCParent();
	MVCController* getController();
protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* e);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
	virtual void keyPressEvent(QKeyEvent* e);
	virtual void keyReleaseEvent(QKeyEvent* e);
public:
	virtual QRectF boundingRect() const;
protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
public slots:// Methods for slots
	void invalidate();
signals://Methods for signals
	void update();
	void eventCaptured(MVCMacro::EventId, QGraphicsSceneMouseEvent*);
	void eventCaptured(MVCMacro::EventId, QKeyEvent*);
private://Methods for internal use
	void initWidget();
private:
	MVCModel* model;
	MVCController* controller;
	MVCMacro::ViewType viewType;
	QGraphicsObject* component;
	MVCView* parent;
};
/*
 * MVCPixmap
 */
class MVCPixmap : public QGraphicsObject {
	Q_OBJECT
public:
	virtual QRectF boundingRect() const;
	bool changePixmap(const QString &filename);
	void setPixmap(const QPixmap& _pixmap);
public:
	MVCPixmap(const QString &filename, bool center_as_origin = true);
	MVCPixmap();
	virtual ~MVCPixmap();
protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QPixmap pixmap;
};
#endif // MVC_H