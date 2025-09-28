#pragma once
#include "fft.h"
#include <vector>
#include <deque>
#include <algorithm>
#include <cmath>

class SpecWaterFall
{
private:
	float pw = 0.0;

	// FFT相关
	static const int fftSize = 2048;
	static const int fftOrder = 10; // 2^10 = 1024
	std::vector<float> fftReal;
	std::vector<float> fftImag;
	std::vector<float> window;
	std::vector<float> inputBuffer;
	std::vector<float> magnitudeSpectrum;

	// 瀑布图数据存储
	std::deque<std::vector<float>> waterfallData;
	int maxWaterfallLines = 1024; // 最大保存的频谱线数

	int inputBufferPos = 0;
	bool needsFFT = false;

	// 初始化窗函数（汉宁窗）
	void initWindow()
	{
		window.resize(fftSize);
		for (int i = 0; i < fftSize; ++i)
		{
			window[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (fftSize - 1)));
		}
	}

	// 频率拉伸函数
	float stretchFrequency(float x) const
	{
		if (std::abs(pw) > 0.001f)
		{
			return (std::exp(x * pw) - 1.0f) / (std::exp(pw) - 1.0f);
		}
		return x;
	}

	// 执行FFT并计算幅度谱
	void performFFT()
	{
		// 应用窗函数
		for (int i = 0; i < fftSize; ++i)
		{
			fftReal[i] = inputBuffer[i] * window[i];
			fftImag[i] = 0.0f;
		}

		// 执行FFT
		fft_f32(fftReal, fftImag, fftSize, 1);

		// 计算幅度谱（只取前一半，因为FFT结果是对称的）
		magnitudeSpectrum.resize(fftSize / 2);
		for (int i = 0; i < fftSize / 2; ++i)
		{
			float magnitude = std::sqrt(fftReal[i] * fftReal[i] + fftImag[i] * fftImag[i]);
			// 转换为dB
			magnitudeSpectrum[i] = 20.0f * std::log10(magnitude + 1e-10f);
		}

		// 应用频率拉伸
		if (std::abs(pw) > 0.001f)
		{
			std::vector<float> stretchedSpectrum(fftSize / 2);
			for (int i = 0; i < fftSize / 2; ++i)
			{
				float normalizedFreq = (float)i / (fftSize / 2 - 1);
				float stretchedFreq = stretchFrequency(normalizedFreq);

				// 线性插值
				float srcIndex = stretchedFreq * (fftSize / 2 - 1);
				int idx0 = (int)srcIndex;
				int idx1 = std::min(idx0 + 1, fftSize / 2 - 1);
				float frac = srcIndex - idx0;

				stretchedSpectrum[i] = magnitudeSpectrum[idx0] * (1.0f - frac) +
					magnitudeSpectrum[idx1] * frac;
			}
			magnitudeSpectrum = std::move(stretchedSpectrum);
		}

		// 限制范围
		for (auto& val : magnitudeSpectrum)
		{
			val += 60.0;
			val /= 60.0;
			if (val < 0.0f) val = 0.0f;
			if (val > 1.0f) val = 1.0f;
		}

		// 添加到瀑布图数据
		waterfallData.push_back(magnitudeSpectrum);

		// 限制瀑布图数据量
		while (waterfallData.size() > maxWaterfallLines)
		{
			waterfallData.pop_front();
		}
	}

public:
	SpecWaterFall()
	{
		fftReal.resize(fftSize);
		fftImag.resize(fftSize);
		inputBuffer.resize(fftSize);
		initWindow();
	}

	void SetPow(float newPw)
	{
		pw = newPw;
	}

	void ProcessBlockIn(const float* in, int numSamples)
	{
		for (int i = 0; i < numSamples; ++i)
		{
			inputBuffer[inputBufferPos] = in[i];
			inputBufferPos++;

			if (inputBufferPos >= fftSize)
			{
				performFFT();

				inputBufferPos = 0;
			}
		}
	}

	// 获取瀑布图数据
	const std::deque<std::vector<float>>& getWaterfallData() const
	{
		return waterfallData;
	}

	// 获取频谱分辨率
	int getSpectrumSize() const
	{
		return fftSize / 2;
	}

	// 设置最大瀑布线数
	void setMaxWaterfallLines(int maxLines)
	{
		maxWaterfallLines = maxLines;
	}
};