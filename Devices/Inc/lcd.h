#ifndef __LCD_H
#define __LCD_H
#include "stdio.h"
#include "stdint.h"
#include "ltdc.h"
#include "dma2d.h"
/*--------------- 移植时需要更改 Start ---------------*/
#include "stm32h743xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#define LCD_Backlight_PIN (GPIO_PIN_6)
#define LCD_Backlight_PORT (GPIOH)
#define LCD_MemoryAdd (0xC0000000) // 显存的起始地址
/*--------------- 移植时需要更改  End  ---------------*/
/*--------------- LTDC配置参数 Start ---------------*/
/**
 * @note
 * 1. 如果只用单层，该参数定义为1即可，使用双层的话，需要修改为 2
 * 2. FK743M1-IIT6 核心板 使用的是外部SDRAM作为显存，起始地址0xC0000000，大小为32MB
 * 3. 显存所需空间 = 分辨率 * 每个像素所占字节数，例如 480*272的屏，使用16位色（RGB565或者AEGB1555），需要显存 480*272*2 = 261120 字节
 */
#define LCD_NUM_LAYERS (1) //	定义显示的层数，750可驱动两层显示

/**
 * @note
 * 1. 定义 layer0、layer1 的颜色格式
 * 2. 可选:
 * LTDC_PIXEL_FORMAT_RGB565
 * LTDC_PIXEL_FORMAT_ARGB1555
 * LTDC_PIXEL_FORMAT_ARGB4444
 * LTDC_PIXEL_FORMAT_RGB888
 * LTDC_PIXEL_FORMAT_ARGB8888
 */
#define ColorMode_0 (LTDC_PIXEL_FORMAT_RGB565)
#if LCD_NUM_LAYERS == 2 // 如果开启了双层，则在此处定义 layer1 的颜色格式
#define ColorMode_1 (LTDC_PIXEL_FORMAT_ARGB8888)
#endif
/*--------------- LTDC配置参数  End  ---------------*/
/*--------------- LCD定义参数 Start ---------------*/
/**
 * @brief 显示方向参数
 * @example LCD_DisplayDirection(Direction_H) ，设置屏幕横屏显示
 * @example LCD_DisplayDirection(Direction_V) ，设置屏幕竖屏显示
 */
#define Direction_H 0 // LCD横屏显示
#define Direction_V 1 // LCD竖屏显示

/** 
 * @brief 常用颜色定义
 * @note
 * 1. 这里为了方便用户使用，定义的是32位颜色，然后再通过代码自动转换成对应颜色格式所需要的的颜色
 * 2. 32位的颜色中，从高位到低位分别对应 A、R、G、B  4个颜色通道，其中A表示透明通道
 * 3. 最多可设置255级透明色，ff表示不透明，0表示完全透明
 * 4. 除非使用ARGB1555和ARGB8888等支持透明色的颜色格式，不然透明色不起作用，其中ARGB1555仅支持一位
 * 透明色，即仅有透明和不透明两种状态，ARGB4444有16级透明度，ARGB8888支持255级透明度
 * 5. 用户可以在电脑用调色板获取24位RGB颜色，然后再补充透明通道得到32位的颜色，再将此32位颜色输入
 * LCD_SetColor()或LCD_SetBackColor()就可以显示出相应的颜色。
 * 6. 使用示例：纯蓝色的RGB值为0x0000FF，如果不需要透明色，则对应的32位颜色值为 0xff0000FF
 * 7. 以下定义的颜色都设置为不透明，用户可根据需求自行定义对应的颜色
 */
#define LCD_WHITE 0xffFFFFFF     // 纯白色
#define LCD_BLACK 0xff000000     // 纯黑色
#define LCD_BLUE 0xff0000FF      // 纯蓝色
#define LCD_GREEN 0xff00FF00     // 纯绿色
#define LCD_RED 0xffFF0000       // 纯红色
#define LCD_CYAN 0xff00FFFF      // 蓝绿色
#define LCD_MAGENTA 0xffFF00FF   // 紫红色
#define LCD_YELLOW 0xffFFFF00    // 黄色
#define LCD_GREY 0xff2C2C2C      // 灰色
#define LIGHT_BLUE 0xff8080FF    // 亮蓝色
#define LIGHT_GREEN 0xff80FF80   // 亮绿色
#define LIGHT_RED 0xffFF8080     // 亮红色
#define LIGHT_CYAN 0xff80FFFF    // 亮蓝绿色
#define LIGHT_MAGENTA 0xffFF80FF // 亮紫红色
#define LIGHT_YELLOW 0xffFFFF80  // 亮黄色
#define LIGHT_GREY 0xffA3A3A3    // 亮灰色
#define DARK_BLUE 0xff000080     // 暗蓝色
#define DARK_GREEN 0xff008000    // 暗绿色
#define DARK_RED 0xff800000      // 暗红色
#define DARK_CYAN 0xff008080     // 暗蓝绿色
#define DARK_MAGENTA 0xff800080  // 暗紫红色
#define DARK_YELLOW 0xff808000   // 暗黄色
#define DARK_GREY 0xff404040     // 暗灰色

/* LCD相关参数，根据屏幕的手册进行设置 */
#define HBP 43
#define VBP 12
#define HSW 1
#define VSW 1
#define HFP 8
#define VFP 8
#define LCD_Width 480                 // LCD的像素长度
#define LCD_Height 272                // LCD的像素宽度

/* layer0 每个像素所占字节 */
#if (ColorMode_0 == LTDC_PIXEL_FORMAT_RGB565 || ColorMode_0 == LTDC_PIXEL_FORMAT_ARGB1555 || ColorMode_0 == LTDC_PIXEL_FORMAT_ARGB4444)
#define BytesPerPixel_0 2 // 16位色模式每个像素占2字节
#elif ColorMode_0 == LTDC_PIXEL_FORMAT_RGB888
#define BytesPerPixel_0 3 // 24位色模式每个像素占3字节
#elif ColorMode_0 == LTDC_PIXEL_FORMAT_ARGB8888
#define BytesPerPixel_0 4 // 32位色模式每个像素占4字节
#endif

/* layer1 每个像素所占字节 */
#if LCD_NUM_LAYERS == 2
#if (ColorMode_1 == LTDC_PIXEL_FORMAT_RGB565 || ColorMode_1 == LTDC_PIXEL_FORMAT_ARGB1555 || ColorMode_1 == LTDC_PIXEL_FORMAT_ARGB4444)
#define BytesPerPixel_1 2 // 16位色模式每个像素占2字节
#elif ColorMode_1 == LTDC_PIXEL_FORMAT_RGB888
#define BytesPerPixel_1 3 // 24位色模式每个像素占3字节
#else
#define BytesPerPixel_1 4 // 32位色模式每个像素占4字节
#endif
#define LCD_MemoryAdd_OFFSET LCD_Width * LCD_Height * BytesPerPixel_0 // 第二层的显存的偏移地址
#endif

/* 背光引脚操作抽象化 */
#define LCD_Backlight_OFF HAL_GPIO_WritePin(LCD_Backlight_PORT, LCD_Backlight_PIN, GPIO_PIN_RESET); // 关闭背光
#define LCD_Backlight_ON HAL_GPIO_WritePin(LCD_Backlight_PORT, LCD_Backlight_PIN, GPIO_PIN_SET);    // 开启背光
/*--------------- LCD定义参数  End  ---------------*/
/*--------------- 函数声明 Start ---------------*/
/* 基本配置 */
void LCD_Init(void);
void LCD_Clear(void);                                                        // 清屏函数
void LCD_ClearRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height); // 局部清屏函数

/* 绘制设置 */
void LCD_SetLayer(uint8_t Layerx);            // 设置层
void LCD_SetColor(uint32_t Color);            // 设置画笔颜色
void LCD_SetBackColor(uint32_t Color);        // 设置背景颜色
void LCD_DisplayDirection(uint8_t direction); // 设置显示方向

/* 绘制图片 */
void LCD_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pImage);

/* 2D图形绘制函数 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint32_t color);                 // 画点
uint32_t LCD_ReadPoint(uint16_t x, uint16_t y);                             // 读点

/* 区域填充函数 */
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height); // 填充矩形
/*--------------- 函数声明  End  ---------------*/
#endif
