#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/dmi.h>
#include <linux/version.h>
#include <linux/ctype.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c/pca954x.h>
#include <linux/i2c-mux.h>
#include <linux/i2c-mux-gpio.h>
#include <linux/i2c/sff-8436.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>

#define CPULD_ADDR        0x31
#define SWPLD2_ADDR       0x35
#define SWPLD3_ADDR       0x36
#define BUS0_BASE_NUM     10
#define BUS0_DEV_NUM      3
#define DEFAULT_NUM       0
#define EEPROM_VAL        0xfc
#define PCA9548_VAL       0xfd
#define QSFP_VAL          0xff
#define BUS0_MUX_REG      0x14
#define QSFP_PRESENCE_1   0x51
#define QSFP_PRESENCE_2   0x52
#define QSFP_PRESENCE_3   0x51
#define QSFP_PRESENCE_4   0x52
#define SFP_PRESENCE      0x71
#define QSFP_RESPONDE_1   0x31
#define QSFP_RESPONDE_2   0x32
#define QSFP_RESPONDE_3   0x31
#define QSFP_RESPONDE_4   0x32
#define QSFP_LP_MODE_1    0x21
#define QSFP_LP_MODE_2    0x22
#define QSFP_LP_MODE_3    0x21 
#define QSFP_LP_MODE_4    0x22
#define QSFP_RESET_1      0x11
#define QSFP_RESET_2      0x12
#define QSFP_RESET_3      0x11
#define QSFP_RESET_4      0x12
#define QSFP_INTERRUPT_1  0x61
#define QSFP_INTERRUPT_2  0x62
#define QSFP_INTERRUPT_3  0x61
#define QSFP_INTERRUPT_4  0x62

#define et_c032if_i2c_device_num(NUM){                                        \
        .name                   = "delta-et-c032if-i2c-device",               \
        .id                     = NUM,                                        \
        .dev                    = {                                           \
                    .platform_data = &et_c032if_i2c_device_platform_data[NUM],\
                    .release       = device_release,                          \
        },                                                                    \
}

struct mutex dni_lock;

/*Define struct to get client of i2c_new_deivce */
struct i2c_client * i2c_client_9548_1;
struct i2c_client * i2c_client_9548_2;
struct i2c_client * i2c_client_9548_3;
struct i2c_client * i2c_client_9548_4;
struct i2c_client * i2c_client_9548_5;
struct i2c_client * i2c_client_9548_6;

static struct kobject *kobj_cpld;
static struct kobject *kobj_swpld2;
static struct kobject *kobj_swpld3;

enum{
    BUS0 = 0,
    BUS1,
    BUS2,
    BUS3,
    BUS4,
    BUS5,
    BUS6,
    BUS7,
    BUS8,
    BUS9,
    BUS10,
    BUS11,
    BUS12,
    BUS13,
    BUS14,
    BUS15,
    BUS16,
    BUS17,
    BUS18,
    BUS19,
};

static struct cpld_attribute_data {
    uint8_t bus;
    uint8_t addr;
    uint8_t reg;
    uint8_t mask;
    char note[350];
};

unsigned char reverse_8bits(unsigned char c)
{
        unsigned char s = 0;
        int i;
        for (i = 0; i < 8; ++i) {
                s <<= 1;
                s |= c & 1;
                c >>= 1;
        }
        return s;
}

unsigned char dni_log2 (unsigned char num){
    unsigned char num_log2 = 0;
    while(num > 0){
        num = num >> 1;
        num_log2 += 1;
    }
    return num_log2 -1;
}

/*----------------   I2C device   - start   ------------- */
static void device_release(struct device *dev)
{
    return;
}

struct i2c_device_platform_data {
    int parent;
    struct i2c_board_info           info;
    struct i2c_client              *client;
};
/* pca9548 - add 8 bus */

static struct pca954x_platform_mode pca954x_1_mode[] =
{
    { .adap_id = 20,
      .deselect_on_exit = 1,
    },
    { .adap_id = 21,
      .deselect_on_exit = 1,
    },
    { .adap_id = 22,
      .deselect_on_exit = 1,
    },
    { .adap_id = 23,
      .deselect_on_exit = 1,
    },
    { .adap_id = 24,
      .deselect_on_exit = 1,
    },
    { .adap_id = 25,
      .deselect_on_exit = 1,
    },
    { .adap_id = 26,
      .deselect_on_exit = 1,
    },
    { .adap_id = 27,
      .deselect_on_exit = 1,
    },
};

static struct pca954x_platform_mode pca954x_2_mode[] = 
{
    { .adap_id = 30,
      .deselect_on_exit = 1,
    },
    { .adap_id = 31,
      .deselect_on_exit = 1,
    },
    { .adap_id = 32,
      .deselect_on_exit = 1,
    },
    { .adap_id = 33,
      .deselect_on_exit = 1,
    },
    { .adap_id = 34,
      .deselect_on_exit = 1,
    },
    { .adap_id = 35,
      .deselect_on_exit = 1,
    },
    { .adap_id = 36,
      .deselect_on_exit = 1,
    },
    { .adap_id = 37,
      .deselect_on_exit = 1,
    },
};

static struct pca954x_platform_mode pca954x_3_mode[] =
{
    { .adap_id = 38,
      .deselect_on_exit = 1,
    },
    { .adap_id = 39,
      .deselect_on_exit = 1,
    },
    { .adap_id = 40,
      .deselect_on_exit = 1,
    },
    { .adap_id = 41,
      .deselect_on_exit = 1,
    },
    { .adap_id = 42,
      .deselect_on_exit = 1,
    },
    { .adap_id = 43,
      .deselect_on_exit = 1,
    },
    { .adap_id = 44,
      .deselect_on_exit = 1,
    },
    { .adap_id = 45,
      .deselect_on_exit = 1,
    },
};

static struct pca954x_platform_mode pca954x_4_mode[] =
{
    { .adap_id = 46,
      .deselect_on_exit = 1,
    },
    { .adap_id = 47,
      .deselect_on_exit = 1,
    },
    { .adap_id = 48,
      .deselect_on_exit = 1,
    },
    { .adap_id = 49,
      .deselect_on_exit = 1,
    },
    { .adap_id = 50,
      .deselect_on_exit = 1,
    },
    { .adap_id = 51,
      .deselect_on_exit = 1,
    },
    { .adap_id = 52,
      .deselect_on_exit = 1,
    },
    { .adap_id = 53,
      .deselect_on_exit = 1,
    },
};

static struct pca954x_platform_mode pca954x_5_mode[] =
{
    { .adap_id = 54,
      .deselect_on_exit = 1,
    },
    { .adap_id = 55,
      .deselect_on_exit = 1,
    },
    { .adap_id = 56,
      .deselect_on_exit = 1,
    },
    { .adap_id = 57,
      .deselect_on_exit = 1,
    },
    { .adap_id = 58,
      .deselect_on_exit = 1,
    },
    { .adap_id = 59,
      .deselect_on_exit = 1,
    },
    { .adap_id = 60,
      .deselect_on_exit = 1,
    },
    { .adap_id = 61,
      .deselect_on_exit = 1,
    },
};

static struct pca954x_platform_mode pca954x_6_mode[] =
{
    { .adap_id = 62,
      .deselect_on_exit = 1,
    },
    { .adap_id = 63,
      .deselect_on_exit = 1,
    },
    { .adap_id = 64,
      .deselect_on_exit = 1,
    },
    { .adap_id = 65,
      .deselect_on_exit = 1,
    },
    { .adap_id = 66,
      .deselect_on_exit = 1,
    },
    { .adap_id = 67,
      .deselect_on_exit = 1,
    },
    { .adap_id = 68,
      .deselect_on_exit = 1,
    },
    { .adap_id = 69,
      .deselect_on_exit = 1,
    },
};

static struct pca954x_platform_data pca954x_data[] = 
{
    {
        .modes = pca954x_1_mode,
        .num_modes = ARRAY_SIZE(pca954x_1_mode),
    },
    {
        .modes = pca954x_2_mode,
        .num_modes = ARRAY_SIZE(pca954x_2_mode),
    },
    {
        .modes = pca954x_3_mode,
        .num_modes = ARRAY_SIZE(pca954x_3_mode),
    },
    {
        .modes = pca954x_4_mode,
        .num_modes = ARRAY_SIZE(pca954x_4_mode),
    },
    {
        .modes = pca954x_5_mode,
        .num_modes = ARRAY_SIZE(pca954x_5_mode),
    },
    {
        .modes = pca954x_6_mode,
        .num_modes = ARRAY_SIZE(pca954x_6_mode),
    },
};

static struct i2c_board_info __initdata i2c_info_pca9548[] =
{
        {
            I2C_BOARD_INFO("pca9548", 0x70),
            .platform_data = &pca954x_data[0],
        },
        {
            I2C_BOARD_INFO("pca9548", 0x71),
            .platform_data = &pca954x_data[1],
        },
        {
            I2C_BOARD_INFO("pca9548", 0x72),
            .platform_data = &pca954x_data[2],
        },
        {
            I2C_BOARD_INFO("pca9548", 0x73),
            .platform_data = &pca954x_data[3],
        },
        {
            I2C_BOARD_INFO("pca9548", 0x74),
            .platform_data = &pca954x_data[4],
        },
        {
            I2C_BOARD_INFO("pca9548", 0x76),
            .platform_data = &pca954x_data[5],
        },
};

static struct i2c_device_platform_data et_c032if_i2c_device_platform_data[] = {
    {
        /* id eeprom (0x53) */
        .parent = 10,
        .info = { I2C_BOARD_INFO("24c02", 0x53) },
        .client = NULL,
    },
    {
        /* qsfp 1 (0x50) */
        .parent = 30,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 2 (0x50) */
        .parent = 31,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 3 (0x50) */
        .parent = 32,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 4 (0x50) */
        .parent = 33,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 5 (0x50) */
        .parent = 34,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 6 (0x50) */
        .parent = 35,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 7 (0x50) */
        .parent = 36,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 8 (0x50) */
        .parent = 37,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 9 (0x50) */
        .parent = 38,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 10 (0x50) */
        .parent = 39,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 11 (0x50) */
        .parent = 40,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 12 (0x50) */
        .parent = 41,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 13 (0x50) */
        .parent = 42,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 14 (0x50) */
        .parent = 43,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 15 (0x50) */
        .parent = 44,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 16 (0x50) */
        .parent = 45,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 17 (0x50) */
        .parent = 46,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 18 (0x50) */
        .parent = 47,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 19 (0x50) */
        .parent = 48,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 20 (0x50) */
        .parent = 49,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 21 (0x50) */
        .parent = 50,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 22 (0x50) */
        .parent = 51,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 23 (0x50) */
        .parent = 52,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 24 (0x50) */
        .parent = 53,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 25 (0x50) */
        .parent = 54,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 26 (0x50) */
        .parent = 55,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 27 (0x50) */
        .parent = 56,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 28 (0x50) */
        .parent = 57,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 29 (0x50) */
        .parent = 58,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 30 (0x50) */
        .parent = 59,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 31 (0x50) */
        .parent = 60,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* qsfp 32 (0x50) */
        .parent = 61,
        .info = { .type = "optoe1", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* sfp 1 (0x50) */
        .parent = 62,
        .info = { .type = "optoe2", .addr = 0x50 },
        .client = NULL,
    },
    {
        /* sfp 2 (0x50) */
        .parent = 63,
        .info = { .type = "optoe2", .addr = 0x50 },
        .client = NULL,
    },

};


static struct platform_device et_c032if_i2c_device[] = {
    et_c032if_i2c_device_num(0),
    et_c032if_i2c_device_num(1),
    et_c032if_i2c_device_num(2),
    et_c032if_i2c_device_num(3),
    et_c032if_i2c_device_num(4),
    et_c032if_i2c_device_num(5),
    et_c032if_i2c_device_num(6),
    et_c032if_i2c_device_num(7),
    et_c032if_i2c_device_num(8),
    et_c032if_i2c_device_num(9),
    et_c032if_i2c_device_num(10),
    et_c032if_i2c_device_num(11),
    et_c032if_i2c_device_num(12),
    et_c032if_i2c_device_num(13),
    et_c032if_i2c_device_num(14),
    et_c032if_i2c_device_num(15),
    et_c032if_i2c_device_num(16),
    et_c032if_i2c_device_num(17),
    et_c032if_i2c_device_num(18),
    et_c032if_i2c_device_num(19),
    et_c032if_i2c_device_num(20),
    et_c032if_i2c_device_num(21),
    et_c032if_i2c_device_num(22),
    et_c032if_i2c_device_num(23),
    et_c032if_i2c_device_num(24),
    et_c032if_i2c_device_num(25),
    et_c032if_i2c_device_num(26),
    et_c032if_i2c_device_num(27),
    et_c032if_i2c_device_num(28),
    et_c032if_i2c_device_num(29),
    et_c032if_i2c_device_num(30),
    et_c032if_i2c_device_num(31),
    et_c032if_i2c_device_num(32),
    et_c032if_i2c_device_num(33),
    et_c032if_i2c_device_num(34),
};

/*----------------   I2C device   - end   ------------- */

/*----------------   I2C driver   - start   ------------- */
static int __init i2c_device_probe(struct platform_device *pdev)
{
    struct i2c_device_platform_data *pdata;
    struct i2c_adapter *parent;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(pdata->parent);
    if (!parent) {
        dev_err(&pdev->dev, "Parent adapter (%d) not found\n",
            pdata->parent);
        return -ENODEV;
    }

    pdata->client = i2c_new_device(parent, &pdata->info);
    if (!pdata->client) {
        dev_err(&pdev->dev, "Failed to create i2c client %s at %d\n",
            pdata->info.type, pdata->parent);
        return -ENODEV;
    }

    return 0;
}

static int __exit i2c_deivce_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent;
    struct i2c_device_platform_data *pdata;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
        return -ENODEV;
    }
    if (pdata->client) {
        parent = (pdata->client)->adapter;
        i2c_unregister_device(pdata->client);
        i2c_put_adapter(parent);
    }
    return 0;
}
static struct platform_driver i2c_device_driver = {
    .probe = i2c_device_probe,
    .remove = __exit_p(i2c_deivce_remove),
    .driver = {
        .owner = THIS_MODULE,
        .name = "delta-et-c032if-i2c-device",
    }
};

/*----------------   I2C driver   - end   ------------- */

/*----------------    CPLD - start   ------------- */

unsigned char cpupld_reg_addr;
unsigned char swpld2_reg_addr;
unsigned char swpld3_reg_addr;

/*    CPLD  -- device   */

enum cpld_type {
    cpu_cpld,
};

enum swpld2_type {
    swpld2,
};

enum swpld3_type {
    swpld3,
};

enum cpld_attributes {
//CPLDs address and value
    CPLD_REG_ADDR,
    CPLD_REG_VALUE,
    SWPLD2_REG_ADDR,
    SWPLD2_REG_VALUE,
    SWPLD3_REG_ADDR,
    SWPLD3_REG_VALUE,
    SFP_SELECT_PORT,
    SFP_IS_PRESENT,
    SFP_IS_PRESENT_ALL,
    QSFP_LP_MODE,
    QSFP_RESET,
    QSFP_INTERRUPT,
    QSFP_RESPONDE,
};

struct cpld_platform_data {
    int reg_addr;
    struct i2c_client *client;
};

static struct cpld_platform_data et_c032if_cpld_platform_data[] = {
    [cpu_cpld] = {
        .reg_addr = CPULD_ADDR,
    },
};

static struct cpld_platform_data et_c032if_swpld2_platform_data[] = {
    [swpld2] = {
        .reg_addr = SWPLD2_ADDR,
    },
};

static struct cpld_platform_data et_c032if_swpld3_platform_data[] = {
    [swpld3] = {
        .reg_addr = SWPLD3_ADDR,
    },
};

static struct platform_device et_c032if_cpld = {
    .name = "delta-et-c032if-cpld",
    .id   = 0,
    .dev  = {
        .platform_data   = et_c032if_cpld_platform_data,
        .release         = device_release,
    },
};

static struct platform_device et_c032if_swpld2 = {
    .name = "delta-et-c032if-swpld2",
    .id   = 0,
    .dev  = {
        .platform_data  = et_c032if_swpld2_platform_data,
        .release        = device_release
    },
};

static struct platform_device et_c032if_swpld3 = {
    .name = "delta-et-c032if-swpld3",
    .id   = 0,
    .dev  = {
        .platform_data  = et_c032if_swpld3_platform_data,
        .release        = device_release
    },
};

static ssize_t get_cpld_reg(struct device *dev, struct device_attribute *dev_attr, char *buf)
{
    int ret;
    int mask;
    int value;
    char note[450];
    unsigned char reg;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    mutex_lock(&dni_lock);
    switch (attr->index) {
        case CPLD_REG_ADDR:
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%02x\n", cpupld_reg_addr);
        case SWPLD2_REG_ADDR:
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%02x\n", swpld2_reg_addr);
        case SWPLD3_REG_ADDR:
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%02x\n", swpld3_reg_addr);
        case CPLD_REG_VALUE:
            ret = i2c_smbus_read_byte_data(pdata[cpu_cpld].client, cpupld_reg_addr);
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%02x\n", ret);
        case SWPLD2_REG_VALUE:
            ret = i2c_smbus_read_byte_data(pdata[swpld2].client, swpld2_reg_addr);
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%02x\n", ret);
        case SWPLD3_REG_VALUE:
            ret = i2c_smbus_read_byte_data(pdata[swpld3].client, swpld3_reg_addr);
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%02x\n", ret);
        default:
            mutex_unlock(&dni_lock);
            return sprintf(buf, "%d not found", attr->index);
    }

    switch (mask) {
        case 0xff:
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%02x%s", value, note);
        case 0x0f:
        case 0x07:
        case 0x03:
            break;
        case 0x0c:
            value = value >> 2;
            break;
        case 0xf0:
        case 0x70:
        case 0x30:
            value = value >> 4;
            break;
        case 0xe0:
            value = value >> 5;
            break;
        case 0xc0:
            value = value >> 6;
            break;
        default :
            value = value >> dni_log2(mask);
            mutex_unlock(&dni_lock);
            return sprintf(buf, "%d%s", value, note);
    }
    mutex_unlock(&dni_lock);
    return sprintf(buf, "0x%02x%s", value, note);
}

static ssize_t set_cpld_reg(struct device *dev, struct device_attribute *dev_attr,
            const char *buf, size_t count)
{
    int err;
    int value;
    int set_data;
    unsigned long set_data_ul;
    unsigned char reg;
    unsigned char mask;
    unsigned char mask_out;

    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    err = kstrtoul(buf, 0, &set_data_ul);
    if (err){
        return err;
    }

    mutex_lock(&dni_lock);
    set_data = (int)set_data_ul;
    if (set_data > 0xff){
        printk(KERN_ALERT "address out of range (0x00-0xFF)\n");
        mutex_unlock(&dni_lock);
        return count;
    }

    switch (attr->index) {
        case CPLD_REG_ADDR:
            cpupld_reg_addr = set_data;
            mutex_unlock(&dni_lock);
            return count;
        case SWPLD2_REG_ADDR:
            swpld2_reg_addr = set_data;
            mutex_unlock(&dni_lock);
            return count;
        case SWPLD3_REG_ADDR:
            swpld3_reg_addr = set_data;
            mutex_unlock(&dni_lock);
            return count;
        case CPLD_REG_VALUE:
            i2c_smbus_write_byte_data(pdata[cpu_cpld].client, cpupld_reg_addr, set_data);
            mutex_unlock(&dni_lock);
            return count;
        case SWPLD2_REG_VALUE:
            i2c_smbus_write_byte_data(pdata[swpld2].client, swpld2_reg_addr, set_data);
            mutex_unlock(&dni_lock);
            return count;
        case SWPLD3_REG_VALUE:
            i2c_smbus_write_byte_data(pdata[swpld3].client, swpld3_reg_addr, set_data);
            mutex_unlock(&dni_lock);
            return count;
        default:
            mutex_unlock(&dni_lock);
            return sprintf(buf, "%d not found", attr->index);
    }

    switch (mask) {
        case 0x03:
        case 0x07:
        case 0x0f:
        case 0xff:
            set_data = mask_out | (set_data & mask);
            break;
        case 0x0c:
            set_data = set_data << 2;
            set_data = mask_out | (set_data & mask);
            break;
        case 0xf0:
        case 0x70:
        case 0x30:
            set_data = set_data << 4;
            set_data = mask_out | (set_data & mask);
            break;
        case 0xe0:
            set_data = set_data << 5;
            set_data = mask_out | (set_data & mask);
            break;
        case 0xc0:
            set_data = set_data << 6;
            set_data = mask_out | (set_data & mask);
            break;
        default :
            set_data = mask_out | (set_data << dni_log2(mask) );
    }
    switch (attr->index) {
        default:
            mutex_unlock(&dni_lock);
            return sprintf(buf, "cpld not found");
    }
    mutex_unlock(&dni_lock);
    return count;
}

/* ---------------- SFP attribute read/write - start -------- */
long sfp_port_data = 0;
static struct kobject *kobj_cpld;
static struct kobject *kobj_swpld1;
static struct kobject *kobj_swpld2;
static struct kobject *kobj_swpld3;


static ssize_t for_status(struct device *dev, struct device_attribute *dev_attr, char *buf){
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct device *i2cdev_1 = kobj_to_dev(kobj_swpld2);
    struct device *i2cdev_2 = kobj_to_dev(kobj_swpld3);
    struct cpld_platform_data *pdata1 = i2cdev_1->platform_data;
    struct cpld_platform_data *pdata2 = i2cdev_2->platform_data;

    mutex_lock(&dni_lock);
    long port_t = 0;
    int ret;
    u32 data = 0;
    int data2 = 0;
    u8 reg_t = 0x00;
    u8 save_bytes = 0x00;
    int bit_t = 0x00;

    switch (attr->index) {
        case SFP_IS_PRESENT:
            /* Report the SFP/QSFP ALL PRESENCE status
             * This data information form SWPLD2 and SWPLD3. */

            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_PRESENCE_4);
            data = (u32)(ret & 0xff);
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_PRESENCE_3);
            data |= (u32)(ret & 0xff) << 8;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_PRESENCE_2);
            data |= (u32)(ret & 0xff) << 16;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_PRESENCE_1);
            data |= (u32)(ret & 0xff) << 24;
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, SFP_PRESENCE);
            data2 = (ret & 0x44);
            save_bytes = data2 & 0x40;
            save_bytes = save_bytes << 1;
            data2 &= 0x04;
            data2 = data2 << 4;
            data2 |= save_bytes;
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%x%x\n", data, data2);
 
        case QSFP_LP_MODE:
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_LP_MODE_4);
            data = (u32)(ret & 0xff);
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_LP_MODE_3);
            data |= (u32)(ret & 0xff) << 8;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_LP_MODE_2);
            data |= (u32)(ret & 0xff) << 16;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_LP_MODE_1);
            data |= (u32)(ret & 0xff) << 24;
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%x\n", data);

         case QSFP_RESET:
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_RESET_4);
            data = (u32)(ret & 0xff);
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_RESET_3);
            data |= (u32)(ret & 0xff) << 8;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_RESET_2);
            data |= (u32)(ret & 0xff) << 16;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_RESET_1);
            data |= (u32)(ret & 0xff) << 24;
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%x\n", data);

         case QSFP_INTERRUPT:
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_INTERRUPT_4);
            data = (u32)(ret & 0xff);
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_INTERRUPT_3);
            data |= (u32)(ret & 0xff) << 8;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_INTERRUPT_2);
            data |= (u32)(ret & 0xff) << 16;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_INTERRUPT_1);
            data |= (u32)(ret & 0xff) << 24;
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%x\n", data);

         case QSFP_RESPONDE:
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_RESPONDE_4);
            data = (u32)(ret & 0xff);
            ret = i2c_smbus_read_byte_data(pdata2[swpld3].client, QSFP_RESPONDE_3);
            data |= (u32)(ret & 0xff) << 8;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_RESPONDE_2);
            data |= (u32)(ret & 0xff) << 16;
            ret = i2c_smbus_read_byte_data(pdata1[swpld2].client, QSFP_RESPONDE_1);
            data |= (u32)(ret & 0xff) << 24;
            mutex_unlock(&dni_lock);
            return sprintf(buf, "0x%x\n", data);

        default:
            mutex_unlock(&dni_lock);
            return sprintf(buf, "%d not found", attr->index);
    }
}


static ssize_t set_lpmode_data(struct device *dev, struct device_attribute *dev_attr, const char *buf, size_t count)
{
    struct device *i2cdev_1 = kobj_to_dev(kobj_swpld2);
    struct device *i2cdev_2 = kobj_to_dev(kobj_swpld3);
    struct cpld_platform_data *pdata1 = i2cdev_1->platform_data;
    struct cpld_platform_data *pdata2 = i2cdev_2->platform_data;
    unsigned long long set_data;
    int err;
    int values = 0x00;
    u8 reg_t = 0x00;

    err = kstrtoull(buf, 16, &set_data);
    if (err){
        return err;
    }
    mutex_lock(&dni_lock);
    reg_t = QSFP_LP_MODE_1;
    values = ((set_data >> 24 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata1[swpld2].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }

    reg_t = QSFP_LP_MODE_2;
    values = ((set_data >> 16 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata1[swpld2].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }

    reg_t = QSFP_LP_MODE_3;
    values = ((set_data >> 8 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata2[swpld3].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }

    reg_t = QSFP_LP_MODE_4;
    values = (set_data & 0xff);
    if (i2c_smbus_write_byte_data(pdata2[swpld3].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }

    mutex_unlock(&dni_lock);
    return count;

ERROR:
    mutex_unlock(&dni_lock);
    return -EIO;
}

static ssize_t set_reset_data(struct device *dev, struct device_attribute *dev_attr, const char *buf, size_t count)
{
    struct device *i2cdev_1 = kobj_to_dev(kobj_swpld2);
    struct device *i2cdev_2 = kobj_to_dev(kobj_swpld3);
    struct cpld_platform_data *pdata1 = i2cdev_1->platform_data;
    struct cpld_platform_data *pdata2 = i2cdev_2->platform_data;
    unsigned long long set_data;
    int err;
    int values = 0x00;
    u8 reg_t = 0x00;

    err = kstrtoull(buf, 16, &set_data);
    if (err){
        return err;
    }

    mutex_lock(&dni_lock);
    reg_t = QSFP_RESET_1;
    values = ((set_data >> 24 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata1[swpld2].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_RESET_2;
    values = ((set_data >> 16 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata1[swpld2].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_RESET_3;
    values = ((set_data >> 8 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata2[swpld3].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_RESET_4;
    values = (set_data & 0xff);
    if (i2c_smbus_write_byte_data(pdata2[swpld3].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }

    mutex_unlock(&dni_lock);
    return count;

ERROR:
    mutex_unlock(&dni_lock);
    return -EIO;
}

static ssize_t set_interrupt_data(struct device *dev, struct device_attribute *dev_attr, const char *buf, size_t count)
{
    struct device *i2cdev_1 = kobj_to_dev(kobj_swpld2);
    struct device *i2cdev_2 = kobj_to_dev(kobj_swpld3);
    struct cpld_platform_data *pdata1 = i2cdev_1->platform_data;
    struct cpld_platform_data *pdata2 = i2cdev_2->platform_data;
    unsigned long long set_data;
    int err;
    int values = 0x00;
    u8 reg_t = 0x00;

    err = kstrtoull(buf, 16, &set_data);
    if (err){
        return err;
    }

    mutex_lock(&dni_lock);
    reg_t = QSFP_INTERRUPT_1;
    values = ((set_data >> 24 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata1[swpld2].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_INTERRUPT_2;
    values = ((set_data >> 16 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata1[swpld2].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_INTERRUPT_3;
    values = ((set_data >> 8 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata2[swpld3].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_INTERRUPT_4;
    values = (set_data & 0xff);

    if (i2c_smbus_write_byte_data(pdata2[swpld3].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }

    mutex_unlock(&dni_lock);
    return count;

ERROR:
    mutex_unlock(&dni_lock);
    return -EIO;
}

static ssize_t set_responde_data(struct device *dev, struct device_attribute *dev_attr, const char *buf, size_t count)
{
    struct device *i2cdev_1 = kobj_to_dev(kobj_swpld2);
    struct device *i2cdev_2 = kobj_to_dev(kobj_swpld3);
    struct cpld_platform_data *pdata1 = i2cdev_1->platform_data;
    struct cpld_platform_data *pdata2 = i2cdev_2->platform_data;
    unsigned long long set_data;
    int err;
    int values = 0x00;
    u8 reg_t = 0x00;

    err = kstrtoull(buf, 16, &set_data);
    if (err){
        return err;
    }

    mutex_lock(&dni_lock);
    reg_t = QSFP_RESPONDE_1;
    values = ((set_data >> 24 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata1[swpld2].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_RESPONDE_2;
    values = ((set_data >> 16 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata1[swpld2].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_RESPONDE_3;
    values = ((set_data >> 8 ) & 0xff);
    if (i2c_smbus_write_byte_data(pdata2[swpld3].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }
    reg_t = QSFP_RESPONDE_4;
    values = (set_data & 0xff);
    if (i2c_smbus_write_byte_data(pdata2[swpld3].client, reg_t, (u8)values) < 0)
    {
        goto ERROR;
    }

    mutex_unlock(&dni_lock);
    return count;

ERROR:
    mutex_unlock(&dni_lock);
    return -EIO;
}

static SENSOR_DEVICE_ATTR(cpld_reg_addr,       S_IRUGO | S_IWUSR, get_cpld_reg,    set_cpld_reg,       CPLD_REG_ADDR);
static SENSOR_DEVICE_ATTR(cpld_reg_value,      S_IRUGO | S_IWUSR, get_cpld_reg,    set_cpld_reg,       CPLD_REG_VALUE);
static SENSOR_DEVICE_ATTR(swpld2_reg_addr,     S_IRUGO | S_IWUSR, get_cpld_reg,    set_cpld_reg,       SWPLD2_REG_ADDR);
static SENSOR_DEVICE_ATTR(swpld2_reg_value,    S_IRUGO | S_IWUSR, get_cpld_reg,    set_cpld_reg,       SWPLD2_REG_VALUE);
static SENSOR_DEVICE_ATTR(swpld3_reg_addr,     S_IRUGO | S_IWUSR, get_cpld_reg,    set_cpld_reg,       SWPLD3_REG_ADDR);
static SENSOR_DEVICE_ATTR(swpld3_reg_value,    S_IRUGO | S_IWUSR, get_cpld_reg,    set_cpld_reg,       SWPLD3_REG_VALUE);
//SFP, QSFP
static SENSOR_DEVICE_ATTR(sfp_is_present,      S_IRUGO,           for_status,      NULL,               SFP_IS_PRESENT);
static SENSOR_DEVICE_ATTR(sfp_lp_mode,         S_IWUSR | S_IRUGO, for_status,      set_lpmode_data,    QSFP_LP_MODE);
static SENSOR_DEVICE_ATTR(sfp_reset,           S_IWUSR | S_IRUGO, for_status,      set_reset_data,     QSFP_RESET);
static SENSOR_DEVICE_ATTR(sfp_interrupt,       S_IWUSR | S_IRUGO, for_status,      set_interrupt_data, QSFP_INTERRUPT);
static SENSOR_DEVICE_ATTR(sfp_responde,        S_IWUSR | S_IRUGO, for_status,      set_responde_data,  QSFP_RESPONDE);


static struct attribute *cpld_attrs[] = {
    &sensor_dev_attr_cpld_reg_value.dev_attr.attr,
    &sensor_dev_attr_cpld_reg_addr.dev_attr.attr,
//SFP, QSFP
    &sensor_dev_attr_sfp_is_present.dev_attr.attr,
    &sensor_dev_attr_sfp_lp_mode.dev_attr.attr,
    &sensor_dev_attr_sfp_reset.dev_attr.attr,
    &sensor_dev_attr_sfp_interrupt.dev_attr.attr,
    &sensor_dev_attr_sfp_responde.dev_attr.attr,
    NULL,
};

static struct attribute *swpld2_attrs[] = {
    &sensor_dev_attr_swpld2_reg_value.dev_attr.attr,
    &sensor_dev_attr_swpld2_reg_addr.dev_attr.attr,
    NULL,
};

static struct attribute *swpld3_attrs[] = {
    &sensor_dev_attr_swpld3_reg_value.dev_attr.attr,
    &sensor_dev_attr_swpld3_reg_addr.dev_attr.attr,
    NULL,
};

static struct attribute_group cpld_attr_grp = {
    .attrs = cpld_attrs,
};

static struct attribute_group swpld2_attr_grp = {
    .attrs = swpld2_attrs,
};

static struct attribute_group swpld3_attr_grp = {
    .attrs = swpld3_attrs,
};


/*    CPLD  -- driver   */
static int __init cpld_probe(struct platform_device *pdev)
{
    struct i2c_mux_core *muxc;
    struct cpld_platform_data *pdata;
    struct i2c_adapter *parent;
    int ret;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "CPLD platform data not found\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(BUS0);
    if (!parent) {
        printk(KERN_WARNING "Parent adapter (%d) not found\n", BUS0);
        return -ENODEV;
    }

    pdata[cpu_cpld].client = i2c_new_dummy(parent, pdata[cpu_cpld].reg_addr);
    if (!pdata[cpu_cpld].client) {
        printk(KERN_WARNING "Fail to create dummy i2c client for addr %d\n", pdata[cpu_cpld].reg_addr);
        goto error;
    }

    kobj_cpld = &pdev->dev.kobj;
    ret = sysfs_create_group(&pdev->dev.kobj, &cpld_attr_grp);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld attribute group");
        goto error;
    }

    return 0;
error:
    kobject_put(kobj_cpld);
    i2c_unregister_device(pdata[cpu_cpld].client);
    i2c_put_adapter(parent);

    return -ENODEV; 
}

static int __exit cpld_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent = NULL;
    struct cpld_platform_data *pdata = pdev->dev.platform_data;

    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
    } 
    else {
        if (pdata[cpu_cpld].client) {
            if (!parent) {
                parent = (pdata[cpu_cpld].client)->adapter;
            }
            i2c_unregister_device(pdata[cpu_cpld].client);
        }
    }
    i2c_put_adapter(parent);

    return 0;
}

static int __init swpld2_probe(struct platform_device *pdev)
{
    struct cpld_platform_data *pdata;
    struct i2c_adapter *parent;
    int ret;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "SWPLD2 platform data not found\n");
        return -ENODEV;
    }
    parent = i2c_get_adapter(27);
    if (!parent) {
        printk(KERN_WARNING "Parent adapter (%d) not found\n", 27);
        return -ENODEV;
    }

    pdata[swpld2].client = i2c_new_dummy(parent, pdata[swpld2].reg_addr);
    if (!pdata[swpld2].client) {
        printk(KERN_WARNING "Fail to create dummy i2c client for addr %d\n", pdata[swpld2].reg_addr);
        goto error;
    }

    kobj_swpld2 = &pdev->dev.kobj;
    ret = sysfs_create_group(&pdev->dev.kobj, &swpld2_attr_grp);
    if (ret) {
        printk(KERN_WARNING "Fail to create swpld attribute group");
        goto error;
    }
    return 0;

error:
    kobject_put(kobj_swpld2);
    i2c_unregister_device(pdata[swpld2].client);
    i2c_put_adapter(parent);
    return -ENODEV;
}

static int __exit swpld2_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent = NULL;
    struct cpld_platform_data *pdata = pdev->dev.platform_data;
    sysfs_remove_group(&pdev->dev.kobj, &swpld2_attr_grp);

    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
    }
    else {
        if (pdata[swpld2].client) {
            if (!parent) {
                parent = (pdata[swpld2].client)->adapter;
            }
        i2c_unregister_device(pdata[swpld2].client);
        }
    }
    i2c_put_adapter(parent);
    return 0;
}


static int __init swpld3_probe(struct platform_device *pdev)
{
    struct cpld_platform_data *pdata;
    struct i2c_adapter *parent;
    int ret;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "SWPLD3 platform data not found\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(27);
    if (!parent) {
        printk(KERN_WARNING "Parent adapter (%d) not found\n", BUS7);
        return -ENODEV;
    }

    pdata[swpld3].client = i2c_new_dummy(parent, pdata[swpld3].reg_addr);
    if (!pdata[swpld3].client) {
        printk(KERN_WARNING "Fail to create dummy i2c client for addr %d\n", pdata[swpld3].reg_addr);
        goto error;
    }

    kobj_swpld3 = &pdev->dev.kobj;
    ret = sysfs_create_group(&pdev->dev.kobj, &swpld3_attr_grp);
    if (ret) {
        printk(KERN_WARNING "Fail to create swpld attribute group");
        goto error;
    }

    return 0;

error:
    kobject_put(kobj_swpld3);
    i2c_unregister_device(pdata[swpld3].client);
    i2c_put_adapter(parent);

    return -ENODEV;
}

static int __exit swpld3_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent = NULL;
    struct cpld_platform_data *pdata = pdev->dev.platform_data;
    sysfs_remove_group(&pdev->dev.kobj, &swpld3_attr_grp);

    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
    }
    else {
        if (pdata[swpld3].client) {
            if (!parent) {
                parent = (pdata[swpld3].client)->adapter;
            }
        i2c_unregister_device(pdata[swpld3].client);
        }
    }
    i2c_put_adapter(parent);
    return 0;
}

static struct platform_driver cpld_driver = {
    .probe  = cpld_probe,
    .remove = __exit_p(cpld_remove),
    .driver = {
        .owner  = THIS_MODULE,
        .name   = "delta-et-c032if-cpld",
    },
};

static struct platform_driver swpld2_driver = {
    .probe  = swpld2_probe,
    .remove = __exit_p(swpld2_remove),
    .driver = {
        .owner = THIS_MODULE,
        .name  = "delta-et-c032if-swpld2",
    },
};

static struct platform_driver swpld3_driver = {
    .probe  = swpld3_probe,
    .remove = __exit_p(swpld3_remove),
    .driver = {
        .owner = THIS_MODULE,
        .name  = "delta-et-c032if-swpld3",
    },
};


/*----------------    CPLD  - end   ------------- */

/*----------------    MUX   - start   ------------- */

struct cpld_mux_platform_data {
    int parent;
    int base_nr;
    int reg_addr;
    struct i2c_client *cpld;
};

struct cpld_mux {
    struct i2c_adapter *parent;
    struct i2c_adapter **child;
    struct cpld_mux_platform_data data;
};

static struct cpld_mux_platform_data et_c032if_cpld_mux_platform_data[] = {
    {
        .parent         = BUS0,
        .base_nr        = BUS0_BASE_NUM,
        .cpld           = NULL,
        .reg_addr       = BUS0_MUX_REG,
    },
};

static struct platform_device et_c032if_cpld_mux[] =
{
    {
        .name           = "delta-et-c032if-cpld-mux",
        .id             = 0,
        .dev            = {
                .platform_data   = &et_c032if_cpld_mux_platform_data[0],
                .release         = device_release,
        },
    },
};

static int cpld_reg_write_byte(struct i2c_client *client, u8 regaddr, u8 val)
{
    union i2c_smbus_data data;

    data.byte = val;
    return client->adapter->algo->smbus_xfer(client->adapter, client->addr,
                                             client->flags,
                                             I2C_SMBUS_WRITE,
                                             regaddr, I2C_SMBUS_BYTE_DATA, &data);
}

static int cpld_mux_select(struct i2c_mux_core *muxc, u32 chan)
{
    struct cpld_mux  *mux = i2c_mux_priv(muxc);
    u8 cpld_mux_val=0;

    if ( mux->data.base_nr == BUS0_BASE_NUM )
    {
        switch (chan) {
            case 0:
                cpld_mux_val = EEPROM_VAL;
                break;
            case 1:
                cpld_mux_val = PCA9548_VAL;
                break;
            case 2:
                cpld_mux_val = QSFP_VAL;
                break;
            default:
                cpld_mux_val = 0x00;
                break;
        }
    }
    else
    {
        cpld_mux_val = 0x00;
    }
    return cpld_reg_write_byte(mux->data.cpld, mux->data.reg_addr, (u8)(cpld_mux_val & 0xff));

}

static int __init cpld_mux_probe(struct platform_device *pdev)
{
    struct i2c_mux_core *muxc;
    struct cpld_mux *mux;
    struct cpld_mux_platform_data *pdata;
    struct i2c_adapter *parent;
    int i, ret, dev_num;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "SWPLD platform data not found\n");
        return -ENODEV;
    }
    mux = kzalloc(sizeof(*mux), GFP_KERNEL);
    if (!mux) {
        printk(KERN_ERR "Failed to allocate memory for mux\n");
        return -ENOMEM;
    }
    mux->data = *pdata;

    parent = i2c_get_adapter(pdata->parent);
    if (!parent) {
        kfree(mux);
        dev_err(&pdev->dev, "Parent adapter (%d) not found\n", pdata->parent);
        return -ENODEV;
    }
    /* Judge bus number to decide how many devices*/
    switch (pdata->parent) {
        case BUS0:
            dev_num = BUS0_DEV_NUM;
            break;
        default :
            dev_num = DEFAULT_NUM;
            break;
    }

    muxc = i2c_mux_alloc(parent, &pdev->dev, dev_num, 0, 0,cpld_mux_select, NULL);
    if (!muxc) {
        ret = -ENOMEM;
        goto alloc_failed;
    }
    muxc->priv = mux;
    platform_set_drvdata(pdev, muxc);

    for (i = 0; i < dev_num; i++)
    {
        int nr = pdata->base_nr + i;
        unsigned int class = 0;
        ret = i2c_mux_add_adapter(muxc, nr, i, class);
        if (ret) {
            dev_err(&pdev->dev, "Failed to add adapter %d\n", i);
            goto add_adapter_failed;
        }
    }
    dev_info(&pdev->dev, "%d port mux on %s adapter\n", dev_num, parent->name);
    return 0;

add_adapter_failed:
    i2c_mux_del_adapters(muxc);
alloc_failed2:
    kfree(mux);
alloc_failed:
    i2c_put_adapter(parent);

    return ret;
}

static int __exit cpld_mux_remove(struct platform_device *pdev)
{
    struct i2c_mux_core *muxc = platform_get_drvdata(pdev);
    struct i2c_adapter *parent = muxc->parent;

    i2c_mux_del_adapters(muxc);
    i2c_put_adapter(parent);

    return 0;
}

static struct platform_driver cpld_mux_driver = {
    .probe  = cpld_mux_probe,
    .remove = __exit_p(cpld_mux_remove), /* TODO */
    .driver = {
        .owner  = THIS_MODULE,
        .name   = "delta-et-c032if-cpld-mux",
    },
};
/*----------------    MUX   - end   ------------- */

/*----------------   module initialization     ------------- */
static int __init delta_et_c032if_platform_init(void)
{
    struct i2c_adapter *adapter;
    struct cpld_platform_data *cpld_pdata;
    struct cpld_mux_platform_data *cpld_mux_pdata;
    int ret,i = 0;

    mutex_init(&dni_lock);
    printk("c032if_platform module initialization\n");

    // set the CPLD prob and  remove
    ret = platform_driver_register(&cpld_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register cpld driver\n");
        goto error_cpld_driver;
    }

    ret = platform_driver_register(&cpld_mux_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register swpld mux driver\n");
        goto error_cpld_mux_driver;
    }

    // set the SWPLD prob and remove
    ret = platform_driver_register(&swpld2_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register swpld driver\n");
        goto error_swpld2_driver;
    }

    // set the SWPLD prob and remove
    ret = platform_driver_register(&swpld3_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register swpld driver\n");
        goto error_swpld3_driver;
    }

    // register the i2c devices    
    ret = platform_driver_register(&i2c_device_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register i2c device driver\n");
        goto error_i2c_device_driver;
    }

    // register the CPLD
    ret = platform_device_register(&et_c032if_cpld);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld device\n");
        goto error_et_c032if_cpld;
    }

    cpld_pdata = et_c032if_cpld.dev.platform_data;
    cpld_mux_pdata = et_c032if_cpld_mux[0].dev.platform_data;
    cpld_mux_pdata->cpld = cpld_pdata[cpu_cpld].client;
    ret = platform_device_register(&et_c032if_cpld_mux[0]);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld mux\n");
        goto error_et_c032if_cpld_mux;
    }

    adapter = i2c_get_adapter(11);
    i2c_client_9548_1 = i2c_new_device(adapter, &i2c_info_pca9548[0]);
    i2c_put_adapter(adapter);
    adapter = i2c_get_adapter(12);
    i2c_client_9548_2 = i2c_new_device(adapter, &i2c_info_pca9548[1]);
    i2c_client_9548_3 = i2c_new_device(adapter, &i2c_info_pca9548[2]);
    i2c_client_9548_4 = i2c_new_device(adapter, &i2c_info_pca9548[3]);
    i2c_client_9548_5 = i2c_new_device(adapter, &i2c_info_pca9548[4]);
    i2c_client_9548_6 = i2c_new_device(adapter, &i2c_info_pca9548[5]);
    i2c_put_adapter(adapter);

    // register the SWPLD2
    ret = platform_device_register(&et_c032if_swpld2);
    if (ret) {
        printk(KERN_WARNING "Fail to create swpld2 device\n");
        goto error_swpld2_device;
    }

    // register the SWPLD3
    ret = platform_device_register(&et_c032if_swpld3);
    if (ret) {
        printk(KERN_WARNING "Fail to create swpld3 device\n");
        goto error_swpld3_device;
    }
    for (i = 0; i < ARRAY_SIZE(et_c032if_i2c_device); i++)
    {
        ret = platform_device_register(&et_c032if_i2c_device[i]);
        if (ret) {
            printk(KERN_WARNING "Fail to create i2c device %d\n", i);
            goto error_et_c032if_i2c_device;
        }
    }
    if (ret)
        goto error_et_c032if_cpld;

    return 0;

error_et_c032if_i2c_device:
    i--;
    for (; i >= 0; i--) {
        platform_device_unregister(&et_c032if_i2c_device[i]);
    }
    i = ARRAY_SIZE(et_c032if_cpld_mux);
    platform_device_unregister(&et_c032if_swpld3);
error_swpld3_device:   
    platform_device_unregister(&et_c032if_swpld2);
error_swpld2_device:
    i2c_unregister_device(i2c_client_9548_1);
    i2c_unregister_device(i2c_client_9548_2);
    i2c_unregister_device(i2c_client_9548_3);
    i2c_unregister_device(i2c_client_9548_4);
    i2c_unregister_device(i2c_client_9548_5);
    i2c_unregister_device(i2c_client_9548_6);
error_et_c032if_cpld_mux:
    platform_device_unregister(&et_c032if_cpld_mux[0]);
    platform_device_unregister(&et_c032if_cpld);
error_et_c032if_cpld:
    platform_driver_unregister(&i2c_device_driver);
error_i2c_device_driver:
    platform_driver_unregister(&swpld3_driver);
error_swpld3_driver:
    platform_driver_unregister(&swpld2_driver);
error_swpld2_driver:
    platform_driver_unregister(&cpld_mux_driver);
error_cpld_mux_driver:
    platform_driver_unregister(&cpld_driver);
error_cpld_driver:
    return ret;
}

static void __exit delta_et_c032if_platform_exit(void)
{
    int i = 0;

    for (i = 0; i < ARRAY_SIZE(et_c032if_i2c_device); i++) {
        platform_device_unregister(&et_c032if_i2c_device[i]);
    }
    platform_device_unregister(&et_c032if_swpld2);
    platform_device_unregister(&et_c032if_swpld3);
    i2c_unregister_device(i2c_client_9548_1);
    i2c_unregister_device(i2c_client_9548_2);
    i2c_unregister_device(i2c_client_9548_3);
    i2c_unregister_device(i2c_client_9548_4);
    i2c_unregister_device(i2c_client_9548_5);
    i2c_unregister_device(i2c_client_9548_6);
    platform_device_unregister(&et_c032if_cpld_mux[0]);
    platform_device_unregister(&et_c032if_cpld);
    platform_driver_unregister(&i2c_device_driver);
    platform_driver_unregister(&swpld2_driver);
    platform_driver_unregister(&swpld3_driver);
    platform_driver_unregister(&cpld_mux_driver);
    platform_driver_unregister(&cpld_driver);
}

module_init(delta_et_c032if_platform_init);
module_exit(delta_et_c032if_platform_exit);

MODULE_DESCRIPTION("Delta et-c032if Platform Support");
MODULE_AUTHOR("Johnson Lu <johnson.lu@deltaww.com>");
MODULE_LICENSE("GPL"); 
