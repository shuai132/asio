//
// ssl/detail/shutdown_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_SSL_DETAIL_SHUTDOWN_OP_HPP
#define ASIO_SSL_DETAIL_SHUTDOWN_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "asio/detail/config.hpp"

#if !defined(ASIO_ENABLE_OLD_SSL)
# include "asio/detail/handler_alloc_helpers.hpp"
# include "asio/detail/handler_invoke_helpers.hpp"
# include "asio/ssl/detail/buffer_space.hpp"
# include "asio/ssl/detail/engine.hpp"
# include "asio/ssl/detail/transport.hpp"
#endif // !defined(ASIO_ENABLE_OLD_SSL)

#include "asio/detail/push_options.hpp"

namespace asio {
namespace ssl {
namespace detail {

#if !defined(ASIO_ENABLE_OLD_SSL)

template <typename Stream, typename ShutdownHandler>
class shutdown_op
{
public:
  shutdown_op(detail::engine& engine, detail::transport<Stream>& transport,
      detail::buffer_space& space, ShutdownHandler& handler)
    : engine_(engine),
      transport_(transport),
      space_(space),
      handler_(ASIO_MOVE_CAST(ShutdownHandler)(handler))
  {
  }

  void operator()(asio::error_code ec, int result, int start = 0)
  {
    switch (start)
    {
    case 1:
      do
      {
        result = engine_.shutdown(space_, ec);
        transport_.async(result, space_, ec, start, *this);
        return; default:;
      } while (result < 0);

      handler_(engine_.map_error_code(ec));
    }
  }

//private:
  detail::engine& engine_;
  detail::transport<Stream>& transport_;
  detail::buffer_space& space_;
  ShutdownHandler handler_;
};

template <typename Stream, typename ShutdownHandler>
inline void* asio_handler_allocate(std::size_t size,
    shutdown_op<Stream, ShutdownHandler>* this_handler)
{
  return asio_handler_alloc_helpers::allocate(
      size, this_handler->handler_);
}

template <typename Stream, typename ShutdownHandler>
inline void asio_handler_deallocate(void* pointer, std::size_t size,
    shutdown_op<Stream, ShutdownHandler>* this_handler)
{
  asio_handler_alloc_helpers::deallocate(
      pointer, size, this_handler->handler_);
}

template <typename Function, typename Stream, typename ShutdownHandler>
inline void asio_handler_invoke(const Function& function,
    shutdown_op<Stream, ShutdownHandler>* this_handler)
{
  asio_handler_invoke_helpers::invoke(
      function, this_handler->handler_);
}

#endif // !defined(ASIO_ENABLE_OLD_SSL)

} // namespace detail
} // namespace ssl
} // namespace asio

#include "asio/detail/pop_options.hpp"

#endif // ASIO_SSL_DETAIL_SHUTDOWN_OP_HPP