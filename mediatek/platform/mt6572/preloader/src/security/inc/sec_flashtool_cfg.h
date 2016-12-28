
#ifndef SEC_FLASHTOOL_CFG_H
#define SEC_FLASHTOOL_CFG_H

#define FLASHTOOL_CFG_MAGIC             (0x544F4F4C)

typedef struct {
    unsigned char                       m_img_name [16];
    unsigned int                        m_img_offset;    
    unsigned int                        m_img_length;
} BYPASS_CHECK_IMAGE_T;

#define FLASHTOOL_CFG_SIZE              (76)
typedef struct {
    unsigned int                        m_magic_num;
    BYPASS_CHECK_IMAGE_T                m_bypass_check_img [3];
} FLASHTOOL_SECCFG_T;


#define FLASHTOOL_NON_SLA_FORBID_MAGIC             (0x544F4F4D)

typedef struct {
    unsigned char                       m_img_name [16];
    unsigned int                        m_img_offset;    
    unsigned int                        m_img_length;
} FORBID_DOWNLOAD_IMAGE_T;

#define FLASHTOOL_NON_SLA_FORBID_CFG_SIZE              (52)
typedef struct {
    unsigned int                        m_forbid_magic_num;
    FORBID_DOWNLOAD_IMAGE_T             m_forbid_dl_nsla_img [2];
} FLASHTOOL_FORBID_DOWNLOAD_NSLA_T;

#endif /* SEC_FLASHTOOL_CFG_H */

