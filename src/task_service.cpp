#include "taskflow/task_service.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace taskflow {

std::string to_string(TaskStatus status) {
    switch (status) {
        case TaskStatus::New: return "New";
        case TaskStatus::InProgress: return "InProgress";
        case TaskStatus::Done: return "Done";
        case TaskStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

TaskStatus status_from_string(const std::string& value) {
    if (value == "New") return TaskStatus::New;
    if (value == "InProgress") return TaskStatus::InProgress;
    if (value == "Done") return TaskStatus::Done;
    if (value == "Cancelled") return TaskStatus::Cancelled;
    throw std::invalid_argument("unknown status");
}

TaskService::TaskService(TaskRepositoryPtr store, PriorityStrategyPtr ranking)
    : store_(std::move(store)), ranking_(std::move(ranking)) {
    if (!store_ || !ranking_) throw std::invalid_argument("missing deps");
}

void TaskService::set_audit(AuditFn fn) { audit_ = std::move(fn); }

Task TaskService::create(std::string title, std::string assignee, int priority,
                         std::string description, std::string dept) {
    guard_input(title, assignee, priority, description);
    Task draft;
    draft.title = std::move(title);
    draft.assignee = std::move(assignee);
    draft.priority = priority;
    draft.description = std::move(description);
    draft.dept = std::move(dept);
    draft.status = TaskStatus::New;
    auto made = store_->create(std::move(draft));
    if (audit_) audit_("create", made.id);
    return made;
}

bool TaskService::reassign(std::int64_t id, std::string assignee) {
    if (assignee.empty()) throw std::invalid_argument("assignee empty");
    auto task = store_->find_by_id(id);
    if (!task || finished(task->status)) return false;
    task->assignee = std::move(assignee);
    const bool ok = store_->update(*task);
    if (ok && audit_) audit_("reassign", id);
    return ok;
}

bool TaskService::move_to(std::int64_t id, TaskStatus next) {
    auto task = store_->find_by_id(id);
    if (!task || !can_move(task->status, next)) return false;
    task->status = next;
    const bool ok = store_->update(*task);
    if (ok && audit_) audit_("move", id);
    return ok;
}

std::vector<Task> TaskService::by_priority() const {
    auto items = store_->list();
    std::sort(items.begin(), items.end(), [&](const Task& a, const Task& b) {
        const int sa = ranking_->score(a);
        const int sb = ranking_->score(b);
        return sa == sb ? a.id < b.id : sa < sb;
    });
    return items;
}

std::optional<Task> TaskService::by_id(std::int64_t id) const {
    return store_->find_by_id(id);
}

void TaskService::guard_input(const std::string& title, const std::string& assignee,
                              int priority, const std::string& description) const {
    if (title.empty()) throw std::invalid_argument("title empty");
    if (assignee.empty()) throw std::invalid_argument("assignee empty");
    if (priority < 1 || priority > 5) throw std::invalid_argument("priority 1..5");
    if (description.size() > 1000) throw std::invalid_argument("description too long");
}

bool TaskService::can_move(TaskStatus from, TaskStatus to) const {
    if (from == to) return true;
    if (finished(from)) return false;
    if (from == TaskStatus::New)
        return to == TaskStatus::InProgress || to == TaskStatus::Cancelled;
    if (from == TaskStatus::InProgress)
        return to == TaskStatus::Done || to == TaskStatus::Cancelled;
    return false;
}

} // namespace taskflow
