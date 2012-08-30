#ifndef __FLUC_SMART_PTR_HH__
#define __FLUC_SMART_PTR_HH__

namespace iNA {


/**
 * Class for counted reference semantics.
 *
 * Deletes the object to which it refers when the last CountedPtr that refers to it is destroyed.
 */
template <class T>
class SmartPtr
{
private:
  /**
   * Pointer to the value.
   */
  T* ptr;

  /**
   * Shared number of owners.
   */
  long* count;


public:
  /**
   * Initialize pointer with existing pointer.
   *
   * Requires that the pointer p is a return value of new.
   */
  explicit SmartPtr (T* p=0)
    : ptr(p), count(new long(1))
  {
     // Pass..
  }

  /**
   * Copy pointer (one more owner).
   */
  SmartPtr (const SmartPtr<T>& p) throw()
    : ptr(p.ptr), count(p.count)
  {
    if(0 != this->count)
    {
      ++*count;
    }
  }

  /**
   * Destructor (delete value if this was the last owner).
   */
  ~SmartPtr () throw()
  {
    dispose();
  }

  /**
   * assignment (unshare old and share new value)
   */
  SmartPtr<T>&
  operator= (const SmartPtr<T>& p) throw()
  {
    if (this != &p)
    {
      // First increment the counter of the element being assigned:
      ++(*(p.count));

      // Then, decrement (and free if needed) the element being held:
      dispose();

      // Take over pointer and reference counter:
      ptr = p.ptr;
      count = p.count;
    }

    // Done.
    return *this;
  }

  /**
   * Access the value to which the pointer refers.
   */
  T&
  operator*() const throw()
  {
    return *ptr;
  }

  /**
   * Element access for instances.
   */
  T*
  operator->() const throw()
  {
    return ptr;
  }

  /**
   * Returns a weak reference.
   */
  T*
  get_ptr() const throw()
  {
    return ptr;
  }


private:
  /**
   * Actually destroys the instance and counter.
   */
  void dispose()
  {
    if (--*count == 0)
    {
      delete count;
      if (0 != ptr)
        delete ptr;
    }
  }
};


}

#endif
