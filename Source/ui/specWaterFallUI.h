#pragma once
#include <JuceHeader.h>
#include "../dsp/specWaterFall.h"

class SpecWaterFallUI : public juce::Component, private juce::Timer
{
private:
	float speed = 3.0f; // 1֡10����
	SpecWaterFall& swf;

	// ��Ⱦ�õ�ͼ�񻺴�
	juce::Image waterfallImage;
	int imageHeight = 512;
	float scrollOffset = 0.0f;
	float maxColorV = 100;//�����ɫֵ
	// ��ɫӳ�䣺�Ӻ�ɫ����ɫ
	juce::Colour getColourForMagnitude(float magnitude) const
	{
		// magnitude��Χ��0-1��ӳ�䵽��ɫ(0,0,0)����ɫ(maxColorV,maxColorV,maxColorV)
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

		// ȷ��ͼ���С��ȷ
		int currentWidth = getWidth();
		if (currentWidth <= 0) currentWidth = 512;

		if (waterfallImage.getWidth() != currentWidth ||
			waterfallImage.getHeight() != imageHeight)
		{
			waterfallImage = juce::Image(juce::Image::RGB, currentWidth, imageHeight, true);
		}

		// ���µ�Ƶ��������ӵ�ͼ�񶥲����������������ƶ�
		if (dataLines > 0)
		{
			// ������ʱͼ�������浱ǰ����
			juce::Image tempImage = waterfallImage.createCopy();

			// ʹ��Graphics����ͼ���ƶ�����
			{
				juce::Graphics g(waterfallImage);
				// ���ԭͼ��
				g.fillAll(juce::Colours::black);
				// ����ʱͼ�����»���
				g.drawImageAt(tempImage, 0, (int)speed);
			} // Graphics���������ﱻ���٣��ͷ�ͼ�����Ȩ��

			// ���ڿ��԰�ȫ�ؽ������ؼ�����
			// �ڶ��������µ�Ƶ����
			const auto& spectrum = waterfallData.back(); // ʹ�����µ�Ƶ������

			for (int y = 0; y < (int)speed && y < imageHeight; ++y)
			{
				for (int x = 0; x < currentWidth; ++x)
				{
					// ��x����ӳ�䵽Ƶ������
					float specIndex = ((float)x / currentWidth) * (spectrumSize - 1);
					int idx0 = (int)specIndex;
					int idx1 = juce::jlimit(0, spectrumSize - 1, idx0 + 1);
					float frac = specIndex - idx0;

					// ���Բ�ֵ��ȡ����ֵ
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
		// ������ʱ����30fps
		startTimerHz(30);
	}

	~SpecWaterFallUI()
	{
		stopTimer();
	}

	void paint(juce::Graphics& g) override
	{
		// ��䱳��Ϊ��ɫ
		g.fillAll(juce::Colours::black);

		// �����ٲ�ͼ
		if (waterfallImage.isValid())
		{
			g.drawImageAt(waterfallImage, 0, 0);
		}
	}

	void resized() override
	{
		// �������С�ı�ʱ�����´���ͼ��
		imageHeight = getHeight();
		if (imageHeight <= 0) imageHeight = 512;

		waterfallImage = juce::Image(juce::Image::RGB, getWidth(), imageHeight, true);
		waterfallImage.clear(waterfallImage.getBounds(), juce::Colours::black);
	}

	// ���ù����ٶ�
	void setSpeed(float newSpeed)
	{
		speed = juce::jlimit(1.0f, 50.0f, newSpeed);
	}

	// ��ȡ��ǰ�ٶ�
	float getSpeed() const
	{
		return speed;
	}
};