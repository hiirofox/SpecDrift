#include "EnveUI.h"

void EnveUI::paint(juce::Graphics& g)
{
	if (!enveFunc) return;

	int w = getWidth();
	int h = getHeight();

	// 绘制包络曲线
	drawEnvelope(g);

	// 绘制曲线调节句柄
	drawCurveHandles(g);

	// 绘制节点
	drawPoints(g);

	// 绘制边框
	g.setColour(juce::Colour(0xff00ff00));  // 绿色边框
	g.drawRect(juce::Rectangle<int>(0, 0, w, h), 2.5f);
}

void EnveUI::mouseDown(const juce::MouseEvent& event)
{
	if (!enveFunc) return;

	juce::Point<float> mousePos = event.position;

	// 检查是否点击了节点
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

	// 检查是否点击了曲线句柄
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
		// 计算曲线调节 - 修复y1<y0时的方向问题
		const auto& points = enveFunc->getPoints();
		if (dragIndex < points.size() - 1) {
			juce::Point<float> p1 = normalizedToScreen(points[dragIndex].x, points[dragIndex].y);
			juce::Point<float> p2 = normalizedToScreen(points[dragIndex + 1].x, points[dragIndex + 1].y);

			// 计算鼠标相对于线段中点的垂直偏移
			juce::Point<float> lineVec = p2 - p1;
			float lineLength = lineVec.getDistanceFromOrigin();
			if (lineLength > 0) {
				// 获取线段中点
				juce::Point<float> midPoint = (p1 + p2) * 0.5f;

				// 计算垂直方向向量
				juce::Point<float> perpVec(-lineVec.y / lineLength, lineVec.x / lineLength);

				// 计算偏移量
				float offset = perpVec.getDotProduct(event.position - midPoint);

				// 根据y方向修正偏移方向
				// 当y1 < y0时（下降段），需要反转偏移方向以保持直观的交互
				float y0 = points[dragIndex].y;
				float y1 = points[dragIndex + 1].y;
				if (y1 < y0) {
					offset = -offset;  // 反转偏移方向
				}

				// 映射到合适的曲线范围，针对指数函数调整灵敏度
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

	// 在双击位置添加新节点
	juce::Point<float> normalizedPos = screenToNormalized(event.position);

	// 检查是否在有效范围内
	if (normalizedPos.x > 0.001f && normalizedPos.x < 0.999f) {
		enveFunc->addPoint(normalizedPos.x, normalizedPos.y);
		repaint();
	}
}

juce::Point<float> EnveUI::normalizedToScreen(float x, float y) const
{
	return juce::Point<float>(
		x * getWidth(),
		(1.0f - y) * getHeight()  // Y轴翻转
	);
}

juce::Point<float> EnveUI::screenToNormalized(const juce::Point<float>& screenPoint) const
{
	return juce::Point<float>(
		screenPoint.x / getWidth(),
		1.0f - (screenPoint.y / getHeight())  // Y轴翻转
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

		// 为所有线段提供曲线句柄，不论当前是否有曲线
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

	g.setColour(juce::Colour(0xff00ffff));  // 蓝色包络

	juce::Path envelopePath;

	// 绘制高精度包络曲线
	const int resolution = getWidth() * 2;  // 高分辨率采样
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

		// 绘制节点
		g.setColour(juce::Colour(0xff00ffff));  // 蓝色
		g.fillEllipse(screenPoint.x - POINT_RADIUS, screenPoint.y - POINT_RADIUS,
			POINT_RADIUS * 2, POINT_RADIUS * 2);

		// 绘制节点边框
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
		// 对于指数曲线，即使curve值很小也显示句柄
		if (std::abs(curves[i].curve) > 0.001f) {
			EnvelopePoint midPoint = enveFunc->getCurveMidPoint(i);
			juce::Point<float> screenPoint = normalizedToScreen(midPoint.x, midPoint.y);

			// 根据曲线强度调整句柄颜色
			float curveStrength = std::abs(curves[i].curve);
			uint8_t alpha = (uint8_t)(128 + 127 * curveStrength);  // 根据曲线强度调整透明度

			// 绘制曲线调节句柄
			g.setColour(juce::Colour::fromRGBA(255, 255, 255, alpha));
			g.fillEllipse(screenPoint.x - CURVE_HANDLE_RADIUS, screenPoint.y - CURVE_HANDLE_RADIUS,
				CURVE_HANDLE_RADIUS * 2, CURVE_HANDLE_RADIUS * 2);

			g.setColour(juce::Colours::white);
			g.drawEllipse(screenPoint.x - CURVE_HANDLE_RADIUS, screenPoint.y - CURVE_HANDLE_RADIUS,
				CURVE_HANDLE_RADIUS * 2, CURVE_HANDLE_RADIUS * 2, 1.0f);
		}
		else {
			// 为直线段显示一个淡化的句柄，方便用户发现
			EnvelopePoint midPoint = enveFunc->getCurveMidPoint(i);
			juce::Point<float> screenPoint = normalizedToScreen(midPoint.x, midPoint.y);

			g.setColour(juce::Colour::fromRGBA(255, 255, 255, 64));  // 很淡的句柄
			g.fillEllipse(screenPoint.x - CURVE_HANDLE_RADIUS * 0.7f, screenPoint.y - CURVE_HANDLE_RADIUS * 0.7f,
				CURVE_HANDLE_RADIUS * 1.4f, CURVE_HANDLE_RADIUS * 1.4f);
		}
	}
}