#ifndef __FLUC_LINKEDLIST_HH__
#define __FLUC_LINKEDLIST_HH__

namespace Fluc {


/**
 * A mix-in class implenting a double-linked list.
 */
template <class T>
class LinkedList : public T
{
public:
  /**
   * Defines a forward-iterator over elements of the list.
   */
  class iterator
  {
  protected:
    /**
     * Holds a weak reference to the current list item, if 0 -> end of list.
     */
    LinkedList<T> *_item;


  public:
    /**
     * Constructs a forward iterator pointing right after the last element in any list.
     */
    iterator()
      : _item(0)
    {
      // Pass...
    }

    /**
     * Constructs a forward iterator pointing to the given element.
     */
    iterator(LinkedList<T> *node)
      : _item(node)
    {
      // Pass...
    }

    /**
     * Implements comparison of iterators by comparison of referenced items.
     */
    inline bool operator==(const iterator &other) const
    {
      return this->_item == other._item;
    }

    /**
     * Implements comparison of iterators by comparison of referenced items.
     */
    inline bool operator!=(const iterator &other) const
    {
      return this->_item != other._item;
    }

    /**
     * Increments forward iterator.
     */
    inline iterator const operator++(int)
    {
      // If not at end-of-list -> update item reference to next item
      if (0 != this->_item)
      {
        this->_item = this->_item->_next;
      }

      // Done.
      return *this;
    }

    /**
     * Returns item.
     */
    inline LinkedList<T> *operator->()
    {
      return this->_item;
    }

    /**
     * Retunrs reference to item.
     */
    inline LinkedList<T> *&operator*()
    {
      return this->_item;
    }
  };


  /**
   * Defines a revered iterator over the linked list.
   */
  class reverse_iterator
  {
  protected:
    /**
     * Holds a weak reference to the current item.
     */
    LinkedList<T> *_item;

  public:
    /**
     * Constructs a reversed iterator pointing right before the first element in list.
     */
    reverse_iterator()
      : _item(0)
    {
      // Pass...
    }

    /**
     * Constructs a reversed iterator pointing to the given item.
     */
    reverse_iterator(LinkedList<T> *node)
      : _item(node)
    {
      // Pass...
    }

    /**
     * Implements iterator comparison by item comparison.
     */
    inline bool operator==(const reverse_iterator &other) const
    {
      return this->_item == other._item;
    }

    /**
     * Implements iterator comparison by item comparison.
     */
    inline bool operator!=(const reverse_iterator &other) const
    {
      return this->_item != other._item;
    }

    /**
     * Increments the reversed iterator.
     */
    inline reverse_iterator const operator++(int)
    {
      if (0 != this->_item)
      {
        this->_item = this->_item->_prev;
      }

      return *this;
    }

    /**
     * Returns the item.
     */
    inline LinkedList<T> *operator->()
    {
      return this->_item;
    }

    /**
     * Returns a reference to the item.
     */
    inline LinkedList<T> *&operator*()
    {
      return this->_item;
    }
  };


protected:
  /**
   * Holds a weak reference to the next element in list.
   */
  LinkedList<T> *_next;

  /**
   * Holds a weak reference to the previous element in list.
   */
  LinkedList<T> *_prev;


public:
  /**
   * Default constructor, does nothing.
   */
  LinkedList()
    : _next(0), _prev(0)
  {
    // Pass...
  }

  /**
   * Retunrs the next element in list.
   */
  inline LinkedList<T>* next()
  {
    return this->_next;
  }

  /**
   * Returns the previous element in list.
   */
  inline LinkedList<T>* prev()
  {
    return this->_prev;
  }

  /**
   * Retunrs the first element in list.
   */
  LinkedList<T>* first()
  {
    if (0 == this->_prev)
    {
      return this;
    }

    return this->_prev->first();
  }

  /**
   * Returns the last element in list.
   */
  LinkedList<T>* last()
  {
    if (0 == this->_next)
    {
      return this;
    }

    return this->_next->last();
  }


  /**
   * Inserts an item (or list of items) after this item.
   */
  void insertAfter(LinkedList<T>* node)
  {
    LinkedList<T> *old_tail = this->_next;

    this->_next = node; node->_prev = this;

    if (0 != old_tail)
    {
      node->last()->insertAfter(old_tail);
    }
  }
};


}
#endif // __FLUC_LINKEDLIST_HH__
