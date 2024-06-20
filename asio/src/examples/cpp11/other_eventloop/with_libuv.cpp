#include <asio.hpp>
#include <iostream>
#include <uv.h>

//#define USE_LOG
#ifdef USE_LOG
#include "log.h"
#else
#define LOG printf
#endif

int main() {
  uv_loop_t *loop = uv_default_loop();
  static asio::io_context io_context;
  asio::io_context::work work(io_context);

  uv_timer_t timer_req;
  uv_timer_init(loop, &timer_req);
  uv_timer_start(
      &timer_req, [](uv_timer_t *handle) { LOG("libuv Timer fired!\n"); },
      10000, 10000);

  asio::steady_timer asio_timer(io_context);
  std::function<void()> start_asio_timer;
  start_asio_timer = [&] {
    asio_timer.expires_after(asio::chrono::milliseconds(1000));
    asio_timer.async_wait([&](const asio::error_code &ec) {
      LOG("asio Timer fired!\n");
      start_asio_timer();
    });
  };
  start_asio_timer();

  static std::condition_variable async_done;
  std::mutex async_done_lock;
  uv_async_t async;
  uv_async_init(loop, &async, [](uv_async_t *handle) {
    io_context.poll_one();
    async_done.notify_one();
  });

  std::thread([&] {
    for (;;) {
      LOG("wait_event...");
      asio::error_code ec;
      auto &service = asio::use_service<asio::detail::scheduler>(io_context);
      service.wait_event(INTMAX_MAX, ec);
      uv_async_send(&async);
      std::unique_lock<std::mutex> lock(async_done_lock);
      async_done.wait(lock);
    }
  }).detach();

  uv_run(loop, UV_RUN_DEFAULT);
  return 0;
}
