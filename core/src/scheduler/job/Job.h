// Copyright (C) 2019-2020 Zilliz. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under the License.
#pragma once

#include <condition_variable>
#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "db/SnapshotVisitor.h"
#include "db/snapshot/ResourceTypes.h"
#include "scheduler/interface/interfaces.h"
#include "scheduler/task/Task.h"
#include "server/context/Context.h"

namespace milvus {
namespace scheduler {

enum class JobType {
    INVALID = -1,
    SEARCH = 0,
    BUILD = 2,
};

using JobId = std::uint64_t;
using SegmentVisitorMap = std::unordered_map<engine::snapshot::ID_TYPE, engine::SegmentVisitorPtr>;
using JobTasks = std::vector<TaskPtr>;

class Job : public interface::dumpable {
 public:
    inline JobId
    id() const {
        return id_;
    }

    inline JobType
    type() const {
        return type_;
    }

    json
    Dump() const override;

    JobTasks
    CreateTasks();

    void
    TaskDone(Task* task);

    void
    WaitFinish();

    Status&
    status() {
        return status_;
    }

 protected:
    explicit Job(JobType type);

    virtual void
    OnCreateTasks(JobTasks& tasks) = 0;

 protected:
    Status status_;
    std::mutex mutex_;
    std::condition_variable cv_;

 private:
    JobId id_ = 0;
    JobType type_;

    JobTasks tasks_;
    bool tasks_created_ = false;
};

using JobPtr = std::shared_ptr<Job>;
using JobWPtr = std::weak_ptr<Job>;

}  // namespace scheduler
}  // namespace milvus
