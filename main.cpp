#include <iostream>
#include <string>

#include <vulkan/vulkan.h>

#include <JobSystem.h>

// std::mutex s_mutex;
//
// struct TestJob : Job
// {
// public:
//     explicit TestJob(size_t i, int name) : counter(i), name(name) {};
//
//     CompletionPolicy Execute() override
//     {
//         for (int i = 0; i < counter; i++)
//         {
//             {
//                 std::lock_guard<std::mutex> lock(s_mutex);
//                 std::cout << name << " called " << i << std::endl;
//
//             }
//         }
//         return CompletionPolicy::Complete;
//     }
//
// private:
//     size_t counter;
//     int name;
// };

int main(void)
{
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkResult result = VK_SUCCESS;

    std::cout << "Initialize vulkan instance" << std::endl;
    VkInstanceCreateInfo infoInstance = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,

    };

    result = vkCreateInstance(&infoInstance, nullptr, &instance);
    std::cout << "Returns " << result << std::endl;

    // int i = 1, j = 2, k = 3;
    // JobSystem::GetInstance().Schedule<TestJob>(i, i);
    // JobSystem::GetInstance().Schedule<TestJob>(j, j);
    // JobSystem::GetInstance().ScheduleImportant<TestJob>(k, k);

    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
