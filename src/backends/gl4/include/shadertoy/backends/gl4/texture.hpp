#ifndef _SHADERTOY_BACKENDS_GL4_TEXTURE_HPP_
#define _SHADERTOY_BACKENDS_GL4_TEXTURE_HPP_

#include "shadertoy/backends/gl4/pre.hpp"
#include "shadertoy/backends/gl4/resource.hpp"
#include "shadertoy/backends/gx/texture.hpp"

#include "shadertoy/shadertoy_error.hpp"

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
/**
 * @brief Error thrown when an attempt is made to dereference a null texture.
 */
class shadertoy_EXPORT null_texture_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the null_texture_error class.
	 */
	explicit null_texture_error();
};

/**
 * @brief Implementation of the allocator pattern for textures.
 * Takes a target as a supplementary parameter compared to the standard
 * resource allocators. Also links statically to glCreateTexture and
 * glDeleteTexture according to the OpenGL spec.
 */
class shadertoy_EXPORT texture_allocator
{
	public:
	/**
	 * @brief Create a new texture of the given type.
	 *
	 * @param  target Target of the texture to create
	 * @return        Id of the created texture
	 *
	 * @throws opengl_error
	 */
	GLuint create(GLenum target);

	/**
	 * @brief Delete the given texture
	 *
	 * @param resource Id of the texture to delete
	 *
	 * @throws opengl_error
	 */
	void destroy(GLuint resource);
};

/**
 * @brief Represents an OpenGL texture.
 */
class shadertoy_EXPORT texture : public resource<texture, texture_allocator, null_texture_error, gx::texture>
{
	public:
	texture(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	/**
	 * @brief Create a new texture for the given target.
	 *
	 * @param target Target of the new texture.
	 *
	 * @throws opengl_error
	 */
	texture(GLenum target);

	/**
	 * @brief glBindTexture
	 *
	 * @param target Target to bind the texture to
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void bind(GLenum target) const override;

	/**
	 * @brief glBindTexture
	 *
	 * Unbinds the current texture for the given \p target.
	 *
	 * @param target Target to unbind from
	 *
	 * @throws opengl_error
	 */
	void unbind(GLenum target) const override;

	/**
	 * @brief glBindTextureUnit
	 *
	 * @param unit Unit to bind the texture to
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void bind_unit(GLuint unit) const override;

	/**
	 * @brief glTextureParameteri
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void parameter(GLenum pname, GLint param) const override;

	/**
	 * @brief glTextureParameterf
	 *
	 * @param pname Parameter name to set
	 * @param param Value of the parameter
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void parameter(GLenum pname, GLfloat param) const override;

	/**
	 * @brief glGetTextureLevelParameteriv
	 *
	 * @param level  Level
	 * @param pname  Parameter name
	 * @param params Parameters
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void get_parameter(GLint level, GLenum pname, GLint *params) const override;

	/**
	 * @brief glGetTextureLevelParameterfv
	 *
	 * @param level  Level
	 * @param pname  Parameter name
	 * @param params Parameters
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void get_parameter(GLint level, GLenum pname, GLfloat *params) const override;

	/**
	 * @brief glTextureImage1DEXT
	 *
	 * @param target         Target
	 * @param level          Level
	 * @param internalFormat Internal format
	 * @param width          Width
	 * @param border         Border (must be 0)
	 * @param format         Format
	 * @param type           Type
	 * @param data           Data
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void image_1d(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border,
				  GLenum format, GLenum type, const GLvoid *data) const override;

	/**
	 * @brief glTextureImage2DEXT
	 *
	 * @param target         Target
	 * @param level          Level
	 * @param internalFormat Internal format
	 * @param width          Width
	 * @param height         Height
	 * @param border         Border (must be 0)
	 * @param format         Format
	 * @param type           Type
	 * @param data           Data
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void image_2d(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
				  GLint border, GLenum format, GLenum type, const GLvoid *data) const override;

	/**
	 * @brief glGenerateTextureMipmap
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void generate_mipmap() const override;

	/**
	 * @brief glClearTexImage
	 *
	 * @param level  Level
	 * @param format Format
	 * @param type   Type
	 * @param data   Data
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void clear_tex_image(GLint level, GLenum format, GLenum type, const void *data) const override;

	/**
	 * @brief glGetTextureImage
	 *
	 * @param level   Level
	 * @param format  Format
	 * @param type    Type
	 * @param bufSize Buffer size
	 * @param pixels  Buffer
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void get_image(GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) const override;

	/**
	 * @brief glTextureBuffer
	 *
	 * @param internalformat Internal format
	 * @param buffer         Buffer
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 * @throws null_buffer_error
	 */
	void buffer(GLenum internalformat, const gx::buffer &buffer) const override;

	/**
	 * @brief glTextureBufferRange
	 *
	 * @param internalformat Internal format
	 * @param buffer         Buffer
	 * @param offset         Offset
	 * @param size           Size
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 * @throws null_buffer_error
	 */
	void buffer_range(GLenum internalformat, const gx::buffer &buffer, GLintptr offset, GLsizeiptr size) const override;

	/**
	 * @brief glBindImageTexture
	 *
	 * @param unit    Unit
	 * @param level   Level
	 * @param layered Layered
	 * @param layer   Layer
	 * @param access  Access
	 * @param format  Format
	 *
	 * @throws opengl_error
	 * @throws null_texture_error
	 */
	void bind_image(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) const override;

	/**
	 * @brief glBindImageTexture without a texture to clear bindings
	 *
	 * @param unit    Unit
	 *
	 * @throws opengl_error
	 */
	void unbind_image(GLuint unit) const override;
};
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_TEXTURE_HPP_ */
