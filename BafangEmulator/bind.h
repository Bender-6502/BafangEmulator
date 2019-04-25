#pragma once
#include <memory>
#include <utility>
#include <stdexcept>


namespace core
{
  struct bind_base
  {
    virtual ~bind_base() {}
    virtual void invoke() = 0;
  };


  template<class Func>
  struct bind_impl : bind_base
  {
    bind_impl(Func&& func)
      : func_(std::forward<Func>(func))
    {}

    virtual ~bind_impl()
    {}

    virtual void invoke()
    {
      func_();
    }

  protected:

    Func func_;
  };


  template<class Obj>
  struct bind_member_impl : bind_base
  {
    typedef void(Obj::*Func)();

    bind_member_impl(Func func, Obj* obj)
      : func_(func)
      , obj_(obj)
    {}

    virtual ~bind_member_impl()
    {}

    virtual void invoke()
    {
      (obj_->*func_)();
    }

  protected:

    Func func_;
    Obj* obj_;
  };


  class bind
  {
  public:

    template<class Func>
    explicit bind(Func&& func)
      : impl_(new bind_impl<Func>(std::forward<Func>(func)))
    {}

    template<class Obj>
    explicit bind(void(Obj::*func)(), Obj* obj)
      : impl_(new bind_member_impl<Obj>(func, obj))
    {}

    bind() = default;
    bind(bind&&) = default;
    bind(const bind&) = delete;
    bind& operator=(bind&&) = default;
    bind& operator=(const bind&) = delete;
    ~bind() = default;

    operator bool() const
    {
      return impl_ != nullptr;
    }

    void operator()()
    {
      if (impl_)
        impl_->invoke();
      else
        throw std::runtime_error("no state");
    }


  protected:

    std::unique_ptr<bind_base> impl_;
  };
}
