#pragma once
/**
 * XRTVM Runtime SDK — C++ Interface
 * Version: 1.0.0
 */

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum XrtvmRuntimeResult {
    XRTVM_RT_OK                   = 0,
    XRTVM_RT_ERROR_IO             = 1,
    XRTVM_RT_ERROR_INVALID_FORMAT = 2,
    XRTVM_RT_ERROR_BAD_SIGNATURE  = 3,
    XRTVM_RT_ERROR_INTEGRITY      = 4,
    XRTVM_RT_ERROR_LICENSE        = 5,
    XRTVM_RT_ERROR_POLICY         = 6,
    XRTVM_RT_ERROR_EXECUTION      = 7,
} XrtvmRuntimeResult;

typedef struct XrtvmRuntimeConfig {
    /** 32-byte Ed25519 public key for container verification. */
    const uint8_t* public_key;
    /** Allow execution in VM environments (0 = no). */
    uint8_t allow_vm;
    /** Allow execution in sandbox environments (0 = no). */
    uint8_t allow_sandbox;
    /** Developer mode (0 = disabled). */
    uint8_t developer_mode;
    /** Maximum allowed threat score (0–100). */
    uint32_t max_threat_score;
} XrtvmRuntimeConfig;

/**
 * Loads and executes an XVM container.
 *
 * @param container_path  Path to the .xvm file (UTF-8).
 * @param config          Runtime configuration (must not be NULL).
 * @param exit_code_out   Output variable for the container's exit code.
 * @param error           Optional error message buffer (may be NULL).
 * @param error_len       Size of the error buffer.
 * @return XRTVM_RT_OK on success.
 */
XrtvmRuntimeResult xrtvm_run(
    const char* container_path,
    const XrtvmRuntimeConfig* config,
    int32_t* exit_code_out,
    char* error,
    size_t error_len
);

/**
 * Returns the XRTVM Runtime SDK version string.
 */
const char* xrtvm_runtime_version(void);

/**
 * Validates a container without executing it.
 *
 * @return XRTVM_RT_OK if the container is valid.
 */
XrtvmRuntimeResult xrtvm_validate(
    const char* container_path,
    const XrtvmRuntimeConfig* config,
    char* error,
    size_t error_len
);

#ifdef __cplusplus
} /* extern "C" */
#endif
