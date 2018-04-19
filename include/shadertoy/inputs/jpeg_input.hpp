#ifndef _SHADERTOY_INPUTS_JPEG_INPUT_HPP_
#define _SHADERTOY_INPUTS_JPEG_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/file_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input that is loaded from a file
 *        using libjpeg.
 */
class shadertoy_EXPORT jpeg_input : public file_input
{
protected:
	/**
	 * @brief Loads the image from filename
	 *
	 * @param filename Filename to load the image from
	 * @param vflip    true if the image should be flipped vertically while loading
	 *
	 * @return OpenGL texture representing the image
	 */
	std::shared_ptr<gl::texture> load_file(const std::string &filename, bool vflip) override;

public:
	/**
	 * @brief Initializes a new instance of the jpeg_input class
	 *
	 * This instance will have no filename setup, therefore it will not load
	 * any texture.
	 */
	jpeg_input();

	/**
	 * @brief Initializes a new instance of the jpeg_input class
	 * with a default filename
	 *
	 * @param filename Filename to load the image from
	 */
	explicit jpeg_input(const std::string &filename);
};
}
}

#endif /* _SHADERTOY_INPUTS_JPEG_INPUT_HPP_ */
