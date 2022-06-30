#pragma once

#include <SDL.h>
#include "data.hpp"
#include <map>
#include <string>
#include <vector>

// add a sprite sheet and iterate over it (assuming the spritesheet is exactly the same width and height)

namespace Application::Helper {
	class Animation {
	public:
		//constexpr uint32_t getCurrentFrame();
		/** Quick animation creation
		 *
		 * \param frames -> number of frames in the gif extraction
		 * \param x -> x position of the button
		 * \param y -> y position of the button
		 * \param w -> width of the button
		 * \param h -> height of the button
		 * \return Button object filled with all of the essential details for a customized button
		 */
		void addAnimation(int frames, int x, int y, int w, int h);
		//void setCurrentFrame(std::string_view frameName);
		//void stopAnimation();
		
		// speed -> how fast the animation should play
		void update(float speed, double dt);
		void draw(IMD &img, SDL_Renderer *ren, int x, int y, double scale = 0.0);

	private:
		float frameTime {0.0f};
		int currentFrame {0};
		std::basic_string<char> animStr {};
		// make this an unordered_map?
		std::map<unsigned int, SDL_Rect> frames {};
	};
} // namespace Application::Helper