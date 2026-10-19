#pragma once
#include "taskflow/task.hpp"
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace taskflow {

// ЛР2: строка key=value;key=value
// пример: title=Fix;assignee=Efremov;priority=2;description=bug;dept=qa
inline Task parse_kv_line(const std::string& line) {
    std::unordered_map<std::string, std::string> kv;
    std::stringstream ss(line);
    std::string pair;
    while (std::getline(ss, pair, ';')) {
        auto eq = pair.find('=');
        if (eq == std::string::npos) continue;
        kv[pair.substr(0, eq)] = pair.substr(eq + 1);
    }
    if (!kv.count("title") || !kv.count("assignee") || !kv.count("priority")) {
        throw std::invalid_argument("kv line needs title,assignee,priority");
    }
    Task t;
    t.title = kv["title"];
    t.assignee = kv["assignee"];
    t.priority = std::stoi(kv["priority"]);
    if (kv.count("description")) t.description = kv["description"];
    if (kv.count("dept")) t.dept = kv["dept"];
    t.status = TaskStatus::New;
    return t;
}

} // namespace taskflow
