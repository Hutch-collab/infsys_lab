#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace taskflow {

// ЛР2: журнал пар (действие, id)
class AuditTrail {
public:
    void write(const std::string& action, std::int64_t id) {
        rows_.emplace_back(action, id);
    }
    const std::vector<std::pair<std::string, std::int64_t>>& rows() const { return rows_; }
    std::size_t size() const { return rows_.size(); }

private:
    std::vector<std::pair<std::string, std::int64_t>> rows_;
};

} // namespace taskflow
