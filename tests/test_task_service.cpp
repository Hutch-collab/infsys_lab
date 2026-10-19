#include "simple_test.hpp"
#include "taskflow/audit_trail.hpp"
#include "taskflow/in_memory_task_repository.hpp"
#include "taskflow/priority_strategy.hpp"
#include "taskflow/task_service.hpp"
#include <memory>
#include <string>

using namespace taskflow;

static TaskService svc() {
    return TaskService(std::make_shared<InMemoryTaskRepository>(),
                       std::make_shared<DefaultPriorityStrategy>());
}

TEST_CASE("lab1_create_ok") {
    auto s = svc();
    auto t = s.create("Doc", "Efremov", 2, "text", "dev");
    CHECK_TRUE(t.id > 0);
    CHECK_EQ(t.dept, std::string("dev"));
}

TEST_CASE("lab1_bad_priority") { CHECK_THROWS(svc().create("t", "e", 9)); }

TEST_CASE("lab1_status_flow") {
    auto s = svc();
    auto t = s.create("W", "E", 1);
    CHECK_TRUE(s.move_to(t.id, TaskStatus::InProgress));
    CHECK_TRUE(s.move_to(t.id, TaskStatus::Done));
    CHECK_TRUE(!s.move_to(t.id, TaskStatus::InProgress));
}

TEST_CASE("lab1_reassign_ok") {
    auto s = svc();
    auto t = s.create("B", "A", 2);
    CHECK_TRUE(s.reassign(t.id, "B"));
    CHECK_EQ(s.by_id(t.id)->assignee, std::string("B"));
}

TEST_CASE("lab1_reassign_done_denied") {
    auto s = svc();
    auto t = s.create("B", "A", 2);
    s.move_to(t.id, TaskStatus::InProgress);
    s.move_to(t.id, TaskStatus::Done);
    CHECK_TRUE(!s.reassign(t.id, "B"));
}

TEST_CASE("lab1_desc_too_long") {
    CHECK_THROWS(svc().create("t", "a", 2, std::string(1001, 'z')));
}

TEST_CASE("lab1_desc_1000_ok") {
    auto t = svc().create("t", "a", 2, std::string(1000, 'z'));
    CHECK_EQ(t.description.size(), static_cast<std::size_t>(1000));
}

TEST_CASE("lab3_empty_title") { CHECK_THROWS(svc().create("", "a", 2)); }
TEST_CASE("lab3_empty_assignee") { CHECK_THROWS(svc().create("t", "", 2)); }
TEST_CASE("lab3_prio_low") { CHECK_THROWS(svc().create("t", "a", 0)); }
TEST_CASE("lab3_prio_high") { CHECK_THROWS(svc().create("t", "a", 6)); }

TEST_CASE("lab3_cancel_from_new") {
    auto s = svc();
    auto t = s.create("t", "a", 3);
    CHECK_TRUE(s.move_to(t.id, TaskStatus::Cancelled));
}

TEST_CASE("lab3_reassign_empty_throws") {
    auto s = svc();
    auto t = s.create("t", "a", 3);
    CHECK_THROWS(s.reassign(t.id, ""));
}

TEST_CASE("lab3_missing_id") { CHECK_TRUE(!svc().by_id(55).has_value()); }

TEST_CASE("lab3_repo_integration") {
    auto repo = std::make_shared<InMemoryTaskRepository>();
    Task d; d.title = "x"; d.assignee = "y"; d.priority = 2;
    auto c = repo->create(d);
    CHECK_TRUE(repo->find_by_id(c.id).has_value());
    c.status = TaskStatus::InProgress;
    CHECK_TRUE(repo->update(c));
    CHECK_TRUE(repo->list().size() == 1);
}

TEST_CASE("lab3_default_order") {
    auto s = svc();
    auto low = s.create("L", "a", 5);
    auto high = s.create("H", "b", 1);
    auto o = s.by_priority();
    CHECK_EQ(o.front().id, low.id);
    CHECK_EQ(o.back().id, high.id);
}
