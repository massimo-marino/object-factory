/* 
 * File:   objectFactory.h
 * Author: massimo
 *
 * Created on September 20, 2017, 1:15 PM
 */

#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include <memory>
#include <mutex>
////////////////////////////////////////////////////////////////////////////////
namespace object_factory
{
// create an object of type T and return a std::unique_ptr to it
template <typename T, typename... Args>
auto
createUniquePtr(Args&&... args) -> std::unique_ptr<T>
{
  // since C++14
  return std::make_unique<T>(args...);
}

template <typename T>
using objectFactoryFun = std::function<std::unique_ptr<T>(void)>;

template <typename T, typename... Args>
auto
createObjectFactoryFun(Args&&... args) noexcept -> objectFactoryFun<T>
{
  // return a function object for creating T's objects with the given arguments
  // to be passed to its constructor
  return [args...]()
         {
           return createUniquePtr<T>(std::forward<Args>(args)...);
         };
}
}  // namespace object_factory

namespace object_factory::object_counter
{
//
// See:
// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
//
// TC, the type of the counters, MUST be unsigned
// TC is unsigned long by default
template <typename T, typename TC = unsigned long>
struct objectCounter
{
 public:
  using counterType = TC;
  using counterStatus = std::tuple<counterType, counterType, counterType, bool>;

  objectCounter() noexcept(false)
  {
    std::lock_guard<std::mutex> lg(mtx_);
    ++objectsCreated_;
    ++objectsAlive_;
    if ( checkCounterOverflow() )
    {
      throw std::overflow_error("Object Counters in OVERFLOW");
    }
  }

  objectCounter(const objectCounter&) noexcept(false)
  {
    std::lock_guard<std::mutex> lg(mtx_);
    ++objectsCreated_;
    ++objectsAlive_;
    if ( checkCounterOverflow() )
    {
      throw std::overflow_error("Object Counters in OVERFLOW");
    }
  }

  static
  counterType
  getObjectsCreatedCounter() noexcept
  {
    std::lock_guard<std::mutex> lg(mtx_);
    return objectsCreated_;
  }

  static
  counterType
  getObjectsAliveCounter() noexcept
  {
    std::lock_guard<std::mutex> lg(mtx_);
    return objectsAlive_;
  }

  static
  counterType
  getObjectsDestroyedCounter() noexcept
  {
    std::lock_guard<std::mutex> lg(mtx_);
    return objectsDestroyed_;
  }

  static
  bool
  getTooManyDestructionsFlag() noexcept
  {
    std::lock_guard<std::mutex> lg(mtx_);
    return tooManyDestructions_;
  }

  static
  auto
  getCounterStatus() noexcept -> counterStatus
  {
    std::lock_guard<std::mutex> lg(mtx_);
    return std::make_tuple(objectsCreated_, objectsAlive_, objectsDestroyed_, tooManyDestructions_);
  }

 protected:
   // in a multithreaded process threads can allocate objects of the same class,
   // so static data must be protected
  static std::mutex mtx_;
  static counterType objectsCreated_;
  static counterType objectsAlive_;
  static counterType objectsDestroyed_;
  static bool tooManyDestructions_;

  static constexpr
  inline
  bool
  checkCounterOverflow() noexcept
  {
    return ( (0 == objectsCreated_) || (0 == objectsAlive_) );
  }

  // objects should never be removed through pointers of this type
  ~objectCounter()
  {
    std::lock_guard<std::mutex> lg(mtx_);
    if ( (0 == objectsAlive_) ||
         (objectsCreated_ != (objectsAlive_ + objectsDestroyed_)) )
    {
      tooManyDestructions_ = true;
    }
    else
    {
      --objectsAlive_;
      ++objectsDestroyed_;
    }
  }
};  // struct objectCounter

template <typename T, typename TC>
std::mutex objectCounter<T, TC>::mtx_{};

template <typename T, typename TC>
TC objectCounter<T, TC>::objectsCreated_{0};

template <typename T, typename TC>
TC objectCounter<T, TC>::objectsAlive_{0};

template <typename T, typename TC>
TC objectCounter<T, TC>::objectsDestroyed_{0};

template <typename T, typename TC>
bool objectCounter<T, TC>::tooManyDestructions_{false};

}  // namespace object_factory::object_counter
#endif /* OBJECTFACTORY_H */
