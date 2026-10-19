#pragma once
#include "taskflow/priority_strategy.hpp"
#include "taskflow/repository.hpp"
#include <functional>
#include <string>
#include <vector>

namespace taskflow {

using AuditFn = std::function<void(const std::string& action, std::int64_t id)>;

class TaskService {
public:
    TaskService(TaskRepositoryPtr store, PriorityStrategyPtr ranking);

    Task create(std::string title, std::string assignee, int priority,
                std::string description = "", std::string dept = "dev");
    bool reassign(std::int64_t id, std::string assignee); // ЛР1 assign
    bool move_to(std::int64_t id, TaskStatus next);
    std::vector<Task> by_priority() const;
    std::optional<Task> by_id(std::int64_t id) const;

    void set_audit(AuditFn fn); // ЛР2 Observer

private:
    void guard_input(const std::string& title, const std::string& assignee,
                     int priority, const std::string& description) const;
    bool can_move(TaskStatus from, TaskStatus to) const;

    TaskRepositoryPtr store_;
    PriorityStrategyPtr ranking_;
    AuditFn audit_;
};

} // namespace taskflow
