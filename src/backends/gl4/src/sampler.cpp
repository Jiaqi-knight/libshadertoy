#include <epoxy/gl.h>

#include "shadertoy/backends/gl4/sampler.hpp"
#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/backends/gl4/backend.hpp"

using namespace shadertoy::backends::gl4;

null_sampler_error::null_sampler_error()
: shadertoy_error("An attempt was made to dereference a null sampler")
{
}

void sampler::bind(GLuint unit) const
{
	backend::current().bind_sampler_unit(unit, *this);
}

void sampler::unbind(GLuint unit) const
{
	backend::current().bind_sampler_unit(unit, std::nullopt);
}

void sampler::parameter(GLenum pname, GLint param) const
{
	gl_call(glSamplerParameteri, GLuint(*this), pname, param);
}

void sampler::parameter(GLenum pname, GLfloat param) const
{
	gl_call(glSamplerParameterf, GLuint(*this), pname, param);
}

void sampler::get_parameter(GLenum pname, GLint *param) const
{
	gl_call(glGetSamplerParameteriv, GLuint(*this), pname, param);
}

void sampler::get_parameter(GLenum pname, GLfloat *param) const
{
	gl_call(glGetSamplerParameterfv, GLuint(*this), pname, param);
}