#ifndef _SHADERTOY_INPUTS_IMAGE_INPUT_HPP_
#define _SHADERTOY_INPUTS_IMAGE_INPUT_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/inputs/basic_input.hpp"

namespace shadertoy
{
namespace inputs
{

/**
 * @brief Represents an input based on an image, potentially loaded
 * from the disk or another source.
 */
class shadertoy_EXPORT image_input : public basic_input
{
	/// Texture object to hold the image data
	std::shared_ptr<gl::texture> image_texture_;

protected:
	/**
	 * @brief Implemented by derived classes to provide the image decoding logic
	 *
	 * @return OpenGL texture representing the image
	 */
	virtual std::shared_ptr<gl::texture> load_image() = 0;

	/**
	 * @brief Loads the input's contents.
	 *
	 * Classes implementing image_input should implement load_image to
	 * support this behavior.
	 */
	void load_input() final;

	/**
	 * @brief Resets the input's contents.
	 */
	void reset_input() final;

	/**
	 * @brief Uses this input in a rendering pass.
	 */
	std::shared_ptr<gl::texture> use_input() final;

	/**
	 * @brief Initializes a new instance of the image_input class.
	 */
	image_input();
};
}
}

#endif /* _SHADERTOY_INPUTS_IMAGE_INPUT_HPP_ */
