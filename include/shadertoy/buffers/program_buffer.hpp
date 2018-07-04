#ifndef _SHADERTOY_BUFFERS_PROGRAM_BUFFER_HPP_
#define _SHADERTOY_BUFFERS_PROGRAM_BUFFER_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/buffers/gl_buffer.hpp"

#include "shadertoy/program_input.hpp"

#include <deque>

namespace shadertoy
{
namespace buffers
{

/**
 * @brief      Represents a buffer that processes its inputs with a shader
 */
class shadertoy_EXPORT program_buffer : public gl_buffer
{
private:
	/// Buffer program
	gl::program program_;

	/// Bound uniform state
	std::vector<std::unique_ptr<shadertoy::bound_inputs_base>> bound_inputs_;

	/// Inputs for this shader
	std::deque<program_input> inputs_;

	/// Alternative template to use when generating the program for this buffer
	std::shared_ptr<compiler::program_template> override_program_;

	/// Template part that this buffer should provide to the shader template
	std::unique_ptr<compiler::basic_part> source_;

protected:
	/**
	 * @brief      Initialize the geometry to use for this buffer
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	virtual void init_geometry(const render_context &context, const io_resource &io) = 0;

	/**
	 * @brief      Render the geometry for this buffer. The inputs, program and framebuffer
	 *             are already bound by the caller.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 */
	virtual void render_geometry(const render_context &context, const io_resource &io) = 0;

	/**
	 * @brief      Initialize the contents of this buffer
	 *
	 * @param[in]  context Rendering context to use for shared objects
	 * @param[in]  io      IO resource object
	 */
	void init_contents(const render_context &context, const io_resource &io) override;

	/**
	 * @brief      Render the contents of this buffer.
	 *
	 * @param[in]  context Rendering context to use for rendering this buffer
	 * @param[in]  io      IO resource object
	 */
	void render_gl_contents(const render_context &context, const io_resource &io) override;

public:
	/**
	 * @brief      Initialize a new ShaderProgram buffer
	 *
	 * @param[in]  id       Identifier for this buffer
	 */
	program_buffer(const std::string &id);

	/**
	 * @brief      Get a reference to the program represented by this buffer
	 *
	 * @return     OpenGL program for this buffer.
	 */
	inline const gl::program &program() const
	{ return program_; }

	/**
	 * @brief      Get a reference to the input array for this buffer
	 *
	 * @return     Reference to the input array for this buffer
	 */
	inline const std::deque<program_input> &inputs() const
	{ return inputs_; }

	/**
	 * @brief      Get a reference to the input array for this buffer
	 *
	 * @return     Reference to the input array for this buffer
	 */
	inline std::deque<program_input> &inputs()
	{ return inputs_; }

	/**
	 * @brief      Get the current override program template for this buffer
	 *
	 * @return     Pointer to the override program, or null if this buffer is using the default program template
	 */
	inline const std::shared_ptr<compiler::program_template> &override_program() const
	{ return override_program_; }

	/**
	 * @brief      Set the current override program template for this buffer
	 *
	 * @param new_program Pointer to the override program template, or null if
	 *                    this buffer should use the default program template
	 */
	inline void override_program(std::shared_ptr<compiler::program_template> new_program)
	{ override_program_ = new_program; }

	/**
	 * @brief       Get a reference to the current source part for this buffer
	 *
	 * @return      Pointer to the source part
	 */
	inline const std::unique_ptr<compiler::basic_part> &source() const
	{ return source_; }

	/**
	 * @brief       Set the sources for this buffer to the given part
	 *
	 * @param new_part New part to use as sources for this buffer
	 */
	inline void source(std::unique_ptr<compiler::basic_part> new_part)
	{ source_ = std::move(new_part); }

	/**
	 * @brief       Set the sources for this buffer from the given part
	 *
	 * @param new_source New source string to use for compiling this buffer
	 */
	void source(const std::string &new_source);

	/**
	 * @brief       Set the sources for this buffer from the given part from a file
	 *
	 * The target file will be read everytime this buffer is being compiled. Use
	 * program_buffer#source if you want to cache the file's contents.
	 *
	 * @param new_file New file to load the sources from
	 */
	void source_file(const std::string &new_file);
};
}
}

#endif /* _SHADERTOY_BUFFERS_PROGRAM_BUFFER_HPP_ */
