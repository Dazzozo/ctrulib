/**
 * @file dsp.h
 * @brief DSP Service to access the DSP processor commands (sound)
 *
 * The DSP has access to the Linear memory region, and to the DSP memory region if allowed in the exheader.
 */
#pragma once
#include <3ds/types.h>

typedef enum
{
	DSP_INTERRUPT_PIPE = 2
} DSP_InterruptType;


typedef enum
{
	DSP_PIPE_INPUT  = 0, ///< DSP to ARM
	DSP_PIPE_OUTPUT = 1  ///< ARM to DSP
} DSP_PipeDirection;

/**
 * @brief Initializes the dsp service.
 *
 * Call this before calling any DSP_* function.
 * @note This will also unload any previously loaded DSP binary.
 *       It is done this way since you have to provide your binary when the 3DS leaves sleep mode anyway.
 */
Result dspInit(void);

/**
 * @brief Closes the dsp service.
 * @note This will also unload the DSP binary.
 */
Result dspExit(void);

///Checks if a headphone is inserted.
Result DSP_GetHeadphoneStatus(bool* is_inserted);

/**
 * @brief Flushes the cache
 * @param address   Beginning of the memory range to flush, inside the Linear or DSP memory regions
 * @param size      Size of the memory range to flush
 *
 * Flushes the cache for the specified memory range and invalidates the cache
 */
Result DSP_FlushDataCache(const void* address, u32 size);

/**
 * @brief Invalidates the cache
 * @param address   Beginning of the memory range to invalidate, inside the Linear or DSP memory regions
 * @param size      Size of the memory range to flush
 *
 * Invalidates the cache for the specified memory range
 */
Result DSP_InvalidateDataCache(const void* address, u32 size);

///Retrieves the handle of the DSP semaphore
Result DSP_GetSemaphoreHandle(Handle* semaphore);

///Sets the DSP hardware semaphore value
Result DSP_SetSemaphore(u16 value);

///Masks the DSP hardware semaphore value
Result DSP_SetSemaphoreMask(u16 mask);

/**
 * @brief Loads a DSP binary and starts the DSP
 * @param component The program file address in memory
 * @param size      The size of the program
 * @param prog_mask DSP memory block related ? Default is 0xff.
 * @param data_mask DSP memory block related ? Default is 0xff.
 * @param is_loaded Indicates if the DSP was succesfully loaded.
 *
 * @note The binary must be signed (http://3dbrew.org/wiki/DSP_Binary)
 * @note Seems to be called when the 3ds leaves the Sleep mode
 */
Result DSP_LoadComponent(const void* component, u32 size, u16 prog_mask, u16 data_mask, bool* is_loaded);

///Stops the DSP by unloading the binary
Result DSP_UnloadComponent(void);

/**
 * @brief Registers an event handle with the DSP through IPC
 * @param interrut The type of interrupt that will trigger the event. Usual value is DSP_INTERRUPT_PIPE.
 * @param channel The pipe channel. Usual value is 2
 *
 * @note It is possible that interrupt are inverted
 */
Result DSP_RegisterInterruptEvents(Handle handle, u32 interrupt, u32 channel);

/**
 * @param channel     unknown. Usually 2
 * @param peer        unknown. Usually 0
 * @param buffer      The buffer that will store the values read from the pipe
 * @param length      Length of the buffer
 * @param length_read Number of bytes read by the command
 */
Result DSP_ReadPipeIfPossible(u32 channel, u32 peer, void* buffer, u16 length, u16* length_read);

/**
 * @param channel unknown. Usually 2
 * @param buffer  The message to send to the DSP process
 * @param length  Length of the message
 */
Result DSP_WriteProcessPipe(u32 channel, const void* buffer, u32 length);

///Converts a DSP memory to a virtual address usable by the process
Result DSP_ConvertProcessAddressFromDspDram(u32 dsp_address, u32* arm_address);

/**
 * @brief Reads a DSP register
 * @param regNo Offset of the hardware register, base address is 0x1EC40000
 */
Result DSP_RecvData(u16 regNo, u16* value);

/**
 * @brief Checks if you can read a DSP register
 * @param regNo Offset of the hardware register, base address is 0x1EC40000
 *
 * @warning This call might hang if the data is not ready. See @ref DSP_SendDataIsEmpty.
 */
Result DSP_RecvDataIsReady(u16 regNo, bool* is_ready);

/**
 * @brief Writes to a DSP register
 * @param regNo Offset of the hardware register, base address is 0x1EC40000
 *
 * @warning This call might hang if the SendData is not empty. See @ref DSP_SendDataIsEmpty.
 */
Result DSP_SendData(u16 regNo, u16 value);

/**
 * @brief Checks if you can write to a DSP register ?
 * @param regNo Offset of the hardware register, base address is 0x1EC40000
 */
Result DSP_SendDataIsEmpty(u16 regNo, bool* is_empty);