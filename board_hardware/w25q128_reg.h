#ifndef __W25Q128_REG_H
#define __W25Q128_REG_H

/**
 * @brief chip command definition
 */
#define W25QXX_COMMAND_WRITE_ENABLE                      0x06        /**< write enable */
#define W25QXX_COMMAND_VOLATILE_SR_WRITE_ENABLE          0x50        /**< sr write enable */
#define W25QXX_COMMAND_WRITE_DISABLE                     0x04        /**< write disable */
#define W25QXX_COMMAND_READ_STATUS_REG1                  0x05        /**< read status register-1 */
#define W25QXX_COMMAND_READ_STATUS_REG2                  0x35        /**< read status register-2 */
#define W25QXX_COMMAND_READ_STATUS_REG3                  0x15        /**< read status register-3 */
#define W25QXX_COMMAND_WRITE_STATUS_REG1                 0x01        /**< write status register-1 */
#define W25QXX_COMMAND_WRITE_STATUS_REG2                 0x31        /**< write status register-2 */
#define W25QXX_COMMAND_WRITE_STATUS_REG3                 0x11        /**< write status register-3 */
#define W25QXX_COMMAND_CHIP_ERASE                        0xC7        /**< chip erase */
#define W25QXX_COMMAND_ERASE_PROGRAM_SUSPEND             0x75        /**< erase suspend */
#define W25QXX_COMMAND_ERASE_PROGRAM_RESUME              0x7A        /**< erase resume */
#define W25QXX_COMMAND_POWER_DOWN                        0xB9        /**< power down */
#define W25QXX_COMMAND_RELEASE_POWER_DOWN                0xAB        /**< release power down */
#define W25QXX_COMMAND_READ_MANUFACTURER                 0x90        /**< manufacturer */
#define W25QXX_COMMAND_JEDEC_ID                          0x9F        /**< jedec id */
#define W25QXX_COMMAND_GLOBAL_BLOCK_SECTOR_LOCK          0x7E        /**< global block lock */
#define W25QXX_COMMAND_GLOBAL_BLOCK_SECTOR_UNLOCK        0x98        /**< global block unlock */
#define W25QXX_COMMAND_ENTER_QSPI_MODE                   0x38        /**< enter spi mode */
#define W25QXX_COMMAND_ENABLE_RESET                      0x66        /**< enable reset */
#define W25QXX_COMMAND_RESET_DEVICE                      0x99        /**< reset device */
#define W25QXX_COMMAND_READ_UNIQUE_ID                    0x4B        /**< read unique id */
#define W25QXX_COMMAND_PAGE_PROGRAM                      0x02        /**< page program */
#define W25QXX_COMMAND_QUAD_PAGE_PROGRAM                 0x32        /**< quad page program */
#define W25QXX_COMMAND_SECTOR_ERASE_4K                   0x20        /**< sector erase */
#define W25QXX_COMMAND_BLOCK_ERASE_32K                   0x52        /**< block erase */
#define W25QXX_COMMAND_BLOCK_ERASE_64K                   0xD8        /**< block erase */
#define W25QXX_COMMAND_READ_DATA                         0x03        /**< read data */
#define W25QXX_COMMAND_FAST_READ                         0x0B        /**< fast read */
#define W25QXX_COMMAND_FAST_READ_DUAL_OUTPUT             0x3B        /**< fast read dual output */
#define W25QXX_COMMAND_FAST_READ_QUAD_OUTPUT             0x6B        /**< fast read quad output */
#define W25QXX_COMMAND_READ_SFDP_REGISTER                0x5A        /**< read SFDP register */
#define W25QXX_COMMAND_ERASE_SECURITY_REGISTER           0x44        /**< erase security register */
#define W25QXX_COMMAND_PROGRAM_SECURITY_REGISTER         0x42        /**< program security register */
#define W25QXX_COMMAND_READ_SECURITY_REGISTER            0x48        /**< read security register */
#define W25QXX_COMMAND_INDIVIDUAL_BLOCK_LOCK             0x36        /**< individual block lock */
#define W25QXX_COMMAND_INDIVIDUAL_BLOCK_UNLOCK           0x39        /**< individual block unlock */
#define W25QXX_COMMAND_READ_BLOCK_LOCK                   0x3D        /**< read block lock */
#define W25QXX_COMMAND_FAST_READ_DUAL_IO                 0xBB        /**< fast read dual I/O */
#define W25QXX_COMMAND_DEVICE_ID_DUAL_IO                 0x92        /**< device id dual I/O */
#define W25QXX_COMMAND_SET_BURST_WITH_WRAP               0x77        /**< set burst with wrap */
#define W25QXX_COMMAND_FAST_READ_QUAD_IO                 0xEB        /**< fast read quad I/O */
#define W25QXX_COMMAND_WORD_READ_QUAD_IO                 0xE7        /**< word read quad I/O */
#define W25QXX_COMMAND_OCTAL_WORD_READ_QUAD_IO           0xE3        /**< octal word read quad I/O */
#define W25QXX_COMMAND_DEVICE_ID_QUAD_IO                 0x94        /**< device id quad I/O */

#endif // !__W25Q128_REG_H
