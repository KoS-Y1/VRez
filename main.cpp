#include <iostream>

#include <vulkan/vulkan.h>

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

    result = vkCreateInstance(&infoInstance, NULL, &instance);
    std::cout << "Returns " << result << std::endl;

    return 0;
}