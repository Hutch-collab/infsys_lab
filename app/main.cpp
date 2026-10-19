#include "taskflow/audit_trail.hpp"
#include "taskflow/in_memory_task_repository.hpp"
#include "taskflow/kv_task_parser.hpp"
#include "taskflow/priority_strategy.hpp"
#include "taskflow/task_service.hpp"
#include <iostream>
#include <memory>

int main() {
    using namespace taskflow;

    auto store = std::make_shared<InMemoryTaskRepository>();
    auto rank = std::make_shared<WorkQueueStrategy>();
    TaskService svc(store, rank);

    AuditTrail trail;
    svc.set_audit([&](const std::string& action, std::int64_t id) {
        trail.write(action, id);
    });

    svc.create("Prepare plan", "Efremov", 2, "scope", "dev");
    auto t = svc.create("Implement API", "Efremov", 1, "http", "dev");
    svc.move_to(t.id, TaskStatus::InProgress);

    auto imported = parse_kv_line("title=Test suite;assignee=QA;priority=3;description=ctest;dept=qa");
    imported = store->create(imported);

    std::cout << "Efremov TaskFlow queue\n";
    for (const auto& x : svc.by_priority()) {
        std::cout << "[" << x.dept << "] #" << x.id << " " << x.title
                  << " @" << x.assignee << " " << to_string(x.status)
                  << " p=" << x.priority << '\n';
    }
    std::cout << "audit events: " << trail.size() << '\n';
    for (const auto& [action, id] : trail.rows()) {
        std::cout << "  " << action << " -> " << id << '\n';
    }
    std::cout << "kv import id=" << imported.id << '\n';
    return 0;
}
