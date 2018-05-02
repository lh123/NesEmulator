#include "ui/UIComponent.h"

UIComponent::UIComponent() : mShow(false) {}

UIComponent::~UIComponent() {}

bool UIComponent::isShow() const { return mShow; }

void UIComponent::show() { mShow = true; }

void UIComponent::close() { mShow = false; }
