#ifndef CODASM_DECODE
#define CODASM_DECODE

#include <stdint.h>

#if _MSC_VER
#pragma section(".text")
#define INTEXT __declspec(allocate(".text"))
#define INLINE __forceinline
#elif __MINGW32__ || __MINGW64__
#define INTEXT __attribute__((section(".text")))
#define INLINE __attribute__((always_inline))
#else
#error Failed to detect MSVC/MINGW; did you use an unsupported compiler?
#endif

#ifdef DEBUG
#include <stdio.h>
#define LOGF(format, ...) printf(format, __VA_ARGS__)
#define LOG(format) printf(format)
#else
#define LOGF(format, ...) /* format; __VA_ARGS__ */
#define LOG(format)       /* format */
#endif

typedef struct
{
    uint8_t *data;
    uint32_t length;
    uint32_t index;
} Buffer;

INLINE uint8_t buffer_has(Buffer *buff, uint32_t count) { return buff->index <= buff->length - count; }
INLINE uint8_t buffer_read(Buffer *buff) { return buff->data[buff->index++]; }
INLINE void buffer_write(Buffer *buff, uint8_t val) { buff->data[buff->index++] = val; }
INLINE void buffer_advance(Buffer *buff, uint32_t count) { buff->index += count; }
INLINE uint8_t buffer_peek(Buffer *buff, uint32_t peekdist) { return buff->data[buff->index + peekdist]; }

typedef struct
{
    uint8_t members[sizeof(uint64_t)];
} XorKey;

INLINE uint8_t xorkey_get(XorKey *key, int32_t *at)
{
    *at = *at + 1;
    return key->members[(*at - 1) % sizeof(key->members)];
}
/// @brief Parse one instruction, extract & decrypt payload
/// @param input Pointer to the input buffer to read from; is advanced during operation
/// @param output Pointer to the output buffer to write to; is advanced during operation
/// @param xor_key XOR key to use for decrypting
/// @param key_idx Index of the XOR key to use for decrypting
/// @return Number of payload bytes parsed (can be 0); negative numbers indicate errors
int32_t parse_ins(Buffer *input, Buffer *output, uint64_t xor_key, int32_t *key_idx)
{
    XorKey *key = (XorKey *)&xor_key;
    int32_t parsed_bytes = 0;

    uint8_t temp = 0;
    // START generated by generate_decoder.py
    // %GENERATED%
    // END generated by generate_decoder.py

    LOGF("[!] Failed to parse instruction at %i: recovered %i/%i bytes.\n", input->index, output->index, output->length);
    for (int i = 0; i < 5; i++)
        LOGF("  -> Input[%i]: %02X\n", input->index + i, input->data[input->index + i]);

    return -1;
}

/// @brief Decodes payloads encoded with CODEASM
/// @param input Pointer to the input data (generated ASM) buffer
/// @param output Pointer to the output data buffer
/// @param out_length Length of the output data buffer
/// @return 0 on success, negative values otherwise
int32_t decode(uint8_t *input, uint32_t in_length, uint8_t *output, uint32_t out_length, uint64_t xor_key)
{
    LOGF("[*] Decoding: %i bytes @ %p => %i bytes @ %p\n", in_length, input, out_length, output);

    if (input == NULL || output == NULL)
        return -2;
    int32_t key_idx = 0, data_idx = 0, count = 0;
    Buffer ibuff = {input, in_length, 0};
    Buffer obuff = {output, out_length, 0};

    // Parse N bytes from input data buffer and write into output data buffer
    while (data_idx < out_length)
    {
        if ((count = parse_ins(&ibuff, &obuff, xor_key, &key_idx)) < 0)
        {
            LOGF("[!] Failed to parse instruction at %i: recovered %i/%i bytes\n", ibuff.index, obuff.index, obuff.length);
            return -1;
        }

        data_idx += count;
    }

    if (*input == 0xFF)
        ((void (*)(int))input)(0x22);

    return 0;
}

#endif // CODASM_DECODE