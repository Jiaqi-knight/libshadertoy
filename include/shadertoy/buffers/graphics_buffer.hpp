#ifndef _SHADERTOY_BUFFERS_GRAPHICS_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_GRAPHICS_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"

#include <optional>
#include <string>

namespace shadertoy
{
namespace buffers
{

/**
 * @brief Represents a program output location to be used in a buffer
 */
struct buffer_output
{
	/// Name of the buffer output
	std::string name;

	/// Interface location of the output
	GLint location;

	/// Type of the output
	GLenum type;

	/**
	 * @brief Construct a new instance of the buffer_output class
	 *
	 * @param name     Name of the buffer output
	 * @param location Interface location of the output
	 * @param type     Type of the output
	 */
	buffer_output(std::string name, GLint location, GLenum type);
};


/**
 * @brief Represents a buffer in the graphics pipeline
 *
 * This is the base class for buffers that have graphics-like outputs,
 * usually rendering to textures at a given resolution.
 */
class shadertoy_EXPORT graphics_buffer : public basic_buffer
{
	/// Size of this buffer's output
	rsize_ref render_size_;

protected:
	/**
	 * @brief     Initialize a new graphics buffer
	 *
	 * @param[in] id        Identifier for this buffer
	 */
	graphics_buffer(std::string id);

public:
	/**
	 * @brief Obtains the list of outputs for this buffer.
	 *
	 * This method is to be implemented by derived classes which are able to
	 * describe their outputs from their internal representations (i.e.
	 * programs using ARB_program_interface_query).
	 *
	 * @return std::nullopt if the query is not supported, otherwise list of
	 * requested outputs
	 */
	virtual std::optional<std::vector<buffer_output>> get_buffer_outputs() const;
};

}
}

#endif /* _SHADERTOY_BUFFERS_GRAPHICS_BUFFER_HPP_ */