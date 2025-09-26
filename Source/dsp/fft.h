#pragma once

#include <math.h>
#include <vector>

void fft_f32(float* are, float* aim, int n, int inv);
void fft_f32(std::vector<float>& are, std::vector<float>& aim, int n, int inv);

template<typename sample>
class FFT
{
private:
	sample w_re;
	sample w_im;
public:
	void fft_f32(sample* are, sample* aim, int n, int inv)
	{
		if (w_re.size() < are[0].size())//·À±¬
		{
			w_re.resize(are[0].size());
			w_im.resize(are[0].size());
		}

		for (int i = 1, j = 0; i < n - 1; ++i)
		{
			for (int s = n; j ^= s >>= 1, ~j & s;)
				;
			if (i < j)
			{
				sample tmp = are[i];
				are[i] = are[j];
				are[j] = tmp;
				tmp = aim[i];
				aim[i] = aim[j];
				aim[j] = tmp;
			}
		}

		int p1, p2;
		for (int m = 2; m <= n; m <<= 1)
		{
			float wm_re = cosf(2.0f * M_PI / m);
			float wm_im = sinf(2.0f * M_PI / m) * inv;

			for (int k = 0; k < n; k += m)
			{
				w_re = 1.0f;
				w_im = 0.0f;
				p1 = m >> 1;

				for (int j = 0; j < p1; ++j)
				{
					p2 = k + j;
					// ¼ÆËã t = w * a[p2 + p1]
					sample t_re = w_re * are[p2 + p1] - w_im * aim[p2 + p1];
					sample t_im = w_re * aim[p2 + p1] + w_im * are[p2 + p1];

					// ¸üÐÂ a[p2] ºÍ a[p2 + p1]
					are[p2 + p1] = are[p2] - t_re;
					aim[p2 + p1] = aim[p2] - t_im;
					are[p2] += t_re;
					aim[p2] += t_im;

					// ¸üÐÂ w = w * wm
					t_re = w_re * wm_re - w_im * wm_im;
					t_im = w_re * wm_im + w_im * wm_re;
					w_re = t_re;
					w_im = t_im;
				}
			}
		}
	}
};