#pragma once
#include <JuceHeader.h>
#include "../dsp/EnveFunc.h"

class EnveUI : public juce::Component
{
private:
    EnveFunc* enveFunc = nullptr;

    // 交互状态
    enum class InteractionMode {
        None,
        DraggingPoint,
        DraggingCurve
    };

    InteractionMode mode = InteractionMode::None;
    int dragIndex = -1;
    juce::Point<float> dragOffset;

    // 视觉参数
    static constexpr float POINT_RADIUS = 6.0f;
    static constexpr float CURVE_HANDLE_RADIUS = 4.0f;
    static constexpr float CLICK_TOLERANCE = 10.0f;

public:
    EnveUI(EnveFunc* ef) : enveFunc(ef) {
        setOpaque(false);  // 设置透明背景
    }

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;

private:
    // 坐标转换
    juce::Point<float> normalizedToScreen(float x, float y) const;
    juce::Point<float> screenToNormalized(const juce::Point<float>& screenPoint) const;

    // 碰撞检测
    int findPointAt(const juce::Point<float>& position) const;
    int findCurveHandleAt(const juce::Point<float>& position) const;

    // 绘制方法
    void drawEnvelope(juce::Graphics& g) const;
    void drawPoints(juce::Graphics& g) const;
    void drawCurveHandles(juce::Graphics& g) const;
};