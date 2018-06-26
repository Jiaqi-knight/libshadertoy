#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/io_resource.hpp"

#include "shadertoy/utils/log.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;

void io_resource::init_render_texture(rsize size, std::shared_ptr<gl::texture> &texptr)
{
	// Only create a texture object if it is necessary
	if (!texptr)
		texptr = std::make_shared<gl::texture>(GL_TEXTURE_2D);

	// Allocate texture storage according to width/height
	texptr->image_2d(GL_TEXTURE_2D, 0, internal_format_, size.width, size.height, 0, GL_BGRA,
					 GL_UNSIGNED_BYTE, nullptr);

	// Clear the frame accumulator so it doesn't contain garbage
	unsigned char black[4] = {0};
	texptr->clear_tex_image(0, GL_BGRA, GL_UNSIGNED_BYTE, black);
}

io_resource::io_resource(rsize_ref &&render_size, GLint internal_format, member_swap_policy swap_policy)
	: render_size_(std::move(render_size)),
	internal_format_(internal_format),
	swap_policy_(swap_policy),
	source_tex_(),
	target_tex_()
{}

void io_resource::allocate()
{
	rsize size(render_size_->resolve());
	assert(size.width > 0);
	assert(size.height > 0);

	// Current texture settings
	rsize current_size;
	GLint current_format(0);
	member_swap_policy current_policy(member_swap_policy::double_buffer);

	// If the textures exist, read their parameters
	if (source_tex_)
	{
		GLint width, height;
		source_tex_->get_parameter(0, GL_TEXTURE_WIDTH, &width);
		source_tex_->get_parameter(0, GL_TEXTURE_HEIGHT, &height);
		source_tex_->get_parameter(0, GL_TEXTURE_INTERNAL_FORMAT, &current_format);
		current_size = rsize(width, height);
	}	

	if (current_size != size || current_format != internal_format_ || current_policy != swap_policy_)
	{
		init_render_texture(size, source_tex_);

		if (swap_policy_ == member_swap_policy::double_buffer)
			init_render_texture(size, target_tex_);
		else if (swap_policy_ == member_swap_policy::single_buffer)
			target_tex_.reset();
	}
}

void io_resource::swap()
{
	if (!source_tex_)
	{
		log::shadertoy()->warn("Swapping unallocated IO resource object {}", (void*)this);
	}
	else
	{
		GLint width, height, current_format;
		source_tex_->get_parameter(0, GL_TEXTURE_WIDTH, &width);
		source_tex_->get_parameter(0, GL_TEXTURE_HEIGHT, &height);
		source_tex_->get_parameter(0, GL_TEXTURE_INTERNAL_FORMAT, &current_format);

		rsize current_size(width, height);
		if (current_size != render_size_->resolve() || current_format != internal_format_)
			log::shadertoy()->warn("IO resource object {} render size and allocated sizes and/or formats mismatch", (void*)this);
	}

	if (target_tex_)
	{
		std::swap(source_tex_, target_tex_);
	}
}
