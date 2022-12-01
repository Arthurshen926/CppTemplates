#include "app_example.h"

AppExample::AppExample(const std::string& config_file) {
  // in this example, we do not need a config file.
  m_ = std::make_unique<ModuleExample>("Tom", 18);
}

void AppExample::feed_str(const std::string& str) {
  std::unique_lock lock(str_mtx);
  str_queue.emplace(str);
}

void AppExample::feed_int(int x) {
  std::unique_lock lock(int_mtx);
  int_queue.emplace(x);
}

void AppExample::process() {
  // check sensor data valid first
  // in this example, we need both string and int.
  if (str_queue.empty() || int_queue.empty()) {
    return;
  }
  // extract data
  std::string      str;
  std::unique_lock str_lock(str_mtx);
  str = std::move(str_queue.front());
  str_queue.pop();
  str_lock.unlock();

  int              x;
  std::unique_lock int_lock(int_mtx);
  x = std::move(int_queue.front());
  int_queue.pop();
  int_lock.unlock();

  // main process
  // in this example, we simply concatenate string and int.
  std::string rst = m_->do_work(str, x);

  // save result
  std::unique_lock rst_lock(rst_mtx);
  rst_queue.emplace(std::move(rst));
  rst_lock.unlock();
}

std::optional<std::string> AppExample::get_rst() {
  std::unique_lock rst_lock(rst_mtx);
  if (rst_queue.empty())
    return std::nullopt;
  std::string rst = std::move(rst_queue.front());
  rst_queue.pop();
  return std::move(rst);
}

std::string AppExample::service(const std::string& arg0, int arg1) {
  return arg0 + std::to_string(arg1);
}
