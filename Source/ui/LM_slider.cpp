#include "LM_slider.h"

void L_MODEL_STYLE::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
{
}

void L_MODEL_STYLE::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
	juce::Path arcPath1, arcPath2;
	float zoomVal = 0.55;//·Å´óÏµÊý
	arcPath1.addArc(x + width * (1.0 - zoomVal) / 2.0, y + height * (1.0 - zoomVal) / 2.0 - 4, width * zoomVal, height * zoomVal, M_PI / 4 - M_PI, M_PI / 4 - M_PI + sliderPosProportional * M_PI * 1.5, true);
	arcPath2.addArc(x + width * (1.0 - zoomVal) / 2.0, y + height * (1.0 - zoomVal) / 2.0 - 4, width * zoomVal, height * zoomVal, M_PI / 4 - M_PI + sliderPosProportional * M_PI * 1.5, -M_PI / 4 + M_PI, true);
	g.setColour(juce::Colour(0x66, 0x66, 0xCC));
	g.strokePath(arcPath1, juce::PathStrokeType(4.0));//»æÖÆ£¬ÕâÀï²ÅÊÇÉèÖÃÏß¿í
	g.setColour(juce::Colour(0x33, 0x33, 0x66));
	g.strokePath(arcPath2, juce::PathStrokeType(4.0));

	g.setColour(juce::Colour(0x22, 0xff, 0x22));//»­ÂÌÉ«µÄÏß
	float rotx = -sin(M_PI / 4 + sliderPosProportional * M_PI * 1.5), roty = cos(M_PI / 4 + sliderPosProportional * M_PI * 1.5);
	g.drawLine(x + width / 2 + rotx * 11 * zoomVal * 2.0,
		y + height / 2 + roty * 11 * zoomVal * 2.0 - 4,
		x + width / 2 + rotx * 17 * zoomVal * 2.0,
		y + height / 2 + roty * 17 * zoomVal * 2.0 - 4, 4.0);
}

void L_MODEL_STYLE::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	if (shouldDrawButtonAsHighlighted) g.setColour(juce::Colour(0x66, 0x66, 0xCC));
	else g.setColour(juce::Colour(0x33, 0x33, 0x66));

	juce::Rectangle<int> rect = button.getBounds();
	int x = rect.getX(), y = rect.getY(), w = rect.getWidth(), h = rect.getHeight();
	g.drawLine(x, y, x + w, y, 4);
	g.drawLine(x, y, x, y + h, 4);
	g.drawLine(x + w, y + h, x + w, y, 4);
	g.drawLine(x + w, y + h, x, y + h, 4);
}

void L_MODEL_STYLE::drawButtonText(juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
}

void L_MODEL_STYLE::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)

{
	juce::Rectangle<int> rect = button.getBounds();
	int x = rect.getX(), y = rect.getY(), w = rect.getWidth(), h = rect.getHeight();//»ñÈ¡°´Å¥³ß´ç

	if (shouldDrawButtonAsDown || button.getToggleState())//Èç¹û°´ÏÂ
	{
		g.setColour(juce::Colour(0x33, 0x33, 0x66));
		g.fillRect(x, y, w, h);//»­Ìî³ä¾ØÐÎ
	}
	else if (shouldDrawButtonAsHighlighted)//Èç¹ûÊó±ê¿¿½ü
	{
		g.setColour(juce::Colour(0x22, 0x22, 0x44));
		g.fillRect(x, y, w, h);//»­Ìî³ä¾ØÐÎ
	}//·ñÔò²»»­¾ØÐÎ

	g.setColour(juce::Colour(0x77, 0x77, 0xEE));
	g.drawLine(x, y, x + w, y, 4);//»­¿ò
	g.drawLine(x, y, x, y + h, 4);
	g.drawLine(x + w, y + h, x + w, y, 4);
	g.drawLine(x + w, y + h, x, y + h, 4);

	//»æÖÆÎÄ×Ö
	g.setColour(juce::Colour(0xff, 0xff, 0xff));
	juce::String name = button.getButtonText();
	g.setFont(juce::Font("FIXEDSYS", 15.0, 0));
	g.drawText(name, x, y, w, h, juce::Justification::centred);
}

void L_MODEL_STYLE::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonWidth, int buttonHeight, juce::ComboBox& box)
{
	int x = box.getX(), y = box.getY(), w = box.getWidth(), h = box.getHeight();
	if (isButtonDown)//Èç¹ûÊó±ê¿¿½ü
	{
		g.setColour(juce::Colour(0x22, 0x22, 0x44));
		g.fillRect(x + 2, y, w - 4, h);//»­Ìî³ä¾ØÐÎ
	}

	g.setColour(juce::Colour(0x77, 0x77, 0xEE));
	g.drawLine(x, y, x + w, y, 4);//»­¿ò
	g.drawLine(x, y, x, y + h, 4);
	g.drawLine(x + w, y + h, x + w, y, 4);
	g.drawLine(x + w, y + h, x, y + h, 4);

	/*
	g.setColour(juce::Colour(0x77, 0xff, 0x77));
	juce::String name = box.getText();
	g.setFont(juce::Font("FIXEDSYS", 15.0, 0));
	g.drawText(name, x + 8, y, w - 8, h, juce::Justification::centredLeft);
	*/
}

void L_MODEL_STYLE::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, const bool isSeparator, const bool isActive, const bool isHighlighted, const bool isTicked, const bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour)
{
	int x = area.getX(), y = area.getY(), w = area.getWidth(), h = area.getHeight();
	if (isTicked)//Èç¹û°´ÏÂ
	{
		g.setColour(juce::Colour(0x44, 0x44, 0x88));
		g.fillRect(x + 2, y, w - 4, h);//»­Ìî³ä¾ØÐÎ
	}
	else if (isHighlighted)//Èç¹ûÊó±ê¿¿½ü
	{
		g.setColour(juce::Colour(0x22, 0x22, 0x44));
		g.fillRect(x + 2, y, w - 4, h);//»­Ìî³ä¾ØÐÎ
	}

	g.setColour(juce::Colour(0x77, 0xff, 0x77));
	g.setFont(juce::Font("FIXEDSYS", 15.0, 0));
	g.drawText(text, x + 8, y, w - 8, h, juce::Justification::centredLeft);
}

void L_MODEL_STYLE::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
	g.fillAll(juce::Colour(0x11, 0x11, 0x22));//ºÚµÄ
	g.setColour(juce::Colour(0x44, 0x44, 0x88));
	g.drawLine(0, 0, width, 0, 4);
	g.drawLine(0, 0, 0, height, 4);
	g.drawLine(width, height, width, 0, 4);
	g.drawLine(width, height, 0, height, 4);
}

void Custom1_Slider::mouseDown(const juce::MouseEvent& event)
{
	if (event.mods.isRightButtonDown())
	{
		// Trigger right-click menu with event
		if (onRightClickRequested)
			onRightClickRequested(event);
		return;
	}

	lastMousePosition = event.source.getScreenPosition();
	setMouseCursor(juce::MouseCursor::NoCursor);
	juce::Slider::mouseDown(event);
}

void Custom1_Slider::mouseUp(const juce::MouseEvent& event)
{
	if (!event.mods.isRightButtonDown())
	{
		juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(lastMousePosition);
		setMouseCursor(juce::MouseCursor::NormalCursor);

		// Notify drag end
		if (onDragEnd)
			onDragEnd();
	}
	juce::Slider::mouseUp(event);
}

void Custom1_Slider::mouseDrag(const juce::MouseEvent& event)
{
	if (!event.mods.isRightButtonDown())
	{
		// Notify drag start on first drag
		static bool isDragging = false;
		if (!isDragging && onDragStart)
		{
			onDragStart();
			isDragging = true;
		}

		juce::Slider::mouseDrag(event);

		// Reset dragging flag when mouse is released
		if (event.mouseWasClicked())
			isDragging = false;
	}
}

LMKnob::LMKnob() : slider(), label(), valueLabel()
{
	// Setup slider
	slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	L_MODEL_STYLE_LOOKANDFEEL = std::make_unique<L_MODEL_STYLE>();
	slider.setLookAndFeel(L_MODEL_STYLE_LOOKANDFEEL.get());

	// Setup main label (knob name) - now clickable
	label.setJustificationType(juce::Justification::centredTop);
	label.setFont(juce::Font("fixedsys", 16.0, 0));
	label.setMinimumHorizontalScale(1.0);
	label.setColour(juce::Label::textColourId, juce::Colour(0x77, 0xff, 0x77));
	label.setFont(label.getFont().withStyle(juce::Font::bold));

	// Make main label clickable
	label.setInterceptsMouseClicks(true, false);
	label.onClick = [this]()
		{
			labelClicked();
		};

	// Setup value label (parameter value display) - initially hidden
	valueLabel.setJustificationType(juce::Justification::centred);
	valueLabel.setFont(juce::Font("fixedsys", 12.0, 0));
	valueLabel.setMinimumHorizontalScale(1.0);
	valueLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ffff));
	valueLabel.setFont(valueLabel.getFont().withStyle(juce::Font::bold));
	valueLabel.setVisible(false);  // Initially hidden

	// Setup slider callbacks
	slider.onValueChange = [this]
		{
			updateValueDisplay();
			this->repaint();
		};
	//todo::onLeftClick

	// Setup drag start callback to show value label
	slider.onDragStart = [this]()
		{
			showValueLabel();
		};

	// Setup drag end callback to start hide timer
	slider.onDragEnd = [this]()
		{
			hideValueLabel();
		};

	// Setup right-click menu for slider
	slider.onRightClickRequested = [this](const juce::MouseEvent& event)
		{
			auto compPos = event.position.toInt();
			showRightClickMenu(compPos + getScreenPosition().toInt() + juce::Point<int>{8, 32});

		};

	setPaintingIsUnclipped(true);
	setOpaque(false);
	addAndMakeVisible(slider);
	addAndMakeVisible(label);
	addAndMakeVisible(valueLabel);

	// Initialize value display
	updateValueDisplay();
}


LMKnob::~LMKnob()
{
	slider.setLookAndFeel(nullptr);
	ParamLinker = nullptr;
	valueInputWindow = nullptr;
}

void LMKnob::paint(juce::Graphics& g)
{
	// Your existing paint implementation
}

void LMKnob::ParamLink(juce::AudioProcessorValueTreeState& stateToUse, const juce::String& parameterID)
{
	ParamLinker = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(stateToUse, parameterID, slider);
	updateValueDisplay(); // Update display after linking
}

void LMKnob::setText(const juce::String& KnobText, const juce::String& unit)
{
	label.setText(KnobText, juce::dontSendNotification);
	text = KnobText;
	unitText = unit;
}

void LMKnob::resized()
{
	slider.setBounds(32 - 56 / 2, 32 - 56 / 2, 56, 56);
	label.setBounds(-32, 48 - 4, 64 + 64, 16);
	valueLabel.setBounds(-32, -4, 64 + 64, 16); // Position below the main label
	valueLabel.setVisible(false);  // Initially hidden
}

void LMKnob::setPos(int x, int y)
{
	setBounds(x - 32, y - 32, 64, 80); // Increase height to accommodate value label
}

void LMKnob::updateValueDisplay()
{
	double value = slider.getValue();
	juce::String valueText;

	// Format the value based on range (you can customize this)
	if (std::abs(value) >= 1000.0)
	{
		valueText = juce::String(value, 0); // No decimal places for large values
	}
	else if (std::abs(value) >= 10.0)
	{
		valueText = juce::String(value, 1); // 1 decimal place
	}
	else
	{
		valueText = juce::String(value, 2); // 2 decimal places
	}

	valueLabel.setText(valueText + unitText, juce::dontSendNotification);
}

void LMKnob::labelClicked()
{
	showValueInputDialog();
}

void LMKnob::showValueInputDialog()
{
	if (valueInputWindow != nullptr)
		return; // Dialog already open

	valueInputWindow = std::make_unique<juce::AlertWindow>("Enter Value",
		paramDescription,
		juce::AlertWindow::NoIcon);
	//description
	valueInputWindow->addTextBlock("Enter new value for " + text + ":");
	valueInputWindow->addTextEditor("value", juce::String(slider.getValue()), "Value:");
	valueInputWindow->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
	valueInputWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

	valueInputWindow->enterModalState(true,
		juce::ModalCallbackFunction::create([this](int result)
			{
				if (result == 1) // OK button pressed
				{
					juce::String inputText = valueInputWindow->getTextEditorContents("value");
					double newValue = inputText.getDoubleValue();

					// Clamp to slider range
					newValue = juce::jlimit(slider.getMinimum(), slider.getMaximum(), newValue);
					slider.setValue(newValue, juce::sendNotificationSync);
				}
				valueInputWindow = nullptr;
			}));
}
void LMKnob::showDescription()
{
	if (paramDescription.isEmpty())
		return; // No description to show
	juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::NoIcon, "Parameter Description:",
		paramDescription, "OK");
}

void LMKnob::showRightClickMenu(const juce::Point<int>& position)
{
	juce::PopupMenu menu;
	menu.addItem(1, "Type Value");
	menu.addItem(2, "Description");
	menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this)
		.withTargetScreenArea(juce::Rectangle<int>(position, position)),
		[this](int result)
		{
			if (result == 1) // "Type Value" selected
			{
				showValueInputDialog();
			}
			if (result == 2)
			{
				showDescription();
			}
		});
}
void LMKnob::showValueLabel()
{
	valueVisible = true;
	valueLabel.setVisible(true);
}


void LMKnob::hideValueLabel()
{
	valueVisible = false;
	valueLabel.setVisible(false);
}

LMButton::LMButton()
{
	// ÉèÖÃ°´Å¥ÊôÐÔ
	button.setButtonText(name);
	button.addListener(this); // ×¢²á¼àÌýÆ÷ÒÔ´¦Àí°´Å¥µã»÷

	L_MODEL_STYLE_LOOKANDFEEL = std::make_unique<L_MODEL_STYLE>();
	button.setLookAndFeel(L_MODEL_STYLE_LOOKANDFEEL.get());//Ó¦ÓÃl-model·ç¸ñ

	setPaintingIsUnclipped(true);//×é¼þÎÞ±ß½ç
	setOpaque(false);//×é¼þºÚÉ«²¿·ÖÍ¸Ã÷

	addAndMakeVisible(button);
}

LMButton::~LMButton()
{
	button.setLookAndFeel(nullptr);
}

void LMButton::setName(juce::String ButtonName)
{
	name = ButtonName;
	button.setButtonText(name);
}


void LMButton::resized()
{
	int w = getBounds().getWidth(), h = getBounds().getHeight();
	button.setBounds(0, 0, w, h);
}

/*
void LMButton::setPos(int x, int y)
{
	// Ìæ´úsetBounds
	setBounds(x - Width / 2, y - 12, Width, 24);
}

void LMButton::setButtonWidth(int ButtonWidth)
{
	Width = ButtonWidth;
	button.setBounds(0, 0, Width, 24);
	setBounds(getX(), getY(), Width, 24);
	repaint();
}
*/

void LMButton::buttonClicked(juce::Button* clicked)
{
	if (clicked == &button)
	{

	}
}

void LMButton::setClickedCallback(std::function<void()> cbFunc)
{
	button.onClick = cbFunc;
}

int LMButton::getButtonState()
{
	return button.getToggleState();
}

// LMCombox implementation

LMCombox::LMCombox()
{
	comboBox.addListener(this);
	L_MODEL_STYLE_LOOKANDFEEL = std::make_unique<L_MODEL_STYLE>();
	comboBox.setLookAndFeel(L_MODEL_STYLE_LOOKANDFEEL.get()); // Ó¦ÓÃl-model·ç¸ñ
	addAndMakeVisible(comboBox);
}

LMCombox::~LMCombox()
{
	comboBox.setLookAndFeel(nullptr);
}

void LMCombox::addItem(juce::String name, int ID)
{
	comboBox.addItem(name, ID);
}

void LMCombox::setSelectedID(int ID)
{
	comboBox.setSelectedId(ID);
}

void LMCombox::setPos(int x, int y)
{
	setBounds(x - Width / 2, y - 12, Width, 24);
}

void LMCombox::setComboxWidth(int ComboxWidth)
{
	Width = ComboxWidth;
	comboBox.setBounds(0, 0, Width, 24);
	setBounds(getX(), getY(), Width, 24);
}

void LMCombox::resized()
{
	// ÉèÖÃComboBoxµÄÎ»ÖÃºÍ´óÐ¡
	comboBox.setBounds(0, 0, Width, 24);
}

void LMCombox::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &comboBox)
	{
		// Handle combo box change
	}
}