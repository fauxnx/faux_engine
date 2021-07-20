#ifndef __NX_BACKEND_VK_H__
#define __NX_BACKEND_VK_H__

#include "faux_engine/render/backend.h"
#include "vulkan/vulkan.h"
#include <vector>
#include <optional>
namespace nx {

  class BackendVK : Backend {
  public:
    BackendVK();
    ~BackendVK();

    nx::Result init() override;
    nx::Result shutdown() override;
    nx::Result makeCurrent() override;

  
  private:
    VkInstance instance_;
    VkDebugUtilsMessengerEXT debugMessenger_;
    VkSurfaceKHR surface_;
    VkPhysicalDevice physicalDevice_;
    VkDevice device_;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;
    VkSwapchainKHR swapChain_;
    std::vector<VkImage> swapChainImages_;
    VkFormat swapChainImageFormat_;
    VkExtent2D swapChainExtent_;

    std::vector<const char*> extensions_;
    std::vector<const char*> deviceExtensions_;
    std::vector<const char*> layers_;

    nx::Result checkExtensions();
    nx::Result checkLayers();
    u32 rateDeviceSuitability(VkPhysicalDevice device);
    nx::Result checkDeviceExtensionSupport(VkPhysicalDevice device);

    struct QueueFamilyIndices {
      std::optional<u32> graphicsFamily;
      std::optional<u32> presentFamily;

      bool isComplete();
    };
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    };
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    nx::Result createVulkanInstance();
    nx::Result setupDebugMessenger();
    nx::Result createWindow();
    nx::Result createSurface();
    nx::Result pickPhysicalDevice();
    nx::Result createLogicalDevice();
    nx::Result createSwapChain();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance, 
      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
      const VkAllocationCallbacks* pAllocator, 
      VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(
      VkInstance instance,
      VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks* pAllocator);
  };

}

#endif