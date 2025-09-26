#pragma once
#include <JuceHeader.h>
#include "../dsp/EnveFunc.h"

class EnveUI : public juce::Component
{
private:
    EnveFunc* enveFunc = nullptr;

    // ����״̬
    enum class InteractionMode {
        None,
        DraggingPoint,
        DraggingCurve
    };

    InteractionMode mode = InteractionMode::None;
    int dragIndex = -1;
    juce::Point<float> dragOffset;

    // �Ӿ�����
    static constexpr float POINT_RADIUS = 6.0f;
    static constexpr float CURVE_HANDLE_RADIUS = 4.0f;
    static constexpr float CLICK_TOLERANCE = 10.0f;

public:
    EnveUI(EnveFunc* ef) : enveFunc(ef) {
        setOpaque(false);  // ����͸������
    }

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;

private:
    // ����ת��
    juce::Point<float> normalizedToScreen(float x, float y) const;
    juce::Point<float> screenToNormalized(const juce::Point<float>& screenPoint) const;

    // ��ײ���
    int findPointAt(const juce::Point<float>& position) const;
    int findCurveHandleAt(const juce::Point<float>& position) const;

    // ���Ʒ���
    void drawEnvelope(juce::Graphics& g) const;
    void drawPoints(juce::Graphics& g) const;
    void drawCurveHandles(juce::Graphics& g) const;
};