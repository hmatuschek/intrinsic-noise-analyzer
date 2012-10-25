#include "event.hh"

using namespace iNA;


AbstractSlot::~AbstractSlot()
{
  // Pass...
}



Event::Event(Ast::Node *sender)
  : _sender(sender)
{
  // Pass..
}

Event::~Event()
{
  for (std::list<AbstractSlot *>::iterator item=_handler.begin(); item!=_handler.end(); item++) {
    delete *item;
  }
}

void
Event::connect(AbstractSlot *handler) {
  _handler.push_back(handler);
}

void
Event::send() {
  for (std::list<AbstractSlot *>::iterator item=_handler.begin(); item!=_handler.end(); item++) {
    (*item)->handle(_sender);
  }
}
