#include "lcd.h"

extern DMA2D_HandleTypeDef hdma2d; // DMA2D句柄
extern LTDC_HandleTypeDef hltdc;	// LTDC句柄

/* LCD相关参数结构体 */
struct
{
	uint32_t Color;			 //	LCD当前画笔颜色
	uint32_t BackColor;		 //	背景色
	uint32_t ColorMode;		 // 颜色格式
	uint32_t LayerMemoryAdd; //	层显存地址
	uint8_t Layer;			 //	当前层
	uint8_t Direction;		 //	显示方向
	uint8_t BytesPerPixel;	 // 每个像素所占字节数
} LCD;

/**
 * @brief LCD初始化函数，包括LTDC配置，layer配置，LCD配置
 */
void LCD_Init(void)
{
	LCD_Backlight_OFF; // 先关闭背光引脚，初始化LTDC之后再开启

	/* ltdc配置 */
	__HAL_RCC_DMA2D_CLK_ENABLE();		  // 使能DMA2D时钟
	hltdc.Instance = LTDC;
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;	 // 低电平有效
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;	 // 低电平有效
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;	 // 低电平有效，要注意的是，很多面板都是高电平有效，但是743需要设置成低电平才能正常显示
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC; // 正常时钟信号
	hltdc.Init.HorizontalSync = HSW - 1;		 // 根据屏幕设置参数即可
	hltdc.Init.VerticalSync = VSW - 1;
	hltdc.Init.AccumulatedHBP = HBP + HSW - 1;
	hltdc.Init.AccumulatedVBP = VBP + VSW - 1;
	hltdc.Init.AccumulatedActiveW = LCD_Width + HSW + HBP - 1;
	hltdc.Init.AccumulatedActiveH = LCD_Height + VSW + VBP - 1;
	hltdc.Init.TotalWidth = LCD_Width + HSW + HBP + HFP - 1;
	hltdc.Init.TotalHeigh = LCD_Height + VSW + VBP + VFP - 1;
	hltdc.Init.Backcolor.Red = 0;	// 初始背景色，R
	hltdc.Init.Backcolor.Green = 0; // 初始背景色，G
	hltdc.Init.Backcolor.Blue = 0;	// 初始背景色，B
	HAL_LTDC_Init(&hltdc);			// 初始化LTDC参数
	
	/* layer0显示配置 */
	LTDC_LayerCfgTypeDef pLayerCfg = {0}; // layer0 相关参数
	pLayerCfg.WindowX0 = 0;								  // 水平起点
	pLayerCfg.WindowX1 = LCD_Width;						  // 水平终点
	pLayerCfg.WindowY0 = 0;								  // 垂直起点
	pLayerCfg.WindowY1 = LCD_Height;					  // 垂直终点
	pLayerCfg.ImageWidth = LCD_Width;					  // 显示区域宽度
	pLayerCfg.ImageHeight = LCD_Height;					  // 显示区域高度
	pLayerCfg.PixelFormat = ColorMode_0;				  // 颜色格式
	pLayerCfg.Alpha = 255;								  // 取值范围0~255，255表示不透明，0表示完全透明
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA; // 混合系数1
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA; // 混合系数2
	pLayerCfg.FBStartAdress = LCD_MemoryAdd;			  // 显存地址
	pLayerCfg.Alpha0 = 0;								  // 初始颜色，A
	pLayerCfg.Backcolor.Blue = 0;						  // 初始颜色，R
	pLayerCfg.Backcolor.Green = 0;						  // 初始颜色，G
	pLayerCfg.Backcolor.Red = 0;						  // 初始颜色，B
	HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);		  // 配置layer0

#if ((ColorMode_0 == LTDC_PIXEL_FORMAT_RGB888) || (ColorMode_0 == LTDC_PIXEL_FORMAT_ARGB8888)) // 判断是否使用24位或者32位色
	/**
	 * @note 因为743每个通道的低位都是采用伪随机抖动输出，如果不开启颜色抖动，则无法正常显示24位或者32位色
	 */
	HAL_LTDC_EnableDither(&hltdc); // 开启颜色抖动
#endif

	/* layer1 显示配置 */
#if (LCD_NUM_LAYERS == 2) // 如果定义了双层
	LTDC_LayerCfgTypeDef pLayerCfg1 = {0};
	pLayerCfg1.WindowX0 = 0;																   // 水平起点
	pLayerCfg1.WindowX1 = LCD_Width;														   // 水平终点
	pLayerCfg1.WindowY0 = 0;																   // 垂直起点
	pLayerCfg1.WindowY1 = LCD_Height;														   // 垂直终点
	pLayerCfg1.ImageWidth = LCD_Width;														   // 显示区域宽度
	pLayerCfg1.ImageHeight = LCD_Height;													   // 显示区域高度
	pLayerCfg1.PixelFormat = ColorMode_1;													   // 颜色格式，layer1 应配置为带有透明色的格式，例如ARGB8888或ARGB1555
	pLayerCfg1.Alpha = 255;																	   // 取值范围0~255，255表示不透明，0表示完全透明
	pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;								   // 混合系数1
	pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;								   // 混合系数2
	pLayerCfg1.FBStartAdress = LCD_MemoryAdd + LCD_MemoryAdd_OFFSET;						   // 显存地址
	pLayerCfg1.Alpha0 = 0;																	   // 初始颜色，A
	pLayerCfg1.Backcolor.Red = 0;															   //	初始颜色，R
	pLayerCfg1.Backcolor.Green = 0;															   //	初始颜色，G
	pLayerCfg1.Backcolor.Blue = 0;															   //	初始颜色，B
	HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1);											   // 初始化 layer1 的配置
#if ((ColorMode_1 == LTDC_PIXEL_FORMAT_RGB888) || (ColorMode_1 == LTDC_PIXEL_FORMAT_ARGB8888)) // 判断是否使用24位或者32位色
	/**
	 * @note 因为743每个通道的低位都是采用伪随机抖动输出，如果不开启颜色抖动，则无法正常显示24位或者32位色
	 */
	HAL_LTDC_EnableDither(&hltdc); // 开启颜色抖动
#endif
#endif

	/* 显示初始化 */

	/* 初始化一些默认配置 */
	LCD_DisplayDirection(Direction_H); // 设置横屏显示
	LCD_SetLayer(0);			 // 切换到 layer0
	LCD_SetBackColor(LCD_BLACK); //	设置背景色
	LCD_SetColor(LCD_WHITE);	 //	设置画笔颜色
	LCD_Clear();				 //	清屏，刷背景色
#if LCD_NUM_LAYERS == 2			 // 如果开启了两层显示
	LCD_SetLayer(1);			 // 切换到前景层 layer1，layer1始终在layer0之上
	LCD_SetBackColor(LCD_BLACK); //	设置背景色
	LCD_SetColor(LCD_WHITE);	 //	设置画笔颜色
	LCD_Clear();				 //	清屏，刷背景色
#endif

	/* 开启背光 */
	LCD_Backlight_ON; 
}

/**
 * @brief 设置要显示和操作的层，切换相应的显存地址、颜色格式等
 * @param layer 要显示和操作的层，可以设置为0或1，即选择 layer0 或 layer1
 * @note 
 * 743的LTDC层顺序是固定的，layer1 在 layer0之上，即开启两层显示时，
 * layer1 是前景层，通常使用带透明色的颜色格式，layer0 是背景层，
 * 只开启单层时，默认只操作 layer0
 */
void LCD_SetLayer(uint8_t layer)
{
#if LCD_NUM_LAYERS == 2 // 如果开了双层

	if (layer == 0) // 如果设置的是 layer0
	{
		LCD.LayerMemoryAdd = LCD_MemoryAdd;	 // 获取 layer0 的显存地址
		LCD.ColorMode = ColorMode_0;		 // 获取 layer0 的颜色格式
		LCD.BytesPerPixel = BytesPerPixel_0; // 获取 layer0 的每个像素所需字节数的大小
	}
	else if (layer == 1) // 如果设置的是 layer1
	{
		LCD.LayerMemoryAdd = LCD_MemoryAdd + LCD_MemoryAdd_OFFSET; // 获取 layer1 的显存地址
		LCD.ColorMode = ColorMode_1;							   // 获取 layer1 的颜色格式
		LCD.BytesPerPixel = BytesPerPixel_1;					   // 获取 layer1 的每个像素所需字节数的大小
	}
	LCD.Layer = layer; // 记录当前所在的层

#else // 如果只开启单层，默认操作 layer0
	LCD.LayerMemoryAdd = LCD_MemoryAdd;	 // 获取 layer0 的显存地址
	LCD.ColorMode = ColorMode_0;		 // 获取 layer0 的颜色格式
	LCD.BytesPerPixel = BytesPerPixel_0; // 获取 layer0 的每个像素所需字节数的大小
	LCD.Layer = 0;						 // 层标记设置为 layer0
#endif
}

/**
 * @brief 设置显示颜色
 * @param Color 要显示的颜色，示例：0xff0000FF 表示不透明的蓝色，0xAA0000FF 表示透明度为66.66%的蓝色
 * @note 
 * 1. 为了方便用户使用自定义颜色，入口参数 Color 使用32位的颜色格式，用户无需关心颜色格式的转换
 * 2. 32位的颜色中，从高位到低位分别对应 A、R、G、B  4个颜色通道
 * 3. 高8位的透明通道中，ff表示不透明，0表示完全透明
 * 4. 除非使用ARGB1555和ARGB8888等支持透明色的颜色格式，不然透明色不起作用，其中ARGB1555仅支持一位
 * 透明色，即仅有透明和不透明两种状态，ARGB8888支持255级透明度
 * 5. 这里说到的透明，是指 背景层、layer0和layer1 之间的透明
 */
void LCD_SetColor(uint32_t Color)
{
	uint16_t Alpha_Value = 0, Red_Value = 0, Green_Value = 0, Blue_Value = 0; // 各个颜色通道的值

	if (LCD.ColorMode == LTDC_PIXEL_FORMAT_RGB565) // 将32位色转换为16位色
	{
		Red_Value = (uint16_t)((Color & 0x00F80000) >> 8);
		Green_Value = (uint16_t)((Color & 0x0000FC00) >> 5);
		Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);
		LCD.Color = (uint16_t)(Red_Value | Green_Value | Blue_Value);
	}
	else if (LCD.ColorMode == LTDC_PIXEL_FORMAT_ARGB1555) // 将32位色转换为ARGB1555颜色
	{
		if ((Color & 0xFF000000) == 0) // 判断是否使用透明色
			Alpha_Value = 0x0000;
		else
			Alpha_Value = 0x8000;

		Red_Value = (uint16_t)((Color & 0x00F80000) >> 9);
		Green_Value = (uint16_t)((Color & 0x0000F800) >> 6);
		Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);
		LCD.Color = (uint16_t)(Alpha_Value | Red_Value | Green_Value | Blue_Value);
	}
	else if (LCD.ColorMode == LTDC_PIXEL_FORMAT_ARGB4444) // 将32位色转换为ARGB4444颜色
	{

		Alpha_Value = (uint16_t)((Color & 0xf0000000) >> 16);
		Red_Value = (uint16_t)((Color & 0x00F00000) >> 12);
		Green_Value = (uint16_t)((Color & 0x0000F000) >> 8);
		Blue_Value = (uint16_t)((Color & 0x000000F8) >> 4);
		LCD.Color = (uint16_t)(Alpha_Value | Red_Value | Green_Value | Blue_Value);
	}
	else
		LCD.Color = Color; // 24位色或32位色不需要转换
}

/**
 * @brief 设置背景色,此函数用于清屏以及显示字符的背景色
 * @param Color 要显示的颜色，示例：0xff0000FF 表示不透明的蓝色，0xAA0000FF 表示透明度为66.66%的蓝色
 * @note 
 * 1. 为了方便用户使用自定义颜色，入口参数 Color 使用32位的颜色格式，用户无需关心颜色格式的转换
 * 2. 32位的颜色中，从高位到低位分别对应 A、R、G、B  4个颜色通道
 * 3. 高8位的透明通道中，ff表示不透明，0表示完全透明
 * 4. 除非使用ARGB1555和ARGB8888等支持透明色的颜色格式，不然透明色不起作用，其中ARGB1555仅支持一位
 * 透明色，即仅有透明和不透明两种状态，ARGB8888支持255级透明度
 * 5. 这里说到的透明，是指 背景层、layer0和layer1之间的透明
 */
void LCD_SetBackColor(uint32_t Color)
{
	uint16_t Alpha_Value = 0, Red_Value = 0, Green_Value = 0, Blue_Value = 0; // 各个颜色通道的值

	if (LCD.ColorMode == LTDC_PIXEL_FORMAT_RGB565) // 将32位色转换为16位色
	{
		Red_Value = (uint16_t)((Color & 0x00F80000) >> 8);
		Green_Value = (uint16_t)((Color & 0x0000FC00) >> 5);
		Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);
		LCD.BackColor = (uint16_t)(Red_Value | Green_Value | Blue_Value);
	}
	else if (LCD.ColorMode == LTDC_PIXEL_FORMAT_ARGB1555) // 将32位色转换为ARGB1555颜色
	{
		if ((Color & 0xFF000000) == 0) // 判断是否使用透明色
			Alpha_Value = 0x0000;
		else
			Alpha_Value = 0x8000;

		Red_Value = (uint16_t)((Color & 0x00F80000) >> 9);
		Green_Value = (uint16_t)((Color & 0x0000F800) >> 6);
		Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);
		LCD.BackColor = (uint16_t)(Alpha_Value | Red_Value | Green_Value | Blue_Value);
	}
	else if (LCD.ColorMode == LTDC_PIXEL_FORMAT_ARGB4444) // 将32位色转换为ARGB4444颜色
	{

		Alpha_Value = (uint16_t)((Color & 0xf0000000) >> 16);
		Red_Value = (uint16_t)((Color & 0x00F00000) >> 12);
		Green_Value = (uint16_t)((Color & 0x0000F000) >> 8);
		Blue_Value = (uint16_t)((Color & 0x000000F8) >> 4);
		LCD.BackColor = (uint16_t)(Alpha_Value | Red_Value | Green_Value | Blue_Value);
	}

	else
		LCD.BackColor = Color; // 24位色或32位色不需要转换
}

/**
 * @brief 设置要显示的方向，可输入参数 Direction_H 代表横屏显示，Direction_V 代表竖直显示
 * @param direction 要显示的方向
 * @note 使用示例 LCD_DisplayDirection(Direction_H) ，即设置屏幕横屏显示
 */
void LCD_DisplayDirection(uint8_t direction)
{
	LCD.Direction = direction;
}

/**
 * @brief 清屏函数，将LCD清除为 LCD.BackColor 的颜色，使用DMA2D实现
 * @note 先用 LCD_SetBackColor() 设置要清除的背景色，再调用该函数清屏即可
 */
void LCD_Clear(void)
{
	DMA2D->CR &= ~(DMA2D_CR_START);				   // 停止DMA2D
	DMA2D->CR = DMA2D_R2M;						   // 寄存器到SDRAM
	DMA2D->OPFCCR = LCD.ColorMode;				   // 设置颜色格式
	DMA2D->OOR = 0;								   // 设置行偏移
	DMA2D->OMAR = LCD.LayerMemoryAdd;			   // 地址
	DMA2D->NLR = (LCD_Width << 16) | (LCD_Height); // 设定长度和宽度
	DMA2D->OCOLR = LCD.BackColor;				   // 颜色

	/**
	 * @note
	 * 用户也可以使用 寄存器重载中断进行判断
	 * 除非是对速度要求特别高的场合，不然建议加上判断垂直等待的语句，可以避免撕裂效应
	 */
	while (LTDC->CDSR != 0X00000001)
		; // 判断 显示状态寄存器LTDC_CDSR 的第0位 VDES：垂直数据使能显示状态

	DMA2D->CR |= DMA2D_CR_START; //	启动DMA2D

	while (DMA2D->CR & DMA2D_CR_START)
		; //	等待传输完成
}

/**
 * @brief 局部清屏函数，将指定位置对应的区域清除为 LCD.BackColor 的颜色
 * @param x 起始水平坐标，取值范围0~479
 * @param y 起始垂直坐标，取值范围0~271
 * @param width  要清除区域的横向长度
 * @param height 要清除区域的纵向宽度
 * @note 先用 LCD_SetBackColor() 设置要清除的背景色，再调用该函数清屏即可
 * @example LCD_ClearRect(10, 10, 100, 50) 清除坐标(10,10)开始的长100宽50的区域
 */
void LCD_ClearRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{

	DMA2D->CR &= ~(DMA2D_CR_START); // 停止DMA2D
	DMA2D->CR = DMA2D_R2M;			// 寄存器到SDRAM
	DMA2D->OPFCCR = LCD.ColorMode;	// 设置颜色格式
	DMA2D->OCOLR = LCD.BackColor;	// 颜色

	if (LCD.Direction == Direction_H) // 横屏填充
	{
		DMA2D->OOR = LCD_Width - width;												// 设置行偏移
		DMA2D->OMAR = LCD.LayerMemoryAdd + LCD.BytesPerPixel * (LCD_Width * y + x); // 地址;
		DMA2D->NLR = (width << 16) | (height);										// 设定长度和宽度
	}
	else // 竖屏填充
	{
		DMA2D->OOR = LCD_Width - height;																	   // 设置行偏移
		DMA2D->OMAR = LCD.LayerMemoryAdd + LCD.BytesPerPixel * ((LCD_Height - x - 1 - width) * LCD_Width + y); // 地址
		DMA2D->NLR = (width) | (height << 16);																   // 设定长度和宽度
	}

	DMA2D->CR |= DMA2D_CR_START; //	启动DMA2D

	while (DMA2D->CR & DMA2D_CR_START)
		; // 等待传输完成
}

/**
 * @brief 在指定坐标绘制指定颜色的点
 * @param x 起始水平坐标，取值范围0~479
 * @param y 起始垂直坐标，取值范围0~271
 * @note 1.直接在对应的显存位置写入颜色值，即可实现画点的功能
 * @note 2.使用示例 LCD_DrawPoint(10, 10, 0xff0000FF)，在坐标(10,10)绘制蓝色的点
 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint32_t color)
{

	/*----------------------- 32位色 ARGB8888 模式 ----------------------*/

	if (LCD.ColorMode == LTDC_PIXEL_FORMAT_ARGB8888)
	{
		if (LCD.Direction == Direction_H) // 水平方向
		{
			*(__IO uint32_t *)(LCD.LayerMemoryAdd + 4 * (x + y * LCD_Width)) = color;
		}
		else if (LCD.Direction == Direction_V) // 垂直方向
		{
			*(__IO uint32_t *)(LCD.LayerMemoryAdd + 4 * ((LCD_Height - x - 1) * LCD_Width + y)) = color;
		}
	}
	/*----------------------------- 24位色 RGB888 模式 -------------------------*/

	else if (LCD.ColorMode == LTDC_PIXEL_FORMAT_RGB888)
	{
		if (LCD.Direction == Direction_H) // 水平方向
		{
			*(__IO uint16_t *)(LCD.LayerMemoryAdd + 3 * (x + y * LCD_Width)) = color;
			*(__IO uint8_t *)(LCD.LayerMemoryAdd + 3 * (x + y * LCD_Width) + 2) = color >> 16;
		}
		else if (LCD.Direction == Direction_V) // 垂直方向
		{
			*(__IO uint16_t *)(LCD.LayerMemoryAdd + 3 * ((LCD_Height - x - 1) * LCD_Width + y)) = color;
			*(__IO uint8_t *)(LCD.LayerMemoryAdd + 3 * ((LCD_Height - x - 1) * LCD_Width + y) + 2) = color >> 16;
		}
	}

	/*----------------------- 16位色 ARGB1555、RGB565或者ARGB4444 模式 ----------------------*/
	else
	{
		if (LCD.Direction == Direction_H) // 水平方向
		{
			*(__IO uint16_t *)(LCD.LayerMemoryAdd + 2 * (x + y * LCD_Width)) = color;
		}
		else if (LCD.Direction == Direction_V) // 垂直方向
		{
			*(__IO uint16_t *)(LCD.LayerMemoryAdd + 2 * ((LCD_Height - x - 1) * LCD_Width + y)) = color;
		}
	}
}

/**
 * @brief 读取指定坐标点的颜色，在使用16或24位色模式时，读出来的颜色数据对应为16位或24位
 * @param x 水平坐标，取值范围0~479
 * @param y 垂直坐标，取值范围0~271
 * @note 1.直接读取对应的显存值，即可实现读点的功能
 * @note 2.使用示例 color = LCD_ReadPoint(10, 10)，color为读取到的坐标点(10,10)的颜色
 */
uint32_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
	uint32_t color = 0;

	/*----------------------- 32位色 ARGB8888 模式 ----------------------*/
	if (LCD.ColorMode == LTDC_PIXEL_FORMAT_ARGB8888)
	{
		if (LCD.Direction == Direction_H) // 水平方向
		{
			color = *(__IO uint32_t *)(LCD.LayerMemoryAdd + 4 * (x + y * LCD_Width));
		}
		else if (LCD.Direction == Direction_V) // 垂直方向
		{
			color = *(__IO uint32_t *)(LCD.LayerMemoryAdd + 4 * ((LCD_Height - x - 1) * LCD_Width + y));
		}
	}

	/*----------------------------- 24位色 RGB888 模式 -------------------------*/
	else if (LCD.ColorMode == LTDC_PIXEL_FORMAT_RGB888)
	{
		if (LCD.Direction == Direction_H) // 水平方向
		{
			color = *(__IO uint32_t *)(LCD.LayerMemoryAdd + 3 * (x + y * LCD_Width)) & 0x00ffffff;
		}
		else if (LCD.Direction == Direction_V) // 垂直方向
		{
			color = *(__IO uint32_t *)(LCD.LayerMemoryAdd + 3 * ((LCD_Height - x - 1) * LCD_Width + y)) & 0x00ffffff;
		}
	}

	/*----------------------- 16位色 ARGB1555、RGB565或者ARGB4444 模式 ----------------------*/
	else
	{
		if (LCD.Direction == Direction_H) // 水平方向
		{
			color = *(__IO uint16_t *)(LCD.LayerMemoryAdd + 2 * (x + y * LCD_Width));
		}
		else if (LCD.Direction == Direction_V) // 垂直方向
		{
			color = *(__IO uint16_t *)(LCD.LayerMemoryAdd + 2 * ((LCD_Height - x - 1) * LCD_Width + y));
		}
	}
	return color;
}

/**
 * @brief 在坐标 (x,y) 绘制指定长宽的图片
 * @param x 水平坐标，取值范围 0~479
 * @param y 垂直坐标，取值范围 0~271
 * @param width 图片的宽度，最大取值480
 * @param height 图片的高度，最大取值272
 * @param *pImage 图片数据存储区的首地址
 * @note 要显示的图片需要事先进行取模，且只能显示一种颜色，使用 LCD_SetColor() 函数设置画笔色
 */
void LCD_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pImage)
{
	uint8_t disChar;	   // 字模的值
	uint16_t Xaddress = x; // 水平坐标
	uint16_t i = 0, j = 0, m = 0;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < (float)width / 8; j++)
		{
			disChar = *pImage;

			for (m = 0; m < 8; m++)
			{
				if (disChar & 0x01)
				{
					LCD_DrawPoint(Xaddress, y, LCD.Color); // 当前模值不为0时，使用画笔色绘点
				}
				else
				{
					LCD_DrawPoint(Xaddress, y, LCD.BackColor); // 否则使用背景色绘制点
				}
				disChar >>= 1;
				Xaddress++; // 水平坐标自加

				if ((Xaddress - x) == width) // 如果水平坐标达到了字符宽度，则退出当前循环
				{							 // 进入下一行的绘制
					Xaddress = x;
					y++;
					break;
				}
			}
			pImage++;
		}
	}
}

/**
 * @brief 在坐标 (x,y) 填充指定长宽的实心矩形
 * @param x 水平坐标，取值范围 0~479
 * @param y 垂直坐标，取值范围 0~271
 * @param width 图片的水平宽度，最大取值480
 * @param height 图片的垂直宽度，最大取值272
 * @note 
 * 1.使用DMA2D实现
 * 2.要绘制的区域不能超过屏幕的显示区域
 */
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{

	DMA2D->CR &= ~(DMA2D_CR_START); // 停止DMA2D
	DMA2D->CR = DMA2D_R2M;			// 寄存器到SDRAM
	DMA2D->OPFCCR = LCD.ColorMode;	// 设置颜色格式
	DMA2D->OCOLR = LCD.Color;		// 颜色

	if (LCD.Direction == Direction_H) // 横屏填充
	{
		DMA2D->OOR = LCD_Width - width;												// 设置行偏移
		DMA2D->OMAR = LCD.LayerMemoryAdd + LCD.BytesPerPixel * (LCD_Width * y + x); // 地址;
		DMA2D->NLR = (width << 16) | (height);										// 设定长度和宽度
	}
	else // 竖屏填充
	{
		DMA2D->OOR = LCD_Width - height;																	   // 设置行偏移
		DMA2D->OMAR = LCD.LayerMemoryAdd + LCD.BytesPerPixel * ((LCD_Height - x - 1 - width) * LCD_Width + y); // 地址
		DMA2D->NLR = (width) | (height << 16);																   // 设定长度和宽度
	}

	DMA2D->CR |= DMA2D_CR_START; // 启动DMA2D

	while (DMA2D->CR & DMA2D_CR_START)
		; // 等待传输完成
}
