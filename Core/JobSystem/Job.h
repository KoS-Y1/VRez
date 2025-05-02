#pragma once

class JobSystem;

class Job
{
public:
    enum class CompletionPolicy
    {
        Complete,
        Reschedule,
        RescheduleImportant
    }

    // Disallow copy and move constructor
    Job(const Job &) = delete;
    Job(Job &&) = delete;

    // Disallow copy and move operator
    Job &operator=(const Job &) = delete;
    Job &operator=(Job &&) = delete;

    Job() = default;
    // Virtual destructor since we may delete an instance of a derived class through a pointer to the base class Job
    virtual ~Job() = default;

    // Return true by defualt, derived classes will override it
    virtual bool ReadyForExecution() { return true; }
    // Pure virtual function , derived classes must implement it
    virtual CompletionPolicy Execute() = 0;

protected:
    // Must take the returned result, otherwise give a warning
    [[nodiscard]] JobSystem *GetJobSystem() const { return jobSystem; }

private:
    JobSystem *jobSystem = nullptr;
    friend class JobSystem;
}