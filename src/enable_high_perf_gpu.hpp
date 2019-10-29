#pragma once

#ifdef WIN32
#if defined(_MSC_VER)
#define GPU_EXPORT _declspec(dllexport)
#else
#define GPU_EXPORT __attribute__((dllexport))
#endif
#else
#define GPU_EXPORT
#endif

// Force activation of high performance GPU (mostly on laptop)

// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
GPU_EXPORT int NvOptimusEnablement = 1;

// http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
GPU_EXPORT int AmdPowerXpressRequestHighPerformance = 1;

