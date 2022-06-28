#include "image.hpp"
#include <iostream>

namespace Application::Helper {
	SDL_Surface *loadFile(std::string_view filePath) {
		if (filePath.data() == nullptr) {
			std::cout << "Failed to load file " << SDL_GetError() << '\n';
			return nullptr;
		}
		return IMG_Load(filePath.data());
	}

	// load textures
	IMD Image::create(std::string_view filePath, SDL_Renderer *ren, SDL_Color *key) {
		IMD newImage = std::make_shared<ImageData>();
		newImage->path = filePath;

		auto iter = images.find(filePath);
		if (iter != images.end())
			return iter->second; // we found the filePath

		// start texture creation process
		SDL_Surface *surf = loadFile(filePath);

		if (key != nullptr)
			SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, key->r, key->g, key->b));

		newImage->ptr = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(ren, surf), SDL_DestroyTexture);
		if (newImage->ptr == nullptr) {
			std::cout << "Text texture failed to be created\n";
			return nullptr;
		}
		images.insert({filePath, newImage});

		SDL_FreeSurface(surf);

		return newImage;
	}

	IMD Image::create(uint32_t width, uint32_t height, SDL_Renderer *ren) {
		IMD newImage = std::make_shared<ImageData>();
		newImage->ptr = std::shared_ptr<SDL_Texture>(SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height), SDL_DestroyTexture);
		if (newImage->ptr == nullptr) {
			std::cout << "Texture failed to be created\n";
			return nullptr;
		}
		images.insert({nullptr, newImage});

		return newImage;
	}

	// load text
	IMD Image::createA(const MessageData &msg, SDL_Renderer *ren) {
		IMD newImage = std::make_shared<ImageData>();
		newImage->path = msg.fontFile;

		TTF_Font *font = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		SDL_Surface *surf = TTF_RenderText_Blended(font, msg.msg.data(), msg.col);
		if (surf == nullptr) {
			TTF_CloseFont(font);
			std::cout << "TTF_RenderText error " << TTF_GetError() << "\n";
			return nullptr;
		}

		newImage->ptr = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(ren, surf), SDL_DestroyTexture);
		if (newImage->ptr == nullptr) {
			std::cout << "Text texture failed to be created\n";
			return nullptr;
		}
		images.insert({msg.fontFile, newImage});

		SDL_FreeSurface(surf);
		TTF_CloseFont(font);

		return newImage;
	}

	// load text with outline
	IMD Image::create(const MessageData &msg, SDL_Renderer *ren) {
		IMD newImage = std::make_shared<ImageData>();

		TTF_Font *font = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		TTF_Font *ofont = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		TTF_SetFontOutline(ofont, 1);

		SDL_Surface *bg_surf = TTF_RenderText_Blended(font, msg.msg.data(), msg.col);
		SDL_Surface *fg_surf = TTF_RenderText_Blended(ofont, msg.msg.data(), {0x00, 0x00, 0x00});

		// destination rect that gets the size of the surface (explicit x/y for those that want to understand without digging)
		SDL_Rect position = {position.x = 1, position.y = 1, fg_surf->w, fg_surf->h};
		SDL_BlitSurface(bg_surf, nullptr, fg_surf, &position);

		newImage->ptr = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(ren, fg_surf), SDL_DestroyTexture);
		if (newImage->ptr == nullptr) {
			std::cout << "Text texture failed to be created\n";
			return nullptr;
		}
		images.insert({msg.fontFile, newImage});

		SDL_FreeSurface(bg_surf);
		SDL_FreeSurface(fg_surf);
		TTF_CloseFont(ofont);
		TTF_CloseFont(font);

		return newImage;
	}

	void Image::draw(IMD &img, SDL_Renderer *ren, int x, int y, double sx, double sy, SDL_Rect *clip) noexcept {
		SDL_Rect dst {};
		dst.x = x;
		dst.y = y;
		if (clip != nullptr) {
			dst.w = clip->w;
			dst.h = clip->h;
		} else {
			SDL_QueryTexture(img->ptr.get(), nullptr, nullptr, &dst.w, &dst.h);
		}

		if ((sx && sy) != 0) {
			dst.w *= static_cast<int>(sx);
			dst.h *= static_cast<int>(sy);
		}

		SDL_RenderCopy(ren, img->ptr.get(), clip, &dst);
	}

	void Image::add(std::string_view str, IMD &img) {
		auto iter = images.find(str);
		if (iter != images.end()) {
			std::cout << "image already exists\n";
			return; // canvas exists
		} else {
			images.insert({str, img});
		}
	}

	void Image::remove(IMD &img) {
		img.~shared_ptr();
	}

	void Image::printImageCount() {
		for (const auto &i : images) {
			std::cout << "Image Size: " << images.size() << '\n';
		}
	}

	/*
	TTF_Font *font = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
			if (font == nullptr) {
				std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
				return nullptr;
			}

			TTF_Font *ofont = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
			if (font == nullptr) {
				std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
				return nullptr;
			}

			TTF_SetFontOutline(ofont, 1);

			SDL_Surface *bg_surf = TTF_RenderText_Blended(font, msg.msg.data(), msg.col);
			SDL_Surface *fg_surf = TTF_RenderText_Blended(ofont, msg.msg.data(), {0x00, 0x00, 0x00});

			// destination rect that gets the size of the surface (explicit x/y for those that want to understand without digging)
			SDL_Rect position = {position.x = 1, position.y = 1, fg_surf->w, fg_surf->h};
			SDL_BlitSurface(bg_surf, nullptr, fg_surf, &position);

			newImage->ptr = SDL_CreateTextureFromSurface(ren, fg_surf);
			newImage->bind = newImage->ptr;
			if (newImage->ptr == nullptr) {
				std::cout << "Text texture failed to be created \n";
				return nullptr;
			}
			images.insert({msg.fontFile, newImage});

			SDL_FreeSurface(bg_surf);
			SDL_FreeSurface(fg_surf);
			TTF_CloseFont(ofont);
			TTF_CloseFont(font);

			return newImage;
	*/

	/*
	SDL_Surface *Image::loadFile(std::string_view filePath) {
		if (filePath.data() == nullptr) {
			std::cout << "Failed to load file " << SDL_GetError() << '\n';
			return nullptr;
		}
		return IMG_Load(filePath.data());
	}

	SDL_Texture *Image::loadTexture(std::string_view filePath, SDL_Renderer *ren, SDL_Color *key) {
		SDL_Surface *surf = loadFile(filePath);

		if (key != nullptr)
			SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, key->r, key->g, key->b));

		SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);

		return tex;
	}

	void Image::drawTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, double sx, double sy, SDL_Rect *clip) noexcept {
		SDL_Rect dst {};
		dst.x = x;
		dst.y = y;
		if (clip != nullptr) {
			dst.w = clip->w;
			dst.h = clip->h;
		} else {
			SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
		}

		if ((sx && sy) != 0) {
			dst.w *= sx;
			dst.h *= sy;
		}

		SDL_RenderCopy(ren, tex, clip, &dst);
	}

	SDL_Texture *Image::loadText(std::string_view msg, SDL_Renderer *ren, std::string_view fontFile, const SDL_Color &col, int fontSize) {
		TTF_Font *font = TTF_OpenFont(fontFile.data(), fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		SDL_Surface *surf = TTF_RenderText_Blended(font, msg.data(), col);
		if (surf == nullptr) {
			TTF_CloseFont(font);
			std::cout << "TTF_RenderText error " << TTF_GetError() << "\n";
			return nullptr;
		}

		SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
		if (tex == nullptr) {
			std::cout << "Text texture failed to be created \n";
			return nullptr;
		}

		SDL_FreeSurface(surf);
		TTF_CloseFont(font);

		return tex;
	}

	SDL_Texture *Image::loadTextOutline(std::string_view msg, SDL_Renderer *ren, std::string_view fontFile, const SDL_Color &col, int fontSize) {
		TTF_Font *font = TTF_OpenFont(fontFile.data(), fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		TTF_Font *ofont = TTF_OpenFont(fontFile.data(), fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		TTF_SetFontOutline(ofont, 1);

		SDL_Surface *bg_surf = TTF_RenderText_Blended(font, msg.data(), col);
		SDL_Surface *fg_surf = TTF_RenderText_Blended(ofont, msg.data(), {0x00, 0x00, 0x00});

		// destination rect that gets the size of the surface (explicit x/y for those that want to understand without digging)
		SDL_Rect position = {position.x = 1, position.y = 1, fg_surf->w, fg_surf->h};
		SDL_BlitSurface(bg_surf, nullptr, fg_surf, &position);

		SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, fg_surf);

		SDL_FreeSurface(bg_surf);
		SDL_FreeSurface(fg_surf);
		TTF_CloseFont(ofont);
		TTF_CloseFont(font);

		return tex;
	}
	*/
} // namespace Application::Helper