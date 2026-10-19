#include "simple_test.hpp"
#include "taskflow/audit_trail.hpp"
#include "taskflow/in_memory_task_repository.hpp"
#include "taskflow/kv_task_parser.hpp"
#include "taskflow/priority_strategy.hpp"
#include "taskflow/repository_factory.hpp"
#include "taskflow/sqlite_task_repository.hpp"
#include "taskflow/task_service.hpp"
#include <memory>
#include <string>

using namespace taskflow;

TEST_CASE("lab2_factory_memory") {
    Settings s; s.provider = "memory";
    auto repo = make_repository(s);
    Task d; d.title = "a"; d.assignee = "b"; d.priority = 3;
    CHECK_TRUE(repo->find_by_id(repo->create(d).id).has_value());
}

TEST_CASE("lab2_closed_last") {
    auto repo = std::make_shared<InMemoryTaskRepository>();
    TaskService s(repo, std::make_shared<ClosedLastPriorityStrategy>());
    auto a = s.create("A", "u", 1);
    auto b = s.create("B", "u", 5);
    s.move_to(a.id, TaskStatus::InProgress);
    s.move_to(a.id, TaskStatus::Done);
    CHECK_EQ(s.by_priority().front().id, b.id);
}

TEST_CASE("lab2_factory_sqlite") {
    Settings s; s.provider = "sqlite"; s.sqlite_path = "efr.tmp.db";
    auto repo = make_repository(s);
    auto* p = dynamic_cast<SQLiteTaskRepository*>(repo.get());
    CHECK_TRUE(p != nullptr);
    CHECK_EQ(p->file(), std::string("efr.tmp.db"));
}

TEST_CASE("lab2_factory_bad_provider") {
    Settings s; s.provider = "mongo";
    CHECK_THROWS(make_repository(s));
}

TEST_CASE("lab2_work_queue_prefers_in_progress") {
    auto repo = std::make_shared<InMemoryTaskRepository>();
    TaskService s(repo, std::make_shared<WorkQueueStrategy>());
    auto waiting = s.create("wait", "u", 1, "", "dev");
    auto active = s.create("run", "u", 5, "", "dev");
    s.move_to(active.id, TaskStatus::InProgress);
    CHECK_EQ(s.by_priority().front().id, active.id);
    CHECK_EQ(s.by_priority().back().id, waiting.id);
}

TEST_CASE("lab2_audit_trail") {
    TaskService s(std::make_shared<InMemoryTaskRepository>(),
                  std::make_shared<DefaultPriorityStrategy>());
    AuditTrail trail;
    s.set_audit([&](const std::string& a, std::int64_t id) { trail.write(a, id); });
    auto t = s.create("X", "E", 2);
    CHECK_TRUE(trail.size() == 1);
    CHECK_EQ(trail.rows().front().first, std::string("create"));
    CHECK_EQ(trail.rows().front().second, t.id);
}

TEST_CASE("lab2_kv_parser") {
    auto t = parse_kv_line("title=Hi;assignee=E;priority=2;dept=qa");
    CHECK_EQ(t.title, std::string("Hi"));
    CHECK_EQ(t.assignee, std::string("E"));
    CHECK_EQ(t.priority, 2);
    CHECK_EQ(t.dept, std::string("qa"));
}

TEST_CASE("lab2_kv_bad") { CHECK_THROWS(parse_kv_line("title=only")); }

TEST_CASE("lab2_sqlite_stub") {
    SQLiteTaskRepository r("z.db");
    CHECK_THROWS(r.list());
}
