#include "taskflow/priority_strategy.hpp"

namespace taskflow {

int DefaultPriorityStrategy::score(const Task& task) const {
    return 10 - task.priority;
}

int ClosedLastPriorityStrategy::score(const Task& task) const {
    return finished(task.status) ? 1000 : 10 - task.priority;
}

int WorkQueueStrategy::score(const Task& task) const {
    if (finished(task.status)) return 9000;

    int base = 100;
    if (task.status == TaskStatus::InProgress) base = 0;
    else if (task.status == TaskStatus::New) base = 50;

    int dept_bonus = 0;
    if (task.dept == "qa") dept_bonus = -5; // qa чуть выше в очереди

    return base + (10 - task.priority) + dept_bonus;
}

} // namespace taskflow
