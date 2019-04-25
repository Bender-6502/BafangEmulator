#include "Bind.h"
#include <stdexcept>

#ifdef TESTS
#include <Thread.h>
#endif


namespace Core
{
  CBind::CBind()
  {}


  CBind::CBind(CBind&& rhs)
    : impl_(std::move(rhs.impl_))
  {}


  CBind& CBind::operator=(CBind&& rhs)
  {
    return CBind(std::forward<CBind>(rhs)).swap(*this);
  }


  CBind& CBind::swap(CBind& rhs)
  {
    std::swap(impl_, rhs.impl_);
    return rhs;
  }


  CBind::~CBind()
  {}


  void CBind::operator()()
  {
    if (impl_)
      impl_->run();
    else
      throw std::runtime_error("no state");
  }


#ifdef TESTS
  // Lets unit-test this beast
  void bind_func()
  {
    printf("Function bind\r\n");
  }
  void CBind::tests()
  {
    auto bind1 = CBind(bind_func);
    bind1();
    Core::ThisThread::sleep_for(500);

    auto bind2 = CBind([] { printf("Lambda bind\r\n"); });
    bind2();
    Core::ThisThread::sleep_for(500);

    struct foo { void operator()() { printf("Functor bind\r\n"); } };
    auto bind3 = CBind(foo());
    bind2();
    Core::ThisThread::sleep_for(500);

    struct bar
    {
      bar()
      {
        auto bind1 = CBind(&bar::member, this);
        bind1();
      }

      void member()
      {
        printf("Member bind\r\n");
      }
    }b;

    Core::ThisThread::sleep_for(2000);
  }
#endif
}
