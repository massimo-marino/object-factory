/* 
 * File:   objectFactory.h
 * Author: massimo
 *
 * Created on September 20, 2017, 1:15 PM
 */

#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include <memory>
////////////////////////////////////////////////////////////////////////////////
namespace object_factory
{
// create an object of type T and return a std::unique_ptr to it
template <typename T, typename... Args>
auto createUniquePtr(Args&&... args) -> std::unique_ptr<T>
{
  // since C++14
  return std::make_unique<T>(args...);
}

template <typename T>
using objectFactoryFun = std::function<std::unique_ptr<T>(void)>;

template <typename T, typename... Args>
auto createObjectFactoryFun(Args&&... args) noexcept -> objectFactoryFun<T>
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
// this type MUST be unsigned
using counterType = unsigned long;

template <typename T>
struct objectCounter
{
 public:
  using counterStatus = std::tuple<counterType, counterType, counterType, bool>;

  objectCounter() noexcept(false)
  {
    ++objectsCreated;
    ++objectsAlive;
    if ( checkCounterOverflow() )
    {
      throw std::overflow_error("Object Counters in OVERFLOW");
    }
  }

  objectCounter(const objectCounter&) noexcept(false)
  {
    ++objectsCreated;
    ++objectsAlive;
    if ( checkCounterOverflow() )
    {
      throw std::overflow_error("Object Counters in OVERFLOW");
    }
  }

  static constexpr
  auto
  getObjectsCreatedCounter() noexcept
  {
    return objectsCreated;
  }

  static constexpr
  auto
  getObjectsAliveCounter() noexcept
  {
    return objectsAlive;
  }

  static constexpr
  auto
  getObjectsDestroyedCounter() noexcept
  {
    return objectsDestroyed;
  }

  static constexpr
  auto
  getTooManyDestructionsFlag() noexcept
  {
    return tooManyDestructions;
  }

  static constexpr
  auto
  getCounterStatus() noexcept -> counterStatus
  {
    return std::make_tuple(objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions);
  }

 protected:
  static counterType objectsCreated;
  static counterType objectsAlive;
  static counterType objectsDestroyed;
  static bool tooManyDestructions;

  static constexpr
  inline bool
  checkCounterOverflow() noexcept
  {
    return ( (0 == objectsCreated) || (0 == objectsAlive) );
  }

  // objects should never be removed through pointers of this type
  ~objectCounter()
  {
    if ( (0 == objectsAlive) ||
         (objectsCreated != (objectsAlive + objectsDestroyed)) )
    {
      tooManyDestructions = true;
    }
    else
    {
      --objectsAlive;
      ++objectsDestroyed;
    }
  }
};

template <typename T>
counterType objectCounter<T>::objectsCreated(0);

template <typename T>
counterType objectCounter<T>::objectsAlive(0);

template <typename T>
counterType objectCounter<T>::objectsDestroyed(0);

template <typename T>
bool objectCounter<T>::tooManyDestructions(false);

}  // namespace object_factory::object_counter
#endif /* OBJECTFACTORY_H */
