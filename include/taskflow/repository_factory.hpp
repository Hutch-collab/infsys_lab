#pragma once
#include "taskflow/in_memory_task_repository.hpp"
#include "taskflow/sqlite_task_repository.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace taskflow {

struct Settings {
    std::string provider{"memory"}; // memory | sqlite
    std::string sqlite_path{"efremov.db"};
};

inline TaskRepositoryPtr make_repository(const Settings& s) {
    if (s.provider == "memory") {
        return std::make_shared<InMemoryTaskRepository>();
    }
    if (s.provider == "sqlite") {
        return std::make_shared<SQLiteTaskRepository>(s.sqlite_path);
    }
    throw std::invalid_argument("bad provider: " + s.provider);
}

} // namespace taskflow
