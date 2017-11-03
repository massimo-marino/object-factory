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
template <typename T>
struct objectCounter
{
 public:
  static int objectsCreated;
  static int objectsAlive;

  objectCounter()
  {
    ++objectsCreated;
    ++objectsAlive;
  }

  objectCounter(const objectCounter&)
  {
    ++objectsCreated;
    ++objectsAlive;
  }

 protected:
  // objects should never be removed through pointers of this type
  ~objectCounter()
  {
    --objectsAlive;
  }
};

template <typename T>
int objectCounter<T>::objectsCreated( 0 );

template <typename T>
int objectCounter<T>::objectsAlive( 0 );

}  // namespace object_factory::object_counter
#endif /* OBJECTFACTORY_H */
