#ifndef QSPI_H__
#define QSPI_H__

#include <stdint.h>

enum erase_len{
    ERASE_4KB   = 0,
    ERASE_64KB  = 1,
    ERASE_ALL   = 2
};

/* @brief Read from location in flash.
 *
 * @param *rx_buff  Pointer to allocated buffer where data read will be stored.
 * @param rx_len    Length of buffer in bytes. Must be a multiple of wordsize.
 * @param src       Word-aligned source address to start read from.
 * @return          0 on success, -1 if rx_len or src are not word-aligned,
 *                  -2 if `src` is at or past the end of memory, or if read would
 *                  run past the end of flash.
 *                 
 * @note Will block until QSPI peripheral is ready to read.
 */
int qspi_read(uint32_t *rx_buff, size_t rx_len, uint32_t src);

/* @brief Write to location in flash.
 * 
 * @param *tx_buff  Pointer to buffer containing data that will be written.
 * @param tx_len    Length of buffer in bytes. Must be a multiple of wordsize.
 * @param dst       Word-aligned destination address to start write at.
 * @return          0 on success, -1 if tx_len is not a multiple of wordsize, 
 *                  -2 if `dst` is at or past the end of memory, or if read would
 *                  run past the end of flash.
 *                  
 * @note Will block until QSPI peripheral is ready to read.
 */
int qspi_write(uint32_t *tx_buff, size_t tx_len, uint32_t dst);

/* @brief Erase page in flash.
 *
 * @param start     Word-aligned address to start erasing from.
 * @param len       How much to erase, can be one of ERASE_4KB, ERASE_64KB or ERASE_ALL.
 * @return          0 on success, -1 if `start` is not a word-aligned address.
 *
 * @note Will block until QSPI peripheral is ready to erase.
 * @note Does not currently provide any way to check when erasing is done.
 */
int qspi_erase(uint32_t start, enum erase_len len);


/* @brief Configure and activate QSPI peripheral 
 * @note Currently there is no way to change the configuration,
 *       this simply functions as a wrapper for some asm procedures. */
void qspi_init(void);

/* @brief Deactivate QSPI peripheral.
 * @note This is only really useful 
 *       if power consumption has to be minimized. */
void qspi_deactivate(void);


#endif /* QSPI_H__ */
