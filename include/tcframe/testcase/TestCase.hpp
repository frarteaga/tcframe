#pragma once

#include <functional>
#include <set>
#include <string>
#include <tuple>
#include <utility>

using std::function;
using std::move;
using std::set;
using std::string;
using std::tie;

namespace tcframe {

struct TestCase {
    friend class TestCaseBuilder;

private:
    string id_;
    string description_;
    set<int> subtaskIds_;
    function<void()> applier_;

public:
    const string& id() const {
        return id_;
    }

    const string& description() const {
        return description_;
    }

    const set<int>& subtaskIds() const {
        return subtaskIds_;
    }

    const function<void()> applier() const {
        return applier_;
    }

    bool operator==(const TestCase& o) const {
        return tie(id_, description_, subtaskIds_) == tie(o.id_, o.description_, o.subtaskIds_);
    }


};

class TestCaseBuilder {
private:
    TestCase subject_;

public:
    TestCaseBuilder& setId(string id) {
        subject_.id_ = id;
        return *this;
    }

    TestCaseBuilder& setDescription(string description) {
        subject_.description_ = description;
        return *this;
    }

    TestCaseBuilder& setSubtaskIds(set<int> subtaskIds) {
        subject_.subtaskIds_ = subtaskIds;
        return *this;
    }

    TestCaseBuilder& setApplier(function<void()> applier) {
        subject_.applier_ = applier;
        return *this;
    }

    TestCase build() {
        return move(subject_);
    }
};

}
