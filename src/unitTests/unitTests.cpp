//
// unitTests.cpp
//
#include "../object-counter.h"
#include "../objectFactory.h"
#include <future>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// BEGIN: ignore the warnings listed below when compiled with clang from here
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"

using namespace ::testing;
////////////////////////////////////////////////////////////////////////////////
// Tests
TEST (objectFactory, test_1)
{
  class A final
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

    ~A()
    {
      std::cout << "Destroyed object: ";
      display_object();
    }
  };  // class A

  object_factory::objectFactoryFun<A> objectFactoryFun = object_factory::createObjectFactoryFun<A>();
  using Object = std::unique_ptr<A>;
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
    for (int i {1}; i <= 5; ++i)
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
  class A final : public object_factory::object_counter::objectCounter<A>
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

    ~A()
    {
      std::cout << "Destroyed object: ";
      display_object();
    }
  };  // class A

  ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::getObjectsAliveCounter());
  ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::getObjectsCreatedCounter());
  ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::getObjectsDestroyedCounter());
  ASSERT_EQ(false, object_factory::object_counter::objectCounter<A>::getTooManyDestructionsFlag());

  auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] =
  object_factory::object_counter::objectCounter<A>::getObjectCounters();
  ASSERT_EQ(0, objectsAlive);
  ASSERT_EQ(0, objectsCreated);
  ASSERT_EQ(0, objectsDestroyed);
  ASSERT_EQ(false, tooManyDestructions);

  {
    object_factory::objectFactoryFun<A> objectFactoryFun = object_factory::createObjectFactoryFun<A>();
    using Object = std::unique_ptr<A>;
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

    ASSERT_EQ(5, object_factory::object_counter::objectCounter<A>::getObjectsAliveCounter());
    ASSERT_EQ(5, object_factory::object_counter::objectCounter<A>::getObjectsCreatedCounter());
    ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::getObjectsDestroyedCounter());
    ASSERT_EQ(false, object_factory::object_counter::objectCounter<A>::getTooManyDestructionsFlag());

    auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] =
    object_factory::object_counter::objectCounter<A>::getObjectCounters();
    ASSERT_EQ(5, objectsAlive);
    ASSERT_EQ(5, objectsCreated);
    ASSERT_EQ(0, objectsDestroyed);
    ASSERT_EQ(false, tooManyDestructions);
  
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
      for (int i {1}; i <= 5; ++i)
      {
        // create an object that can be used in this scope only
        Object o = objectFactoryFun();
        // store the object in the vector v using move semantics;
        // the lifetime of the object is extended over this scope
        v.push_back(std::move(o));
      }

      ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::getObjectsAliveCounter());
      ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::getObjectsCreatedCounter());
      ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::getObjectsDestroyedCounter());
      ASSERT_EQ(false, object_factory::object_counter::objectCounter<A>::getTooManyDestructionsFlag());

      auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] =
      object_factory::object_counter::objectCounter<A>::getObjectCounters();
      ASSERT_EQ(10, objectsAlive);
      ASSERT_EQ(10, objectsCreated);
      ASSERT_EQ(0, objectsDestroyed);
      ASSERT_EQ(false, tooManyDestructions);

      // all elements in the vector have the same attribute values
      for (auto&& item : v)
      {
        ASSERT_EQ(item.get()->get_x(), 99);
        ASSERT_EQ(item.get()->get_y(), 88);
        ASSERT_EQ(item.get()->get_z(), 77);
      }
    }  // the objects in the vector v are destroyed, going out of scope

    ASSERT_EQ(5, object_factory::object_counter::objectCounter<A>::getObjectsAliveCounter());
    ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::getObjectsCreatedCounter());
    ASSERT_EQ(5, object_factory::object_counter::objectCounter<A>::getObjectsDestroyedCounter());
    ASSERT_EQ(false, object_factory::object_counter::objectCounter<A>::getTooManyDestructionsFlag());
    
    std::tie(objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions) =
            object_factory::object_counter::objectCounter<A>::getObjectCounters();
    ASSERT_EQ(5, objectsAlive);
    ASSERT_EQ(10, objectsCreated);
    ASSERT_EQ(5, objectsDestroyed);
    ASSERT_EQ(false, tooManyDestructions);
  }

  ASSERT_EQ(0, object_factory::object_counter::objectCounter<A>::getObjectsAliveCounter());
  ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::getObjectsCreatedCounter());
  ASSERT_EQ(10, object_factory::object_counter::objectCounter<A>::getObjectsDestroyedCounter());
  ASSERT_EQ(false, object_factory::object_counter::objectCounter<A>::getTooManyDestructionsFlag());

  std::tie(objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions) =
          object_factory::object_counter::objectCounter<A>::getObjectCounters();
  ASSERT_EQ(0, objectsAlive);
  ASSERT_EQ(10, objectsCreated);
  ASSERT_EQ(10, objectsDestroyed);
  ASSERT_EQ(false, tooManyDestructions);
}

// The way to get an overflow is to set the type of counters to an
// unsigned char or unsigned short
TEST (objectFactory, test_3_A)
{
  using namespace object_factory::object_counter;

  // class A contains an object counter; its counters type is unsigned short and
  // they overflow at 65535 + 1
  class A final : public objectCounter<A, unsigned short>
  {};

  {
    using Object = std::unique_ptr<A>;
    std::vector<Object> v {};
    object_factory::objectFactoryFun<A> objectFactoryFun = object_factory::createObjectFactoryFun<A>();

    auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] = A::getObjectCounters();

    // this loop generates overflow for unsigned short's or shorter types
    for (unsigned int i {1}; i <= 70'000; ++i)
    {
      // create an object that can be used in this scope only
      try
      {
        Object o = objectFactoryFun();
        // store the object in the vector v using move semantics;
        // the lifetime of the object is extended over this scope
        v.push_back(std::move(o));
        std::tie(objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions) = A::getObjectCounters();
        std::cout << i
                  << ": C="
                  << static_cast<long>(objectsCreated)
                  << " A="
                  << static_cast<long>(objectsAlive)
                  << " D="
                  << static_cast<long>(objectsDestroyed)
                  << " TMDs="
                  << std::boolalpha
                  << tooManyDestructions
                  << '\n';
      }
      catch ([[maybe_unused]] const std::overflow_error& e)
      {
        std::cout << ">>> Overflow in object counters occurred <<<\n";
        // counters are all zeroed because of the overflow, then they contain wrong data
        std::tie(objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions) = A::getObjectCounters();
        std::cout << i
                  << ": C="
                  << static_cast<long>(objectsCreated)
                  << " A="
                  << static_cast<long>(objectsAlive)
                  << " D="
                  << static_cast<long>(objectsDestroyed)
                  << " TMDs="
                  << std::boolalpha
                  << tooManyDestructions
                  << '\n';
        ASSERT_EQ(0, objectsCreated);
        ASSERT_EQ(0, objectsDestroyed);
        ASSERT_EQ(0, objectsAlive);
        ASSERT_EQ(false, tooManyDestructions);
        EXPECT_THROW(throw, std::overflow_error);
        return;
      }
    }
  }
  // here if no overflow occurred, but we never arrive here
  // code provided for completeness if a greater type would be used
  auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] = A::getObjectCounters();
  std::cout << "C="
            << static_cast<unsigned long>(objectsCreated)
            << " A="
            << static_cast<unsigned long>(objectsAlive)
            << " D="
            << static_cast<unsigned long>(objectsDestroyed)
            << " TMDs="
            << std::boolalpha
            << tooManyDestructions
            << '\n';
  ASSERT_EQ(70'000, objectsCreated);
  ASSERT_EQ(70'000, objectsDestroyed);
  ASSERT_EQ(0, objectsAlive);
  ASSERT_EQ(false, tooManyDestructions);
}

// The same as test_3_A but without overflow
TEST (objectFactory, test_3_B)
{
  using namespace object_factory::object_counter;

  // class A contains an object counter; its counters type is unsigned int
  class A final : public objectCounter<A, unsigned int>
  {};

  {
    using Object = std::unique_ptr<A>;
    std::vector<Object> v {};
    object_factory::objectFactoryFun<A> objectFactoryFun = object_factory::createObjectFactoryFun<A>();

    auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] = A::getObjectCounters();

    for (unsigned int i {1}; i <= 70'000; ++i)
    {
      // create an object that can be used in this scope only
      try
      {
        Object o = objectFactoryFun();
        // store the object in the vector v using move semantics;
        // the lifetime of the object is extended over this scope
        v.push_back(std::move(o));
        std::tie(objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions) = A::getObjectCounters();
        std::cout << i
                  << ": C="
                  << static_cast<long>(objectsCreated)
                  << " A="
                  << static_cast<long>(objectsAlive)
                  << " D="
                  << static_cast<long>(objectsDestroyed)
                  << " TMDs="
                  << std::boolalpha
                  << tooManyDestructions
                  << '\n';
      }
      // in this test no std::overflow_error is thrown, so the catch block is not executed
      catch ([[maybe_unused]] const std::overflow_error& e)
      {
        std::cout << ">>> Overflow in object counters occurred <<<\n";
        // counters are all zeroed because of the overflow, then they contain wrong data
        std::tie(objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions) = A::getObjectCounters();
        std::cout << i
                  << ": C="
                  << static_cast<long>(objectsCreated)
                  << " A="
                  << static_cast<long>(objectsAlive)
                  << " D="
                  << static_cast<long>(objectsDestroyed)
                  << " TMDs="
                  << std::boolalpha
                  << tooManyDestructions
                  << '\n';
        ASSERT_EQ(0, objectsCreated);
        ASSERT_EQ(0, objectsDestroyed);
        ASSERT_EQ(0, objectsAlive);
        ASSERT_EQ(false, tooManyDestructions);
        EXPECT_THROW(throw, std::overflow_error);
        return;
      }
    }
  }

  auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] = A::getObjectCounters();
  std::cout << "C="
            << static_cast<unsigned long>(objectsCreated)
            << " A="
            << static_cast<unsigned long>(objectsAlive)
            << " D="
            << static_cast<unsigned long>(objectsDestroyed)
            << " TMDs="
            << std::boolalpha
            << tooManyDestructions
            << '\n';
  ASSERT_EQ(70'000, objectsCreated);
  ASSERT_EQ(70'000, objectsDestroyed);
  ASSERT_EQ(0, objectsAlive);
  ASSERT_EQ(false, tooManyDestructions);
}

// test multithread support
TEST (objectFactory, test_4)
{
  using namespace object_factory::object_counter;

  std::clog << "This test takes a LONG time: be patient..."
            << std::endl;

  class A final : public objectCounter<A>
  {};

  // check initial state
  auto [objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions] = A::getObjectCounters();
  ASSERT_EQ(0, objectsAlive);
  ASSERT_EQ(0, objectsCreated);
  ASSERT_EQ(0, objectsDestroyed);
  ASSERT_EQ(false, tooManyDestructions);

  object_factory::objectFactoryFun<A> objectFactoryFun = object_factory::createObjectFactoryFun<A>();
  using Object = std::unique_ptr<A>;
  const unsigned long objectsToCreate {6'745'953};
  const unsigned int threadNumber {11};

  const
  auto
  threadFun = [&objectFactoryFun](const unsigned long objs)
  {
    unsigned long i;
    for (i = 1; i <= objs; ++i)
    {
      Object o = objectFactoryFun();
    }
    return i;
  };

  std::vector<std::future<unsigned long>> threadVector{};

  // tasks launched asynchronously
  for (unsigned int i {1}; i <= threadNumber; ++i)
  {
    threadVector.push_back(std::async(std::launch::async, threadFun, objectsToCreate));
  }

  // wait for all threads to be finished and process any exception
  try
  {
    for (auto&& item: threadVector)
    {
      ASSERT_EQ(item.get(), objectsToCreate + 1);
    }
  }
  catch( const std::exception& e )
  {
    std::clog << "EXCEPTION: "
              << e.what()
              << '\n';
  }

  // check final state
  std::tie(objectsCreated, objectsAlive, objectsDestroyed, tooManyDestructions)= A::getObjectCounters();
  ASSERT_EQ(0, objectsAlive);
  ASSERT_EQ(threadNumber * objectsToCreate, objectsCreated);
  ASSERT_EQ(threadNumber * objectsToCreate, objectsDestroyed);
  ASSERT_EQ(false, tooManyDestructions);
}

TEST (objectFactory, test_5)
{
  using namespace object_factory::object_counter;

  class A final : public objectCounter<A>
  {};
  class B final : public objectCounter<B>
  {};

  // check initial states
  auto [objectsCreated_A, objectsAlive_A, objectsDestroyed_A, tooManyDestructions_A] = A::getObjectCounters();
  ASSERT_EQ(0, objectsAlive_A);
  ASSERT_EQ(0, objectsCreated_A);
  ASSERT_EQ(0, objectsDestroyed_A);
  ASSERT_EQ(false, tooManyDestructions_A);

  auto [objectsCreated_B, objectsAlive_B, objectsDestroyed_B, tooManyDestructions_B] = B::getObjectCounters();
  ASSERT_EQ(0, objectsAlive_B);
  ASSERT_EQ(0, objectsCreated_B);
  ASSERT_EQ(0, objectsDestroyed_B);
  ASSERT_EQ(false, tooManyDestructions_B);

  object_factory::objectFactoryFun<A> A_objectFactoryFun = object_factory::createObjectFactoryFun<A>();
  using A_Object = std::unique_ptr<A>;
  object_factory::objectFactoryFun<B> B_objectFactoryFun = object_factory::createObjectFactoryFun<B>();
  using B_Object = std::unique_ptr<B>;

  {
    // objects created
    A_Object oA1 = A_objectFactoryFun();
    A_Object oA2 = A_objectFactoryFun();
    B_Object oB1 = B_objectFactoryFun();
  }  // objects destroyed leaving this scope

  // check final states
  std::tie(objectsCreated_A, objectsAlive_A, objectsDestroyed_A, tooManyDestructions_A)= A::getObjectCounters();
  ASSERT_EQ(0, objectsAlive_A);
  ASSERT_EQ(2, objectsCreated_A);
  ASSERT_EQ(2, objectsDestroyed_A);
  ASSERT_EQ(false, tooManyDestructions_A);

  std::tie(objectsCreated_B, objectsAlive_B, objectsDestroyed_B, tooManyDestructions_B)= B::getObjectCounters();
  ASSERT_EQ(0, objectsAlive_B);
  ASSERT_EQ(1, objectsCreated_B);
  ASSERT_EQ(1, objectsDestroyed_B);
  ASSERT_EQ(false, tooManyDestructions_B);
}

TEST (objectFactory, test_6)
{
  using namespace object_factory::object_counter;

  class A final : public objectCounter<A>
  {};

  // check initial states
  auto [objectsCreated_A, objectsAlive_A, objectsDestroyed_A, tooManyDestructions_A] = A::getObjectCounters();
  ASSERT_EQ(0, objectsAlive_A);
  ASSERT_EQ(0, objectsCreated_A);
  ASSERT_EQ(0, objectsDestroyed_A);
  ASSERT_EQ(false, tooManyDestructions_A);

  {
    A{};              // ctor(1), dtor(1)
    A a{};            // ctor(2)
    A b = a;          // ctor(3)
    a = b;            // copy assignment(1)
    b = std::move(a); // move assignment(1)
    A c = std::move(b); // move ctor(1)
    b = A{};          // ctor(4), dtor(2), move assignment(2)

    auto [objectsCreated_A, objectsAlive_A, objectsDestroyed_A, tooManyDestructions_A] = A::getObjectCounters();
    auto [copyConstructions_A, copyAssignments_A, moveConstructions_A, moveAssignments_A] = A::getCopyMoveCounters();

    std::cout << "objects created "
              << objectsCreated_A
              << " - objects alive "
              << objectsAlive_A
              << " - objects destroyed "
              << objectsDestroyed_A
              << " - too many destructions "
              << tooManyDestructions_A
              << " - copy constructions "
              << copyConstructions_A
              << " - copy assignments "
              << copyAssignments_A
              << " - move constructions "
              << moveConstructions_A
              << " - move assignments "
              << moveAssignments_A
              << "\n";
  }  // dtor(3), dtor(4)

  // check final states
  std::tie(objectsCreated_A, objectsAlive_A, objectsDestroyed_A, tooManyDestructions_A) = A::getObjectCounters();
  ASSERT_EQ(0, objectsAlive_A);
  ASSERT_EQ(5, objectsCreated_A);
  ASSERT_EQ(5, objectsDestroyed_A);
  ASSERT_EQ(false, tooManyDestructions_A);

  auto [copyConstructions_A, copyAssignments_A, moveConstructions_A, moveAssignments_A] = A::getCopyMoveCounters();

  std::cout << "objects created "
            << objectsCreated_A
            << " - objects alive "
            << objectsAlive_A
            << " - objects destroyed "
            << objectsDestroyed_A
            << " - too many destructions "
            << tooManyDestructions_A
            << " - copy constructions "
            << copyConstructions_A
            << " - copy assignments "
            << copyAssignments_A
            << " - move constructions "
            << moveConstructions_A
            << " - move assignments "
            << moveAssignments_A
            << "\n";

  ASSERT_EQ(copyConstructions_A, A::getCopyConstructionsCounter());
  ASSERT_EQ(copyAssignments_A, A::getCopyAssignmentsCounter());
  ASSERT_EQ(moveConstructions_A, A::getMoveConstructionsCounter());
  ASSERT_EQ(moveAssignments_A, A::getMoveAssignmentsCounter());
}

#pragma clang diagnostic pop
// END: ignore the warnings when compiled with clang up to here
