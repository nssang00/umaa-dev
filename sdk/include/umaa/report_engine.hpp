// Prototype of UMAA EO ICD 6.0 5.2.1 report-by-subscription semantics.
// Payload and DDS TypeSupport are supplied by vendor bindings (future work).
#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace umaa {
template <class T> struct ReportSample {
  std::string source;
  std::int64_t timestamp;
  T data;
};
template <class T> struct ReportHandler {
  virtual ~ReportHandler() = default;
  virtual T read_report() const = 0;
};
template <class T> class ReportProvider {
public:
  ReportProvider(std::string source, const ReportHandler<T>& handler)
      : source_(std::move(source)), handler_(handler) {}
  ReportSample<T> publish(std::int64_t timestamp) const {
    return {source_, timestamp, handler_.read_report()};
  }
  const std::string& source() const { return source_; }
private:
  std::string source_;
  const ReportHandler<T>& handler_;
};
template <class T> class ReportCache {
public:
  // Samples from a restarted provider must not overwrite newer reports.
  bool receive(const ReportSample<T>& report) {
    auto it = latest_.find(report.source);
    if (it != latest_.end() && it->second.timestamp >= report.timestamp) return false;
    latest_[report.source] = report;
    return true;
  }
  bool dispose(const std::string& source) { return latest_.erase(source) != 0; }
  std::optional<ReportSample<T>> latest(const std::string& source) const {
    auto it = latest_.find(source);
    if (it == latest_.end()) return std::nullopt;
    return it->second;
  }
  std::size_t size() const { return latest_.size(); }
private:
  std::unordered_map<std::string, ReportSample<T>> latest_;
};
}  // namespace umaa
