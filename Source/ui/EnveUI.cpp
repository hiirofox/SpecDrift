#include "EnveUI.h"

void EnveUI::paint(juce::Graphics& g)
{
	if (!enveFunc) return;

	int w = getWidth();
	int h = getHeight();

	// ���ư�������
	drawEnvelope(g);

	// �������ߵ��ھ��
	drawCurveHandles(g);

	// ���ƽڵ�
	drawPoints(g);

	// ���Ʊ߿�
	g.setColour(juce::Colour(0xff00ff00));  // ��ɫ�߿�
	g.drawRect(juce::Rectangle<int>(0, 0, w, h), 2.5f);
}

void EnveUI::mouseDown(const juce::MouseEvent& event)
{
	if (!enveFunc) return;

	juce::Point<float> mousePos = event.position;

	// ����Ƿ����˽ڵ�
	int pointIndex = findPointAt(mousePos);
	if (pointIndex != -1) {
		mode = InteractionMode::DraggingPoint;
		dragIndex = pointIndex;
		juce::Point<float> screenPoint = normalizedToScreen(
			enveFunc->getPoints()[pointIndex].x,
			enveFunc->getPoints()[pointIndex].y
		);
		dragOffset = mousePos - screenPoint;
		return;
	}

	// ����Ƿ��������߾��
	int curveIndex = findCurveHandleAt(mousePos);
	if (curveIndex != -1) {
		mode = InteractionMode::DraggingCurve;
		dragIndex = curveIndex;
		return;
	}

	mode = InteractionMode::None;
}

void EnveUI::mouseDrag(const juce::MouseEvent& event)
{
	if (!enveFunc) return;

	if (mode == InteractionMode::DraggingPoint && dragIndex != -1) {
		juce::Point<float> normalizedPos = screenToNormalized(event.position - dragOffset);
		enveFunc->setPointPosition(dragIndex, normalizedPos.x, normalizedPos.y);
		repaint();
	}
	else if (mode == InteractionMode::DraggingCurve && dragIndex != -1) {
		// �������ߵ��� - �޸�y1<y0ʱ�ķ�������
		const auto& points = enveFunc->getPoints();
		if (dragIndex < points.size() - 1) {
			juce::Point<float> p1 = normalizedToScreen(points[dragIndex].x, points[dragIndex].y);
			juce::Point<float> p2 = normalizedToScreen(points[dragIndex + 1].x, points[dragIndex + 1].y);

			// �������������߶��е�Ĵ�ֱƫ��
			juce::Point<float> lineVec = p2 - p1;
			float lineLength = lineVec.getDistanceFromOrigin();
			if (lineLength > 0) {
				// ��ȡ�߶��е�
				juce::Point<float> midPoint = (p1 + p2) * 0.5f;

				// ���㴹ֱ��������
				juce::Point<float> perpVec(-lineVec.y / lineLength, lineVec.x / lineLength);

				// ����ƫ����
				float offset = perpVec.getDotProduct(event.position - midPoint);

				// ����y��������ƫ�Ʒ���
				// ��y1 < y0ʱ���½��Σ�����Ҫ��תƫ�Ʒ����Ա���ֱ�۵Ľ���
				float y0 = points[dragIndex].y;
				float y1 = points[dragIndex + 1].y;
				if (y1 < y0) {
					offset = -offset;  // ��תƫ�Ʒ���
				}

				// ӳ�䵽���ʵ����߷�Χ�����ָ����������������
				float curve = juce::jlimit(-1.0f, 1.0f, offset / (getHeight() * 0.3f));

				enveFunc->setCurve(dragIndex, curve);
				repaint();
			}
		}
	}
}

void EnveUI::mouseUp(const juce::MouseEvent& event)
{
	mode = InteractionMode::None;
	dragIndex = -1;
}

void EnveUI::mouseDoubleClick(const juce::MouseEvent& event)
{
	if (!enveFunc) return;

	// ��˫��λ������½ڵ�
	juce::Point<float> normalizedPos = screenToNormalized(event.position);

	// ����Ƿ�����Ч��Χ��
	if (normalizedPos.x > 0.001f && normalizedPos.x < 0.999f) {
		enveFunc->addPoint(normalizedPos.x, normalizedPos.y);
		repaint();
	}
}

juce::Point<float> EnveUI::normalizedToScreen(float x, float y) const
{
	return juce::Point<float>(
		x * getWidth(),
		(1.0f - y) * getHeight()  // Y�ᷭת
	);
}

juce::Point<float> EnveUI::screenToNormalized(const juce::Point<float>& screenPoint) const
{
	return juce::Point<float>(
		screenPoint.x / getWidth(),
		1.0f - (screenPoint.y / getHeight())  // Y�ᷭת
	);
}

int EnveUI::findPointAt(const juce::Point<float>& position) const
{
	const auto& points = enveFunc->getPoints();

	for (int i = 0; i < points.size(); ++i) {
		juce::Point<float> screenPoint = normalizedToScreen(points[i].x, points[i].y);
		if (position.getDistanceFrom(screenPoint) <= CLICK_TOLERANCE) {
			return i;
		}
	}

	return -1;
}

int EnveUI::findCurveHandleAt(const juce::Point<float>& position) const
{
	const auto& points = enveFunc->getPoints();

	for (int i = 0; i < points.size() - 1; ++i) {
		EnvelopePoint midPoint = enveFunc->getCurveMidPoint(i);
		juce::Point<float> screenPoint = normalizedToScreen(midPoint.x, midPoint.y);

		// Ϊ�����߶��ṩ���߾�������۵�ǰ�Ƿ�������
		if (position.getDistanceFrom(screenPoint) <= CLICK_TOLERANCE) {
			return i;
		}
	}

	return -1;
}

void EnveUI::drawEnvelope(juce::Graphics& g) const
{
	const auto& points = enveFunc->getPoints();
	if (points.size() < 2) return;

	g.setColour(juce::Colour(0xff00ffff));  // ��ɫ����

	juce::Path envelopePath;

	// ���Ƹ߾��Ȱ�������
	const int resolution = getWidth() * 2;  // �߷ֱ��ʲ���
	bool firstPoint = true;

	for (int i = 0; i <= resolution; ++i) {
		float x = (float)i / resolution;
		float y = enveFunc->func(x);
		juce::Point<float> screenPoint = normalizedToScreen(x, y);

		if (firstPoint) {
			envelopePath.startNewSubPath(screenPoint);
			firstPoint = false;
		}
		else {
			envelopePath.lineTo(screenPoint);
		}
	}

	g.strokePath(envelopePath, juce::PathStrokeType(2.0f));
}

void EnveUI::drawPoints(juce::Graphics& g) const
{
	const auto& points = enveFunc->getPoints();

	for (int i = 0; i < points.size(); ++i) {
		juce::Point<float> screenPoint = normalizedToScreen(points[i].x, points[i].y);

		// ���ƽڵ�
		g.setColour(juce::Colour(0xff00ffff));  // ��ɫ
		g.fillEllipse(screenPoint.x - POINT_RADIUS, screenPoint.y - POINT_RADIUS,
			POINT_RADIUS * 2, POINT_RADIUS * 2);

		// ���ƽڵ�߿�
		g.setColour(juce::Colours::white);
		g.drawEllipse(screenPoint.x - POINT_RADIUS, screenPoint.y - POINT_RADIUS,
			POINT_RADIUS * 2, POINT_RADIUS * 2, 1.0f);
	}
}

void EnveUI::drawCurveHandles(juce::Graphics& g) const
{
	const auto& points = enveFunc->getPoints();
	const auto& curves = enveFunc->getCurves();

	for (int i = 0; i < curves.size() && i < points.size() - 1; ++i) {
		// ����ָ�����ߣ���ʹcurveֵ��СҲ��ʾ���
		if (std::abs(curves[i].curve) > 0.001f) {
			EnvelopePoint midPoint = enveFunc->getCurveMidPoint(i);
			juce::Point<float> screenPoint = normalizedToScreen(midPoint.x, midPoint.y);

			// ��������ǿ�ȵ��������ɫ
			float curveStrength = std::abs(curves[i].curve);
			uint8_t alpha = (uint8_t)(128 + 127 * curveStrength);  // ��������ǿ�ȵ���͸����

			// �������ߵ��ھ��
			g.setColour(juce::Colour::fromRGBA(255, 255, 255, alpha));
			g.fillEllipse(screenPoint.x - CURVE_HANDLE_RADIUS, screenPoint.y - CURVE_HANDLE_RADIUS,
				CURVE_HANDLE_RADIUS * 2, CURVE_HANDLE_RADIUS * 2);

			g.setColour(juce::Colours::white);
			g.drawEllipse(screenPoint.x - CURVE_HANDLE_RADIUS, screenPoint.y - CURVE_HANDLE_RADIUS,
				CURVE_HANDLE_RADIUS * 2, CURVE_HANDLE_RADIUS * 2, 1.0f);
		}
		else {
			// Ϊֱ�߶���ʾһ�������ľ���������û�����
			EnvelopePoint midPoint = enveFunc->getCurveMidPoint(i);
			juce::Point<float> screenPoint = normalizedToScreen(midPoint.x, midPoint.y);

			g.setColour(juce::Colour::fromRGBA(255, 255, 255, 64));  // �ܵ��ľ��
			g.fillEllipse(screenPoint.x - CURVE_HANDLE_RADIUS * 0.7f, screenPoint.y - CURVE_HANDLE_RADIUS * 0.7f,
				CURVE_HANDLE_RADIUS * 1.4f, CURVE_HANDLE_RADIUS * 1.4f);
		}
	}
}