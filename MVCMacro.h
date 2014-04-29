#ifndef MVC_MACRO_H
#define MVC_MACRO_H

namespace MVCMacro {
	enum EventId {
		EVENT_MOUSE_PRESS = 2014,
		EVENT_MOUSE_RELEASE,
		EVENT_MOUSE_MOVE,
		EVENT_MOUSE_DOUBLE_CLICK,
		EVENT_KEY_PRESS,
		EVENT_KEY_RELEASE
	};
	enum ViewType {
		VIEW_TYPE_LABEL = 2014,
		VIEW_TYPE_PIXMAP
	};
}

#endif // MVC_MACRO_H