#pragma once

#include "stft.h"
#include "envefunc.h"

class FastSin
{
private:
	constexpr static int table_size = 65536;
	float cos_table[table_size];
public:
	FastSin()
	{
		for (int i = 0; i < table_size; ++i)
		{
			cos_table[i] = cosf((float)i / table_size * 2.0f * M_PI);
		}
	}
	inline float fast_cos(float x)
	{
		x /= 2.0f * M_PI;
		x -= floorf(x);
		x = x * table_size;
		return cos_table[(unsigned short)x];
	}
	inline float fast_sin(float x)
	{
		return fast_cos(x - M_PI * 0.5f);
	}
};
class DelayLine
{
private:
	constexpr static int MaxDelay = 48000;
	float buf[MaxDelay] = { 0 };
	int pos = 0;
	int time = 0;
public:
	void SetDelay(int time)
	{
		this->time = time;
	}
	inline float ProcessSample(float x)
	{
		pos = (pos + 1) % time;
		float y = buf[pos];
		buf[pos] = x;
		return y;
	}
};

class SpecDrift
{
public:
	constexpr static int FFTLen = 8192;
	constexpr static int NumGroups = 24;
private:
	int windowSize = FFTLen / 1;
	int hopSize = FFTLen / 4;

	float window[FFTLen];
	void UpdateWindow()
	{
		for (int i = 0; i < windowSize; ++i)
		{
			window[i] = 0.5f - 0.5f * cosf((float)i / windowSize * 2.0f * M_PI);
		}
		for (int i = windowSize; i < FFTLen; ++i)
		{
			window[i] = 0.0f;
		}
	}

	float inbuf[FFTLen] = { 0 };
	float outbuf[NumGroups][FFTLen] = { 0 };
	int pos = 0, pos_hop = hopSize;

	std::vector<float> inre, inim;
	std::vector<std::vector<float>> outre, outim;//groups

	DelayLine dl[NumGroups];

	float ldelay = 0, rdelay = 0, t0 = 0, fb = 0;
	FastSin fs;

public:
	SpecDrift()
	{
		UpdateWindow();
		inre.resize(FFTLen, 0);
		inim.resize(FFTLen, 0);
		outre.resize(NumGroups);
		outim.resize(NumGroups);
		for (int i = 0; i < NumGroups; ++i)
		{
			outre[i].resize(FFTLen, 0);
			outim[i].resize(FFTLen, 0);
			dl[i].SetDelay(i * hopSize + 1);

			memset(outbuf[i], 0, sizeof(float) * FFTLen);
		}
		memset(inbuf, 0, sizeof(float) * FFTLen);
	}

	void ProcessBlock(const float* in, float* out, int numSamples)
	{
		float normv = (float)hopSize / FFTLen / FFTLen * NumGroups;
		for (int i = 0; i < numSamples; ++i)
		{
			inbuf[pos] = in[i];

			float outv = 0;
			for (int n = 0; n < NumGroups; ++n)
			{
				outv += dl[n].ProcessSample(outbuf[n][pos]);
				outbuf[n][pos] = 0;
			}
			out[i] = outv * normv;

			pos = (pos + 1) % FFTLen;
			pos_hop++;
			if (pos_hop >= hopSize)
			{
				pos_hop = 0;
				//int start = (FFTLen + pos - 1 - windowSize) % FFTLen;
				int start = pos;
				for (int j = 0; j < windowSize; ++j)
				{
					inre[j] = inbuf[(start + j) % FFTLen] * window[j];
					//inre[j] = inbuf[(start + j) % FFTLen];
					inim[j] = 0;
				}
				for (int j = windowSize; j < FFTLen; ++j)
				{
					inre[j] = 0;
					inim[j] = 0;
				}

				fft_f32(inre, inim, FFTLen, 1);

				ProcessSTFT(inre, inim, outre, outim, FFTLen / 2, hopSize);

				for (int n = 0; n < NumGroups; ++n)
				{
					for (int j = 0; j < FFTLen / 2; ++j)
					{
						outre[n][FFTLen - j - 1] = -outre[n][j];
						outim[n][FFTLen - j - 1] = outim[n][j];
					}
					fft_f32(outre[n], outim[n], FFTLen, -1);
					for (int j = 0; j < windowSize; ++j)
					{
						outbuf[n][(start + j) % FFTLen] += outre[n][j] * window[j];
					}
				}
			}
		}
	}

	void ProcessSTFT(std::vector<float>& inre, std::vector<float>& inim,
		std::vector<std::vector<float>>& outre, std::vector<std::vector<float>>& outim,
		int numBins, int hopSize)
	{
		for (int i = 0; i < numBins; ++i)
		{
			float t1 = ldelay + (rdelay - ldelay) * ((float)i / numBins);//µ¥Î»:sample
			int inGroup = t1 / hopSize;
			float t = t1 - inGroup * hopSize;

			float phase = 2.0f * M_PI * t * i / FFTLen;
			float mulre = fs.fast_cos(phase);
			float mulim = fs.fast_sin(phase);
			float rev = inre[i];
			float imv = inim[i];
			float re = rev * mulre - imv * mulim;
			float im = rev * mulim + imv * mulre;

			for (int n = 0; n < NumGroups; ++n)
			{
				outre[n][i] = 0;
				outim[n][i] = 0;
			}

			if (inGroup < 0) inGroup = 0;
			else if (inGroup >= NumGroups) inGroup = NumGroups - 1;
			outre[inGroup][i] = re;
			outim[inGroup][i] = im;
		}
	}

	void SetDelay(float ldelay, float rdelay, float t0, float fb)
	{
		this->ldelay = ldelay * hopSize * NumGroups;
		this->rdelay = rdelay * hopSize * NumGroups;
		this->t0 = t0 * hopSize;
		this->fb = fb;
	}
};

class SpecDrift2
{
public:
	constexpr static int FFTLen = 1024;
	constexpr static int NumGroups = 2048;
private:
	int windowSize = FFTLen / 1;
	int hopSize = FFTLen / 16;

	float window[FFTLen];
	void UpdateWindow()
	{
		for (int i = 0; i < windowSize; ++i)
		{
			window[i] = 0.5f - 0.5f * cosf((float)i / windowSize * 2.0f * M_PI);
		}
		for (int i = windowSize; i < FFTLen; ++i)
		{
			window[i] = 0.0f;
		}
	}

	float inbuf[FFTLen] = { 0 };
	float outbuf[FFTLen] = { 0 };
	int pos = 0, pos_hop = hopSize;

	std::vector<float> inre, inim;
	std::vector<float> outre, outim;//groups


	float ldelay = 0, rdelay = 0, t0 = 0, fb = 0;
	FastSin fs;

	EnveFunc* ef = NULL;
public:
	SpecDrift2(EnveFunc* envefunc)
	{
		UpdateWindow();
		inre.resize(FFTLen, 0);
		inim.resize(FFTLen, 0);
		outre.resize(FFTLen);
		outim.resize(FFTLen);

		this->ef = envefunc;

		memset(outbuf, 0, sizeof(float) * FFTLen);
		memset(inbuf, 0, sizeof(float) * FFTLen);
	}

	void ProcessBlock(const float* in, float* out, int numSamples)
	{
		float normv = (float)hopSize / FFTLen / FFTLen * 8;
		for (int i = 0; i < numSamples; ++i)
		{
			inbuf[pos] = in[i];

			float outv = outbuf[pos];
			outbuf[pos] = 0;

			out[i] = outv * normv;

			pos = (pos + 1) % FFTLen;
			pos_hop++;
			if (pos_hop >= hopSize)
			{
				pos_hop = 0;
				//int start = (FFTLen + pos - 1 - windowSize) % FFTLen;
				int start = pos;
				for (int j = 0; j < windowSize; ++j)
				{
					inre[j] = inbuf[(start + j) % FFTLen] * window[j];
					//inre[j] = inbuf[(start + j) % FFTLen];
					inim[j] = 0;
				}
				for (int j = windowSize; j < FFTLen; ++j)
				{
					inre[j] = 0;
					inim[j] = 0;
				}

				fft_f32(inre, inim, FFTLen, 1);

				ProcessSTFT(inre, inim, outre, outim, FFTLen / 2, hopSize);

				for (int j = 0; j < FFTLen / 2; ++j)
				{
					outre[FFTLen - j - 1] = outre[j];
					outim[FFTLen - j - 1] = -outim[j];
				}
				fft_f32(outre, outim, FFTLen, -1);
				for (int j = 0; j < windowSize; ++j)
				{
					outbuf[(start + j) % FFTLen] += outre[j] * window[j];
				}

			}
		}
	}
	float delayre[NumGroups][FFTLen] = { 0 };
	float delayim[NumGroups][FFTLen] = { 0 };
	int dlypos = 0;
	void ProcessSTFT(std::vector<float>& inre, std::vector<float>& inim,
		std::vector<float>& outre, std::vector<float>& outim,
		int numBins, int hopSize)
	{
		for (int i = 0; i < numBins; ++i)
		{
			//float t1 = ldelay + (rdelay - ldelay) * ((float)i / numBins);//µ¥Î»:sample
			float t1 = ldelay + (rdelay - ldelay) * ef->func((float)i / numBins);
			int inGroup = t1 / hopSize;
			float t = t1 - inGroup * hopSize;

			float phase = 2.0f * M_PI * t * i / FFTLen;
			float mulre = fs.fast_cos(phase);
			float mulim = fs.fast_sin(phase);
			float rev = inre[i] * mulre - inim[i] * mulim;
			float imv = inre[i] * mulim + inim[i] * mulre;

			float lastre = delayre[dlypos][i];
			float lastim = delayim[dlypos][i];

			float lvre = lastre * mulre - lastim * mulim;
			float lvim = lastre * mulim + lastim * mulre;

			delayre[(dlypos + inGroup) % NumGroups][i] = rev + lvre * fb;
			delayim[(dlypos + inGroup) % NumGroups][i] = imv + lvim * fb;


			outre[i] = lastre;
			outim[i] = lastim;
		}
		dlypos++;
		if (dlypos >= NumGroups) dlypos = 0;
	}

	void SetDelay(float ldelay, float rdelay, float t0, float fb)
	{
		this->ldelay = ldelay * hopSize * NumGroups;
		this->rdelay = rdelay * hopSize * NumGroups;
		this->t0 = t0 * hopSize;
		this->fb = fb;
	}
};