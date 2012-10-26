#ifndef __INA_EVENT_HH__
#define __INA_EVENT_HH__

#include "smartptr.hh"
#include <list>
#include <ast/node.hh>


namespace iNA {

class AbstractSlot {
public:
  virtual ~AbstractSlot();
  virtual void handle(Ast::Node *sender) = 0;
};


template <class T>
class Slot : public AbstractSlot
{
protected:
  /** Holds the instance. */
  T *_instance;
  /** Pointer to a member function of template-class. */
  void (T::*_function)();

public:
  /** Constructor. */
  Slot(T *instance, void (T::*function)()) : _instance(instance), _function(function) { }

  /** Calls the referenced handler. */
  void handle(Ast::Node *sender) { (_instance->*_function)(); }
};


/** Trivial event class. */
class Event
{
protected:
  /** Holds the connected handlers. */
  std::list<AbstractSlot *> _handler;
  Ast::Node *_sender;

public:
  Event(Ast::Node *sender);
  ~Event();

  void send();

  void connect(AbstractSlot *handler);

  template <class T> void connect(T *instance, void (T::*function)()) {
    add(new Slot<T>(instance, function));
  }
};


}

#endif // EVENT_HH
