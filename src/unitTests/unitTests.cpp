#include "../objectFactory.h"
#include <future>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// BEGIN: ignore the warnings listed below when compiled with clang from here
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"

using namespace ::testing;
////////////////////////////////////////////////////////////////////////////////
// Tests
TEST (objectFactory, test_1)
{
  class A
  {
    mutable int _x{};
    mutable int _y{};
    mutable int _z{};

   public:
    explicit
    A() noexcept
    {
      std::cout << "default constructor A(): ";
      display_object();
    }

    explicit
    A(const int& x) noexcept
    :
    _x(x)
    {
      std::cout << "constructor-1 A(x): ";
      display_object();
    }

    explicit
    A(const int& x, const int& y) noexcept
    :
    _x(x),
    _y(y)
    {
      std::cout << "constructor-2 A(x, y): ";
      display_object();
    }

    explicit
    A(const int& x, const int& y, const int& z) noexcept
    :
    _x(x),
    _y(y),
    _z(z)
    {
      std::cout << "constructor-3 A(x, y, z): ";
      display_object();
    }

    int get_x() const noexcept
    {
      return _x;
    }
    int get_y() const noexcept
    {
      return _y;
    }
    int get_z() const noexcept
    {
      return _z;
    }

    void display_object (const std::string& prompt = "") const noexcept
    {
      std::cout << prompt
                << "(x, y, z): ("
                << get_x()
                << ", "
                << get_y()
                << ", "
                << get_z()
                << ")"
                << '\n';
    }

    A(const A& rhs) = delete;
    A& operator=(const A& rhs) = delete;

    ~A()
    {
      std::cout << "Destroyed object: ";
      display_object();
    }
  };  // class A

  using Object = std::unique_ptr<A>;
  
  object_factory::objectFactoryFun<A> objectFactoryFun {};

  objectFactoryFun = object_factory::createObjectFactoryFun<A>();
  Object o0 = objectFactoryFun();

  {
    objectFactoryFun = object_factory::createObjectFactoryFun<A, const int>(123);
  }
  Object o1 = objectFactoryFun();

  objectFactoryFun = object_factory::createObjectFactoryFun<A,const int>(456);
  Object o2 = objectFactoryFun();

  objectFactoryFun = object_factory::createObjectFactoryFun<A,const int, const int>(11, 22);
  Object o3 = objectFactoryFun();

  objectFactoryFun = object_factory::createObjectFactoryFun<A,const int, const int, const int>(11, 22, 33);
  Object o4 = objectFactoryFun();

  ASSERT_EQ(o0.get()->get_x(), 0);
  ASSERT_EQ(o0.get()->get_y(), 0);
  ASSERT_EQ(o0.get()->get_z(), 0);
  ASSERT_EQ(o1.get()->get_x(), 123);
  ASSERT_EQ(o1.get()->get_y(), 0);
  ASSERT_EQ(o1.get()->get_z(), 0);
  ASSERT_EQ(o2.get()->get_x(), 456);
  ASSERT_EQ(o2.get()->get_y(), 0);
  ASSERT_EQ(o2.get()->get_z(), 0);
  ASSERT_EQ(o3.get()->get_x(), 11);
  ASSERT_EQ(o3.get()->get_y(), 22);
  ASSERT_EQ(o3.get()->get_z(), 0);
  ASSERT_EQ(o4.get()->get_x(), 11);
  ASSERT_EQ(o4.get()->get_y(), 22);
  ASSERT_EQ(o4.get()->get_z(), 33);

  {
    objectFactoryFun = object_factory::createObjectFactoryFun<A,
                                                              const int,
                                                              const int,
                                                              const int>
                                                              (99, 88, 77);

    std::vector<Object> v {};
    for (int i = 1; i <= 5; ++i)
    {
      // create an object that can be used in this scope only
      Object o = objectFactoryFun();
      // store the object in the vector v using move semantics;
      // the lifetime of the object is extended over this scope
      v.push_back(std::move(o));
    }
    // all elements in the vector have the same attribute values
    for (auto&& item : v)
    {
      ASSERT_EQ(item.get()->get_x(), 99);
      ASSERT_EQ(item.get()->get_y(), 88);
      ASSERT_EQ(item.get()->get_z(), 77);
    }
  }  // the objects in the vector v are destroyed, going out of scope
}

TEST (objectFactory, test_2)
{
  // class A contains an object counter
  class A : public object_factory::object_counter::objectCounter<A>
  {
    mutable int _x{};
    mutable int _y{};
    mutable int _z{};

   public:
    explicit
    A() noexcept
    {
      std::cout << "default constructor A(): ";
      display_object();
    }

    explicit
    A(const int& x) noexcept
    :
    _x(x)
    {
      std::cout << "constructor-1 A(x): ";
      display_object();
    }

    explicit
    A(const int& x, const int& y) noexcept
    :
    _x(x),
    _y(y)
    {
      std::cout << "constructor-2 A(x, y): ";
      display_object();
    }

    explicit
    A(const int& x, const int& y, const int& z) noexcept
    :
    _x(x),
    _y(y),
    _z(z)
    {
      std::cout << "constructor-3 A(x, y, z): ";
      display_object();
    }

    int get_x() const noexcept
    {
      return _x;
    }
    int get_y() const noexcept
    {
      return _y;
    }
    int get_z() const noexcept
    {
      return _z;
    }

    void display_object (const std::string& prompt = "") const noexcept
    {
      std::cout << prompt
                << "(x, y, z): ("
                << get_x()
                << ", "
                << get_y()
                << ", "
                << get_z()
                << ")"
                << '\n';
    }

    A(const A& rhs) = delete;
    A& operator=(const A& rhs) = delete;

    ~A()
    {
      std::cout << "Destroyed object: ";
      display_object();
    }
  };  // class A

  ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::objectsAlive);
  ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::objectsCreated);

  {
    using Object = std::unique_ptr<A>;

    object_factory::objectFactoryFun<A> objectFactoryFun {};

    objectFactoryFun = object_factory::createObjectFactoryFun<A>();
    Object o0 = objectFactoryFun();

    {
      objectFactoryFun = object_factory::createObjectFactoryFun<A, const int>(123);
    }
    Object o1 = objectFactoryFun();

    objectFactoryFun = object_factory::createObjectFactoryFun<A,const int>(456);
    Object o2 = objectFactoryFun();

    objectFactoryFun = object_factory::createObjectFactoryFun<A,const int, const int>(11, 22);
    Object o3 = objectFactoryFun();

    objectFactoryFun = object_factory::createObjectFactoryFun<A,const int, const int, const int>(11, 22, 33);
    Object o4 = objectFactoryFun();

    ASSERT_EQ(5, object_factory::object_counter::objectCounter<A>::objectsAlive);
    ASSERT_EQ(5, object_factory::object_counter::objectCounter<A>::objectsCreated);

    ASSERT_EQ(o0.get()->get_x(), 0);
    ASSERT_EQ(o0.get()->get_y(), 0);
    ASSERT_EQ(o0.get()->get_z(), 0);
    ASSERT_EQ(o1.get()->get_x(), 123);
    ASSERT_EQ(o1.get()->get_y(), 0);
    ASSERT_EQ(o1.get()->get_z(), 0);
    ASSERT_EQ(o2.get()->get_x(), 456);
    ASSERT_EQ(o2.get()->get_y(), 0);
    ASSERT_EQ(o2.get()->get_z(), 0);
    ASSERT_EQ(o3.get()->get_x(), 11);
    ASSERT_EQ(o3.get()->get_y(), 22);
    ASSERT_EQ(o3.get()->get_z(), 0);
    ASSERT_EQ(o4.get()->get_x(), 11);
    ASSERT_EQ(o4.get()->get_y(), 22);
    ASSERT_EQ(o4.get()->get_z(), 33);

    {
      objectFactoryFun = object_factory::createObjectFactoryFun<A,
                                                                const int,
                                                                const int,
                                                                const int>
                                                                (99, 88, 77);

      std::vector<Object> v {};
      for (int i = 1; i <= 5; ++i)
      {
        // create an object that can be used in this scope only
        Object o = objectFactoryFun();
        // store the object in the vector v using move semantics;
        // the lifetime of the object is extended over this scope
        v.push_back(std::move(o));
      }

      ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::objectsAlive);
      ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::objectsCreated);

      // all elements in the vector have the same attribute values
      for (auto&& item : v)
      {
        ASSERT_EQ(item.get()->get_x(), 99);
        ASSERT_EQ(item.get()->get_y(), 88);
        ASSERT_EQ(item.get()->get_z(), 77);
      }
    }  // the objects in the vector v are destroyed, going out of scope

    ASSERT_EQ(5, object_factory::object_counter::objectCounter<A>::objectsAlive);
    ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::objectsCreated);
  }

  ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::objectsAlive);
  ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::objectsCreated);
}

#pragma clang diagnostic pop
// END: ignore the warnings when compiled with clang up to here

//int main(int argc, char **argv) {
//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
//}
