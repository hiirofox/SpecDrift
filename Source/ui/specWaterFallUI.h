#pragma once
#include <JuceHeader.h>
#include "../dsp/specWaterFall.h"

class SpecWaterFallUI : public juce::Component, private juce::Timer
{
private:
	float speed = 3.0f; // 1帧10像素
	SpecWaterFall& swf;

	// 渲染用的图像缓存
	juce::Image waterfallImage;
	int imageHeight = 512;
	float scrollOffset = 0.0f;
	float maxColorV = 100;//最大颜色值
	// 颜色映射：从黑色到白色
	juce::Colour getColourForMagnitude(float magnitude) const
	{
		// magnitude范围是0-1，映射到黑色(0,0,0)到白色(maxColorV,maxColorV,maxColorV)
		//magnitude / 2.0;
		magnitude = magnitude * magnitude * magnitude * magnitude;
		int greyValue = (int)(magnitude * maxColorV);
		if (greyValue < 0) greyValue = 0;
		if (greyValue > maxColorV) greyValue = maxColorV;
		return juce::Colour(greyValue, greyValue, greyValue);
	}

	void updateWaterfallImage()
	{
		const auto& waterfallData = swf.getWaterfallData();
		if (waterfallData.empty())
			return;

		int spectrumSize = swf.getSpectrumSize();
		int dataLines = (int)waterfallData.size();

		// 确保图像大小正确
		int currentWidth = getWidth();
		if (currentWidth <= 0) currentWidth = 512;

		if (waterfallImage.getWidth() != currentWidth ||
			waterfallImage.getHeight() != imageHeight)
		{
			waterfallImage = juce::Image(juce::Image::RGB, currentWidth, imageHeight, true);
		}

		// 将新的频谱数据添加到图像顶部，其他数据向下移动
		if (dataLines > 0)
		{
			// 创建临时图像来保存当前内容
			juce::Image tempImage = waterfallImage.createCopy();

			// 使用Graphics进行图像移动操作
			{
				juce::Graphics g(waterfallImage);
				// 清空原图像
				g.fillAll(juce::Colours::black);
				// 将临时图像向下绘制
				g.drawImageAt(tempImage, 0, (int)speed);
			} // Graphics对象在这里被销毁，释放图像访问权限

			// 现在可以安全地进行像素级操作
			// 在顶部绘制新的频谱线
			const auto& spectrum = waterfallData.back(); // 使用最新的频谱数据

			for (int y = 0; y < (int)speed && y < imageHeight; ++y)
			{
				for (int x = 0; x < currentWidth; ++x)
				{
					// 将x坐标映射到频谱索引
					float specIndex = ((float)x / currentWidth) * (spectrumSize - 1);
					int idx0 = (int)specIndex;
					int idx1 = juce::jlimit(0, spectrumSize - 1, idx0 + 1);
					float frac = specIndex - idx0;

					// 线性插值获取幅度值
					float magnitude = spectrum[idx0] * (1.0f - frac) + spectrum[idx1] * frac;

					juce::Colour pixelColour = getColourForMagnitude(magnitude);
					waterfallImage.setPixelAt(x, y, pixelColour);
				}
			}
		}
	}

	void timerCallback() override
	{
		updateWaterfallImage();
		repaint();
	}

public:
	SpecWaterFallUI(SpecWaterFall& swfRef) : swf(swfRef)
	{
		// 启动定时器，30fps
		startTimerHz(30);
	}

	~SpecWaterFallUI()
	{
		stopTimer();
	}

	void paint(juce::Graphics& g) override
	{
		// 填充背景为黑色
		g.fillAll(juce::Colours::black);

		// 绘制瀑布图
		if (waterfallImage.isValid())
		{
			g.drawImageAt(waterfallImage, 0, 0);
		}
	}

	void resized() override
	{
		// 当组件大小改变时，重新创建图像
		imageHeight = getHeight();
		if (imageHeight <= 0) imageHeight = 512;

		waterfallImage = juce::Image(juce::Image::RGB, getWidth(), imageHeight, true);
		waterfallImage.clear(waterfallImage.getBounds(), juce::Colours::black);
	}

	// 设置滚动速度
	void setSpeed(float newSpeed)
	{
		speed = juce::jlimit(1.0f, 50.0f, newSpeed);
	}

	// 获取当前速度
	float getSpeed() const
	{
		return speed;
	}
};