#pragma once
#include <vector>
#include <algorithm>

struct EnvelopePoint {
    float x, y;
    EnvelopePoint(float x = 0.0f, float y = 0.5f) : x(x), y(y) {}
};

struct CurveHandle {
    float curve;  // ���߶ȣ���Χ [-1, 1]
    CurveHandle(float c = 0.0f) : curve(c) {}
};

class EnveFunc
{
private:
    std::vector<EnvelopePoint> points;
    std::vector<CurveHandle> curves;  // curves[i] ���� points[i] �� points[i+1] ֮�������

public:
    EnveFunc() {
        // ��ʼ�������̶��˵�
        points.push_back(EnvelopePoint(0.0f, 0.5f));  // ��˵�
        points.push_back(EnvelopePoint(1.0f, 0.5f));  // �Ҷ˵�
        curves.push_back(CurveHandle(0.0f));  // ��ʼֱ��
    }

    inline float func(float x) {
        if (points.size() < 2) return 0.5f;

        // ����x��[0,1]��Χ
        x = std::max(0.0f, std::min(1.0f, x));

        // �ҵ�x���ڵ�����
        for (int i = 0; i < points.size() - 1; ++i) {
            if (x >= points[i].x && x <= points[i + 1].x) {
                float t = (x - points[i].x) / (points[i + 1].x - points[i].x);
                float curve = curves[i].curve;
                float y0 = points[i].y;
                float y1 = points[i + 1].y;

                // ʹ��ָ��������ֵ
                if (std::abs(curve) < 0.001f) {
                    // ���Բ�ֵ
                    return y0 + t * (y1 - y0);
                }
                else {
                    // ָ����ֵ: y0 + (y1-y0) * (pow(10, k*t) - 1) / (pow(10, k) - 1)
                    // curve ��Χ [-1, 1] ӳ�䵽 k ��Χ [-3, 3]
                    float k = curve * 3.0f;

                    if (std::abs(k) < 0.001f) {
                        return y0 + t * (y1 - y0);
                    }
                    else {
                        float exp10k = std::pow(10.0f, k);
                        float exp10kt = std::pow(10.0f, k * t);
                        return y0 + (y1 - y0) * (exp10kt - 1.0f) / (exp10k - 1.0f);
                    }
                }
            }
        }

        return points.back().y;
    }

    // ��ȡ���е�
    const std::vector<EnvelopePoint>& getPoints() const { return points; }

    // ��ȡ�������߾��
    const std::vector<CurveHandle>& getCurves() const { return curves; }

    // ��ӵ�
    void addPoint(float x, float y) {
        x = std::max(0.001f, std::min(0.999f, x));  // ��������˵��ص�
        y = std::max(0.0f, std::min(1.0f, y));

        // �ҵ�����λ��
        auto it = std::upper_bound(points.begin(), points.end(), EnvelopePoint(x, y),
            [](const EnvelopePoint& a, const EnvelopePoint& b) { return a.x < b.x; });

        int index = std::distance(points.begin(), it);
        points.insert(it, EnvelopePoint(x, y));
        curves.insert(curves.begin() + index, CurveHandle(0.0f));
    }

    // �Ƴ��㣨�����Ƴ��˵㣩
    void removePoint(int index) {
        if (index > 0 && index < points.size() - 1) {
            points.erase(points.begin() + index);
            curves.erase(curves.begin() + index);
        }
    }

    // ���õ�λ��
    void setPointPosition(int index, float x, float y) {
        if (index >= 0 && index < points.size()) {
            y = std::max(0.0f, std::min(1.0f, y));

            // �˵��x����̶�
            if (index == 0) {
                points[index] = EnvelopePoint(0.0f, y);
            }
            else if (index == points.size() - 1) {
                points[index] = EnvelopePoint(1.0f, y);
            }
            else {
                // ȷ��x�����ڵ�֮��
                float minX = points[index - 1].x + 0.001f;
                float maxX = points[index + 1].x - 0.001f;
                x = std::max(minX, std::min(maxX, x));
                points[index] = EnvelopePoint(x, y);
            }
        }
    }

    // �������߶�
    void setCurve(int index, float curve) {
        if (index >= 0 && index < curves.size()) {
            curves[index].curve = std::max(-1.0f, std::min(1.0f, curve));
        }
    }

    // ��ȡ�����е�λ�ã����ڻ������߾����
    EnvelopePoint getCurveMidPoint(int curveIndex) {
        if (curveIndex >= 0 && curveIndex < curves.size() && curveIndex < points.size() - 1) {
            float midX = (points[curveIndex].x + points[curveIndex + 1].x) * 0.5f;
            float midY = func(midX);
            return EnvelopePoint(midX, midY);
        }
        return EnvelopePoint();
    }

    // �����������ֵ
    void processFunc(float* out, int numSamples) {
        if (!out || numSamples <= 0) return;

        for (int i = 0; i < numSamples; ++i) {
            float x = (float)i / (float)(numSamples - 1);  // ����������ӳ�䵽[0,1]
            out[i] = func(x);
        }
    }
};