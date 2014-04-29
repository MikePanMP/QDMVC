#include "MVC.h"
#include "MVCMacro.h"

void mike(QString str) {
	QMessageBox::about(NULL, QObject::tr(""), str);
}
void mike(const char* str) {
	QMessageBox::about(NULL, QObject::tr(""), QObject::tr(str));
}
void mike(int i) {
	QMessageBox::about(NULL, QObject::tr(""), QString("%1").arg(i));
}

MVCModel::MVCModel() {
}
MVCModel::~MVCModel() {
}
bool MVCModel::addDependent(MVCView* view) {
	dependents.append(view);
	return true;
}
void MVCModel::selfChanged() {
	emit changed();
}

MVCController::MVCController(MVCView* _view) {
	//mike("MVCController::MVCController");
	view = _view;
	model = _view->getModel();
	listenToView();
}

MVCController::~MVCController() {
}

MVCModel* MVCController::getModel() {
	return model;
}

MVCView* MVCController::getView() {
	return view;
}

void MVCController::redButtonPressedEvent() {
	//mike("MVCController::redButtonPressedEvent");
	return;
}
void MVCController::redButtonReleasedEvent() {
	return;
}
void MVCController::redButtonDoubleClickEvent() {
	return;
}
void MVCController::blueButtonPressedEvent() {
	return;
}
void MVCController::blueButtonReleasedEvent() {
	return;
}
void MVCController::blueButtonDoubleClickEvent() {
	return;
}
void MVCController::yellowButtonPressedEvent() {
	return;
}
void MVCController::yellowButtonReleasedEvent() {
	return;
}
void MVCController::yellowButtonDoubleClickEvent() {
	return;
}
void MVCController::keyPressEvent(char key) {
	return;
}
void MVCController::keyReleaseEvent(char key) {
	return;
}

void MVCController::eventCaptured(MVCMacro::EventId eid, QGraphicsSceneMouseEvent* e) {
	//mike("MVCController::eventCaptured");
	//mike(e->button());
	switch (eid) {
	case MVCMacro::EVENT_MOUSE_PRESS:
		switch (e->button()) {
		case Qt::LeftButton:	redButtonPressedEvent(); break;
		case Qt::MiddleButton:	blueButtonPressedEvent(); break;
		case Qt::RightButton:	yellowButtonPressedEvent();	break;
		default: break;
		}
		break;
	case MVCMacro::EVENT_MOUSE_RELEASE:
		switch (e->button()) {
		case Qt::LeftButton:	redButtonReleasedEvent(); break;
		case Qt::MiddleButton:	blueButtonReleasedEvent(); break;
		case Qt::RightButton:	yellowButtonReleasedEvent(); break;
		default: break;
		}
		break;
	default:
		break;
	}
}

void MVCController::eventCaptured(MVCMacro::EventId eid, QKeyEvent* e) {
	switch (eid) {
	case MVCMacro::EVENT_KEY_PRESS:
		keyPressEvent((char)e->key());
		break;
	case MVCMacro::EVENT_KEY_RELEASE:
		keyReleaseEvent((char)e->key());
		break;
	default:
		break;
	}
}

void MVCController::listenToView() {
	//mike("MVCController::listenToView");
	// Mouse
	connect(
		view, 
		SIGNAL(eventCaptured(MVCMacro::EventId, QGraphicsSceneMouseEvent*)), 
		this, 
		SLOT(eventCaptured(MVCMacro::EventId, QGraphicsSceneMouseEvent*))
		);
	// Key
	connect(
		view, 
		SIGNAL(eventCaptured(MVCMacro::EventId, QKeyEvent*)), 
		this, 
		SLOT(eventCaptured(MVCMacro::EventId, QKeyEvent*))
		);
}

void MVCView::init(bool focus) {
	connect(this, SIGNAL(update()), this, SLOT(invalidate()));

	model->addDependent(this);
	connect(model, SIGNAL(changed()), this, SIGNAL(update()));

	initWidget();
	if (focus)
		setFlags(ItemIsFocusable);
	setAcceptedMouseButtons( Qt::LeftButton | Qt::RightButton | Qt::MiddleButton );
	controller = defaultController();
}

void MVCView::changeToType(MVCMacro::ViewType _viewType) {
	//mike("MVCView::changeToType");
	if (component) {
		if (viewType != _viewType) {
			delete component;
			component = NULL;
			viewType = _viewType;
			initWidget();
		}
	} else {
		viewType = _viewType;
		initWidget();
	}
}

MVCController* MVCView::defaultController() {
	//mike("MVCView::defaultController");
	return new MVCController(this);
}

void MVCView::displayImage(const QString png) {
	if (viewType == MVCMacro::VIEW_TYPE_PIXMAP) {
		//mike("MVCView::displayImage");
		static_cast<MVCPixmap*>(component)->changePixmap(png);
	}
}

MVCView::MVCView(
	MVCModel* _model, 
	MVCMacro::ViewType _viewType, 
	MVCView *_parent) 
	: model(_model), viewType(_viewType), QGraphicsObject(_parent), parent(_parent) {
	//mike("MVCView::MVCView");
}

MVCView::~MVCView() {
}

MVCModel* MVCView::getModel() {
	return model;
}
MVCView* MVCView::getMVCParent() {
	return parent;
}
MVCController* MVCView::getController() {
	return controller;
}

void MVCView::initWidget() {
	//mike("MVCView::initWidget");
	switch (viewType) {
	case MVCMacro::VIEW_TYPE_PIXMAP:
		component = new MVCPixmap;
		component->setParentItem(this);
		break;
	default:
		break;
	}
}

void MVCView::mousePressEvent(QGraphicsSceneMouseEvent* e) {
	emit eventCaptured(MVCMacro::EVENT_MOUSE_PRESS, e);
}
void MVCView::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
	emit eventCaptured(MVCMacro::EVENT_MOUSE_RELEASE, e);
}
void MVCView::keyPressEvent(QKeyEvent* e) {
	if (!e->isAutoRepeat())
		emit eventCaptured(MVCMacro::EVENT_KEY_PRESS, e);
}
void MVCView::keyReleaseEvent(QKeyEvent* e) {
	if (!e->isAutoRepeat())
		emit eventCaptured(MVCMacro::EVENT_KEY_RELEASE, e);
}

QRectF MVCView::boundingRect() const {
	QRectF thisRect(0, 0, 200, 200);
	if (viewType == MVCMacro::VIEW_TYPE_PIXMAP) {
		if (!component)
			return thisRect;
		else
			return component->boundingRect();
	}

	return thisRect;
}
void MVCView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget){
	QRectF rect = boundingRect();
	//painter->fillRect(rect, QColor(0,0,0,0));
	painter->fillRect(rect, QColor(Qt::transparent));
}

void MVCView::invalidate() {
	selfUpdate();
	prepareGeometryChange();
}

QRectF MVCPixmap::boundingRect() const{
	return QRectF(0, 0, pixmap.width(), pixmap.height());
}

bool MVCPixmap::changePixmap(const QString &filename){
	bool success = pixmap.load(filename);
	if(success) {
		prepareGeometryChange();
	}
	return success;
}

void MVCPixmap::setPixmap(const QPixmap& _pixmap){
	pixmap = _pixmap;
	prepareGeometryChange();
}

MVCPixmap::MVCPixmap(const QString &filename, bool center_as_origin) : pixmap(filename) {
	if(center_as_origin)
		setTransformOriginPoint(pixmap.width()/2, pixmap.height()/2);
}

MVCPixmap::MVCPixmap() {
}

MVCPixmap::~MVCPixmap() {
}

QVariant MVCPixmap::itemChange(GraphicsItemChange change, const QVariant &value) {
	return QGraphicsObject::itemChange(change, value);
}

void MVCPixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	painter->drawPixmap(0, 0, pixmap);
}
