#pragma once
/**
 * XRTVM Compiler SDK — C++ Interface
 * Version: 1.0.0
 *
 * This header exposes the XRTVM compiler as a C-compatible static library
 * for integration into existing C++/CMake/MSBuild build pipelines.
 */

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Result codes returned by all XRTVM Compiler SDK functions.
 */
typedef enum XrtvmCompilerResult {
    XRTVM_OK                  = 0,
    XRTVM_ERROR_IO            = 1,
    XRTVM_ERROR_INVALID_INPUT = 2,
    XRTVM_ERROR_CRYPTO        = 3,
    XRTVM_ERROR_SIGNATURE     = 4,
    XRTVM_ERROR_OUT_OF_MEMORY = 5,
} XrtvmCompilerResult;

/**
 * Compiler configuration.
 */
typedef struct XrtvmCompilerConfig {
    /** Path to the input PE executable (UTF-8). */
    const char* input_path;
    /** Path for the output XVM container (UTF-8). */
    const char* output_path;
    /** 32-byte Ed25519 signing key. */
    const uint8_t* signing_key;
    /** 32-byte AES-256-GCM encryption key. */
    const uint8_t* encryption_key;
    /** Security level: 0=Low, 1=Medium, 2=High, 3=Maximum. */
    uint8_t security_level;
} XrtvmCompilerConfig;

/**
 * Compiles a PE executable into an XVM container.
 *
 * @param config  Compilation configuration (must not be NULL).
 * @param error   Optional output buffer for error messages (may be NULL).
 * @param error_len Size of the error buffer.
 * @return XRTVM_OK on success, error code otherwise.
 */
XrtvmCompilerResult xrtvm_compile(
    const XrtvmCompilerConfig* config,
    char* error,
    size_t error_len
);

/**
 * Returns the XRTVM Compiler SDK version string.
 */
const char* xrtvm_compiler_version(void);

/**
 * Generates a new Ed25519 key pair.
 *
 * @param private_key_out  Output buffer for 32-byte private key (must be >= 32 bytes).
 * @param public_key_out   Output buffer for 32-byte public key (must be >= 32 bytes).
 * @return XRTVM_OK on success.
 */
XrtvmCompilerResult xrtvm_generate_keypair(
    uint8_t* private_key_out,
    uint8_t* public_key_out
);

#ifdef __cplusplus
} /* extern "C" */
#endif
