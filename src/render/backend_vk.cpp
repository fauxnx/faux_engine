#include "faux_engine/render/backend_vk.h"
#include "faux_engine/core.h"

#include <iostream>
#include <set>
#include "easylogging++.h"

#include "internals.h"

#ifdef FAUXENGINE_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw/glfw3native.h"
#endif
// TODO: Linux

#ifdef DEBUG_
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

nx::BackendVK::BackendVK() {

  instance_ = VK_NULL_HANDLE;
  debugMessenger_ = VK_NULL_HANDLE;
  physicalDevice_ = VK_NULL_HANDLE;
  device_ = VK_NULL_HANDLE;
  graphicsQueue_ = VK_NULL_HANDLE;
  surface_ = VK_NULL_HANDLE;
  swapChain_ = VK_NULL_HANDLE;
  initialized_ = false;
}

nx::BackendVK::~BackendVK() { }

nx::Result nx::BackendVK::init() {

  createVulkanInstance();
  setupDebugMessenger();
  createWindow();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
  createGraphicsPipeline();

  NXCore.renderer_.currentBackend_ = (Backend*)&NXCore.renderer_.backendVK_;

  return nx::Result::Success;
}

nx::Result nx::BackendVK::shutdown() {

  if (initialized_) {
    for (auto imageView : swapChainImageViews_) {
      vkDestroyImageView(device_, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device_, swapChain_, nullptr);
    vkDestroyDevice(device_, nullptr);
    if (enableValidationLayers) {
      DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
    }
    vkDestroyInstance(instance_, nullptr);
  }

  return nx::Result::Success;
}

nx::Result nx::BackendVK::makeCurrent() {

  if (!initialized_) {
    init();
  }
  else {
    createWindow();
  }
  NXCore.settings_.backend_ = nx::Backend::Type::Vulkan;
  NXCore.renderer_.currentBackend_ = (Backend*)&NXCore.renderer_.backendVK_;
  return nx::Result::Success;
}

nx::Result nx::BackendVK::checkExtensions() {

  LOG(TRACE) << "Required extensions:";


  for (const char* extensionName : extensions_) {
    LOG(TRACE) << '\t' << extensionName;
  }

  u32 extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

  LOG(TRACE) << "Available extensions:";

  for (const auto& extension : availableExtensions) {
    LOG(TRACE) << '\t' << extension.extensionName;
  }

  LOG(TRACE) << "Checking for extensions:";

  for (const char* extensionName : extensions_) {
    bool found = false;

    for (const auto& extensionProperties : availableExtensions) {
      if (strcmp(extensionProperties.extensionName, extensionName) == 0) {
        found = true;
        LOG(TRACE) << '\t' << "Found: " << extensionName;
        break;
      }
    }
    if (!found) {
      LOG(FATAL) << '\t' << "Not found: " << extensionName;
      return nx::Result::Error;
    }
  }
  LOG(TRACE) << "All extensions found";
  return nx::Result::Success;
}

nx::Result nx::BackendVK::checkLayers() {

  u32 layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  LOG(TRACE) << "Required layers:";

  for (const char* layerName : layers_) {
    LOG(TRACE) << '\t' << layerName;
  }

  LOG(TRACE) << "Available layers:";

  for (const auto& layerProperties : availableLayers) {
    LOG(TRACE) << '\t' << layerProperties.layerName << '\t' << layerProperties.description;
  }

  LOG(TRACE) << "Checking for layers:";

  for (const char* layerName : layers_) {
    bool found = false;

    for (const auto& layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        found = true;
        LOG(TRACE) << '\t' << "Found: " << layerName;
        break;
      }
    }

    if (!found) {
      LOG(FATAL) << '\t' << "Not found: " << layerName;
      return nx::Result::Error;
    }
  }

  LOG(TRACE) << "All layers found";
  return nx::Result::Success;
}

void nx::BackendVK::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}

nx::Result nx::BackendVK::createVulkanInstance() {

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "faux_engine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "faux_engine";
  appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  u32 glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  

  for (u32 i = 0; i < glfwExtensionCount; i++) {
    extensions_.push_back(glfwExtensions[i]);
  }
  
  if (enableValidationLayers) {
    extensions_.push_back("VK_EXT_debug_utils");
  }

  if (checkExtensions() != nx::Result::Success) {
    return nx::Result::Error;
  }

  if (enableValidationLayers) {
    layers_.push_back("VK_LAYER_KHRONOS_validation");
  }

  if (checkLayers() != nx::Result::Success) {
    LOG(FATAL) << "Couldn't find all the requested layers"; // TODO: handle this error instead of aborting
    return nx::Result::Error;
  }

  createInfo.enabledExtensionCount = (u32)extensions_.size();
  createInfo.ppEnabledExtensionNames = extensions_.data();
  
  createInfo.enabledLayerCount = (u32)layers_.size();
  createInfo.ppEnabledLayerNames = layers_.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (enableValidationLayers) {
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
    LOG(FATAL) << "Failed to create instance"; // TODO: make it so that backends are flagged as unavailable if they couldn't be initialized
    return nx::Result::Error;
  }

  LOG(TRACE) << "Successfully created vulkan instance";
  return nx::Result::Success;
}

nx::Result nx::BackendVK::setupDebugMessenger() {

  if (!enableValidationLayers) return nx::Result::Success;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  populateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger_) != VK_SUCCESS) {
    LOG(ERROR) << "Failed to setup the debug messenger";
    return nx::Result::Error;
  }
  LOG(TRACE) << "Successfully setup the debug messenger";
  return nx::Result::Success;
}

nx::Result nx::BackendVK::createWindow() {

  NXInternal.destroyActiveWindow();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  NXInternal.windowVK_ = glfwCreateWindow(NXCore.window_.width(), NXCore.window_.height(), NXCore.window_.name().size() == 0 ? "faux_engine" : NXCore.window_.name().c_str(), nullptr, nullptr);
  NXInternal.currentWindow_ = NXInternal.windowVK_;
  glfwMakeContextCurrent(NXInternal.windowVK_);

  return nx::Result::Success;
}

nx::Result nx::BackendVK::createSurface() {

  if (glfwCreateWindowSurface(instance_, NXInternal.windowVK_, nullptr, &surface_) != VK_SUCCESS) {
    LOG(ERROR) << "Failed to create window surface";
    return nx::Result::Error;
  }

  LOG(TRACE) << "Succesfully created surface";
  return nx::Result::Success;
}

u32 nx::BackendVK::rateDeviceSuitability(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  int score = 0; // TODO: Check on the various properties and score accordingly

  // Discrete GPUs have a significant performance advantage
  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  // Maximum possible size of textures affects graphics quality
  score += deviceProperties.limits.maxImageDimension2D;

  // Application can't function without geometry shaders
  if (!deviceFeatures.geometryShader) {
    score = 0;
  }

  // Check for queue families
  QueueFamilyIndices indices = findQueueFamilies(device);
  if (!indices.isComplete()) {
    score = 0;
  }

  if (checkDeviceExtensionSupport(device) == nx::Result::Error) {
    score = 0;
  }
  else {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
      score = 0;
    }
  }

  LOG(TRACE) << "Found device " << deviceProperties.deviceName << " with a score of " << score;
  return score;
}

nx::Result nx::BackendVK::checkDeviceExtensionSupport(VkPhysicalDevice device) {

  u32 extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
  
  LOG(TRACE) << "Checking for device layers:";
  for (auto& extensionName : deviceExtensions_) {
    bool found = false;

    for (const auto& extensionProperties : availableExtensions) {
      if (strcmp(extensionProperties.extensionName, extensionName) == 0) {
        found = true;
        LOG(TRACE) << '\t' << "Found: " << extensionName;
        break;
      }
    }
    if (!found) {
      LOG(ERROR) << '\t' << "Not found: " << extensionName;
      return nx::Result::Error;
    }
  }
  LOG(TRACE) << "All device layers found";
  return nx::Result::Success;
}

bool nx::BackendVK::QueueFamilyIndices::isComplete() {
  return graphicsFamily.has_value() && presentFamily.has_value();
}

nx::BackendVK::QueueFamilyIndices nx::BackendVK::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;
  
  u32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);

    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }
    i++;
  }

  return indices;
}

nx::BackendVK::SwapChainSupportDetails nx::BackendVK::querySwapChainSupport(VkPhysicalDevice device) {
  
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

  u32 formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
  }

  u32 presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR nx::BackendVK::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR nx::BackendVK::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D nx::BackendVK::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  }
  else {
    int width, height;
    glfwGetFramebufferSize(NXInternal.windowVK_, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<u32>(width),
        static_cast<u32>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

nx::Result nx::BackendVK::pickPhysicalDevice() {

  u32 deviceCount = 0;
  vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
  
  if (deviceCount == 0) {
    LOG(FATAL) << "No GPU available";
    return nx::Result::Error;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

  LOG(TRACE) << "Looking for suitable devices";

  // Use an ordered map to automatically sort candidates by increasing score
  std::multimap<int, VkPhysicalDevice> candidates;

  for (const auto& device : devices) {
    int score = rateDeviceSuitability(device);
    candidates.insert(std::make_pair(score, device));
  }

  // Check if the best candidate is suitable at all
  if (candidates.rbegin()->first > 0) {
    physicalDevice_ = candidates.rbegin()->second;
  }
  else {
    LOG(FATAL) << "Unable to find a suitable GPU";
    return nx::Result::Error;
  }

  LOG(TRACE) << "Found a suitable GPU";
  return nx::Result::Success;
}

nx::Result nx::BackendVK::createLogicalDevice() {

  deviceExtensions_.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  QueueFamilyIndices indices = findQueueFamilies(physicalDevice_);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<u32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

  float queuePriority = 1.0f;
  for (u32 queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = (u32)queueCreateInfos.size();
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = (u32)deviceExtensions_.size();
  createInfo.ppEnabledExtensionNames = deviceExtensions_.data();

  createInfo.enabledLayerCount = (u32)layers_.size();
  createInfo.ppEnabledLayerNames = layers_.data();

  if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
    LOG(FATAL) << "Failed to create logical device";
    return nx::Result::Error;
  }

  vkGetDeviceQueue(device_, indices.graphicsFamily.value(), 0, &graphicsQueue_);
  vkGetDeviceQueue(device_, indices.presentFamily.value(), 0, &presentQueue_);

  LOG(TRACE) << "Successfully created logical device";
  return nx::Result::Success;
}

nx::Result nx::BackendVK::createSwapChain() {
  SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice_);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

  u32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface_;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = findQueueFamilies(physicalDevice_);
  u32 queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;
  VkResult res = vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapChain_);
  if (res != VK_SUCCESS) {
    LOG(ERROR) << "Failed to create swap chain";
    return nx::Result::Error;
  }

  vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, nullptr);
  swapChainImages_.resize(imageCount);
  vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, swapChainImages_.data());

  swapChainImageFormat_ = surfaceFormat.format;
  swapChainExtent_ = extent;

  LOG(INFO) << "Succesfully created swapchain";
  return nx::Result::Success;
}

nx::Result nx::BackendVK::createImageViews() {

  swapChainImageViews_.resize(swapChainImages_.size());

  for (size_t i = 0; i < swapChainImages_.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages_[i];

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainImageFormat_;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(device_, &createInfo, nullptr, &swapChainImageViews_[i]) != VK_SUCCESS) {
      LOG(ERROR) << "Failed to create image views";
      return nx::Result::Error;
    }
  }

  LOG(INFO) << "Succesfully created image views";
  return nx::Result::Success;
}

nx::Result nx::BackendVK::createGraphicsPipeline() {

  LOG(INFO) << "Succesfully created the graphics pipeline";
  return nx::Result::Success;
}

VKAPI_ATTR VkBool32 VKAPI_CALL nx::BackendVK::debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData) {

  switch (messageSeverity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    LOG(TRACE) << "validation layer: " << pCallbackData->pMessage;
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    LOG(INFO) << "validation layer: " << pCallbackData->pMessage;
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    LOG(WARNING) << "validation layer: " << pCallbackData->pMessage;
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    LOG(ERROR) << "validation layer: " << pCallbackData->pMessage;
    break;
  }


  return VK_FALSE;
}

VkResult nx::BackendVK::CreateDebugUtilsMessengerEXT(
  VkInstance instance, 
  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
  const VkAllocationCallbacks* pAllocator, 
  VkDebugUtilsMessengerEXT* pDebugMessenger) {

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void nx::BackendVK::DestroyDebugUtilsMessengerEXT(
  VkInstance instance, 
  VkDebugUtilsMessengerEXT debugMessenger, 
  const VkAllocationCallbacks* pAllocator) {

  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}