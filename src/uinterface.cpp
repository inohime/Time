#include "uinterface.hpp"

namespace Application::Helper {
	/*
	BUTTONPTR UInterface::createButton(MessageData &msg, IMD texture, int x, int y, unsigned int w, unsigned int h) {
		BUTTONPTR newButton = std::make_shared<Button>();

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};
		if (texture != nullptr)
			newButton->texture = *texture;

		newButton->md = msg;
		btnList.emplace_back(newButton);

		return newButton;
	}
	*/

	BUTTONPTR UInterface::createButton(std::string_view text, IMD texture, SDL_Color col, int x, int y, unsigned int w, unsigned int h) {
		BUTTONPTR newButton = std::make_shared<Button>();

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};

		newButton->text = text;
		if (texture != nullptr)
			newButton->texture = *texture;

		newButton->color = col;
		newButton->initialColor = col;
		newButton->canMinimize = false;
		newButton->canQuit = false;

		btnList.emplace_back(newButton);

		return newButton;
	}

	std::vector<BUTTONPTR> &UInterface::getButtonList() {
		return btnList;
	}

	SDL_Point &UInterface::getMousePos() {
		return mousePos;
	}

	void UInterface::setButtonTexture(BUTTONPTR &button, IMD &texture) {
		button->texture = *texture;
	}

	void UInterface::showOutline(BUTTONPTR &button, bool show) {
		button->showOutline = show;
	}

	bool UInterface::cursorInBounds(BUTTONPTR &button, SDL_Point &mousePos) {
		if (mousePos.x >= button->box.x && mousePos.x <= (button->box.x + button->box.w) &&
			mousePos.y >= button->box.y && mousePos.y <= (button->box.y + button->box.h)) {
			return true;
		}

		return false;
	}

	void UInterface::update(SDL_Event *ev) {
		//SDL_Point mousePos = {};
		for (auto &button : btnList) {
			switch (ev->type) {
				case SDL_MOUSEBUTTONDOWN: {
					if (button->isClickable)
						button->color = {0, 0, 255, 255}; // 
				} break;

				case SDL_MOUSEBUTTONUP: {
					if (!button->isClickable) {
						button->color = button->initialColor;
					} else {
						button->color = {0, 255, 0, 255}; // test
					}
				} break;

				case SDL_MOUSEMOTION: {
					mousePos.x = ev->motion.x;
					mousePos.y = ev->motion.y;
					// check if the cursor is hovering over the button
					if (cursorInBounds(button, mousePos)) {
						button->color = {0, 255, 0, 255};
						button->isClickable = true;
					} else {
						button->color = button->initialColor;
						button->isClickable = false;
					}
				} break;
			}
		}
	}

	void UInterface::draw(BUTTONPTR &button, IMD buttonText, SDL_Renderer *ren, double scaleX, double scaleY, SDL_Rect *clip) {
		SDL_Rect dst = {button->box.x, button->box.y, button->box.w, button->box.h};
		SDL_Rect textDst = {button->box.x, button->box.y, button->box.w, button->box.h}; // test, fix later

		if ((scaleX && scaleY) != 0) {
			dst.w *= static_cast<int>(scaleX);
			dst.h *= static_cast<int>(scaleY);
		}

		if (button->showOutline) {
			SDL_RenderCopy(ren, button->texture.texture.get(), nullptr, &dst);
			SDL_RenderCopy(ren, buttonText->texture.get(), nullptr, &textDst);
			SDL_SetRenderDrawColor(ren, button->color.r, button->color.g, button->color.b, button->color.a);
			SDL_RenderDrawRect(ren, &button->box);
		} else {
			SDL_RenderCopy(ren, button->texture.texture.get(), nullptr, &dst);
			SDL_RenderCopy(ren, buttonText->texture.get(), nullptr, &textDst);
		}
	}
} // namespace Application::Helper