#pragma once
#include <cstdint>
#include <string>

namespace taskflow {

enum class TaskStatus { New, InProgress, Done, Cancelled };

struct Task {
    std::int64_t id{};
    std::string title;
    std::string assignee;
    TaskStatus status{TaskStatus::New};
    int priority{3};
    std::string description; // ЛР1 <= 1000
    std::string dept{"dev"}; // отдел: dev/qa/ops — для стратегии
};

inline bool finished(TaskStatus s) {
    return s == TaskStatus::Done || s == TaskStatus::Cancelled;
}

std::string to_string(TaskStatus status);
TaskStatus status_from_string(const std::string& value);

} // namespace taskflow
