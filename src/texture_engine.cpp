#include <map>
#include <sstream>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>

#include <SOIL/SOIL.h>
#include <jpeglib.h>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl.hpp"

#include "shadertoy/buffer_config.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/texture_engine.hpp"

using namespace std;
using namespace std::placeholders;
using namespace shadertoy;

namespace fs = boost::filesystem;

// Error message from SOIL
extern "C" {
extern char *result_string_pointer;
}

shared_ptr<gl::texture> texture_engine::soil_texture_handler(const input_config &inputConfig,
                                                             bool &skipTextureOptions,
                                                             bool &skipCache,
                                                             bool &framebufferSized)
{
	if (inputConfig.source.empty())
	{
		BOOST_LOG_TRIVIAL(error) << "Missing source path for input "
								 << inputConfig.id;
		return shared_ptr<gl::texture>();
	}

	fs::path texPath(inputConfig.source);

	if (!fs::exists(texPath))
	{
		BOOST_LOG_TRIVIAL(error) << texPath << " not found for input "
								 << inputConfig.id;
		return shared_ptr<gl::texture>();
	}

	shared_ptr<gl::texture> texture;
	string ext(texPath.extension().string());
	boost::algorithm::to_lower(ext);
	if (ext.compare(".jpg") == 0 || ext.compare(".jpeg") == 0)
	{
		// use libjpeg
		FILE *infile;
		string sp(texPath.string());
		if ((infile = fopen(sp.c_str(), "rb")) == NULL)
		{
			BOOST_LOG_TRIVIAL(error) << "could not open " << texPath
									 << " for reading for input "
									 << inputConfig.id;
		}
		else
		{
			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;
			cinfo.err = jpeg_std_error(&jerr);

			jpeg_create_decompress(&cinfo);
			jpeg_stdio_src(&cinfo, infile);

			jpeg_read_header(&cinfo, TRUE);
			jpeg_start_decompress(&cinfo);

			bool decode = true;
			GLenum fmt = GL_RGB;
			if (cinfo.output_components == 1)
				fmt = GL_RED;
			else if (cinfo.output_components == 4)
				fmt = GL_RGBA;
			else if (cinfo.output_components != 3)
			{
				BOOST_LOG_TRIVIAL(error) << "unsupported component count for JPEG "
										 << texPath << " for input " << inputConfig.id;
				// Don't decode unknown format
				decode = false;
			}

			if (decode)
			{
				int stride = cinfo.output_width * cinfo.output_components;
				JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo,
															   JPOOL_IMAGE,
															   stride,
															   1);
				char *imgbuf = new char[cinfo.output_height * stride];

				while (cinfo.output_scanline < cinfo.output_height)
				{
					JDIMENSION read_now = jpeg_read_scanlines(&cinfo, buffer, 1);
					size_t off = cinfo.output_scanline - read_now;
					if (inputConfig.vflip)
						off = cinfo.output_height - off - read_now;
					memcpy(&imgbuf[off * stride], buffer[0], stride);
				}

				texture = make_shared<gl::texture>(GL_TEXTURE_2D);
				texture->image_2d(GL_TEXTURE_2D, 0, GL_RGBA32F, cinfo.output_width,
					cinfo.output_height, 0, fmt, GL_UNSIGNED_BYTE, imgbuf);

				delete[] imgbuf;
			}

			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
		}
	}
	else
	{
		// other, use SOIL
		texture = make_shared<gl::texture>(GL_TEXTURE_2D);
		GLuint texid = SOIL_load_OGL_texture(inputConfig.source.c_str(),
											 SOIL_LOAD_AUTO,
											 GLuint(*texture),
											 inputConfig.vflip ? SOIL_FLAG_INVERT_Y : 0);
		if (texid == 0)
		{
			BOOST_LOG_TRIVIAL(warning) << "failed to load '"
									   << inputConfig.source << "' for input " << inputConfig.id
									   << ": " << result_string_pointer;
			texture = shared_ptr<gl::texture>();
		}
	}

	if (texture)
	{
		BOOST_LOG_TRIVIAL(info) << "loaded '" << inputConfig.source
								<< "' for input " << inputConfig.id;
	}

	return texture;
}

shared_ptr<gl::texture> texture_engine::noise_texture_handler(const input_config &inputConfig,
															  bool &skipTextureOptions,
															  bool &skipCache,
															  bool &framebufferSized)
{
	// A noise texture
	auto noiseTexture = make_shared<gl::texture>(GL_TEXTURE_2D);
	noiseTexture->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	noiseTexture->parameter(GL_TEXTURE_SWIZZLE_G, GL_RED);

	// Create the actual noise
	vector<unsigned char> rnd(config_.width * config_.height);
	generate(rnd.begin(), rnd.end(), []() { return rand() % 256; });

	// Load it
	noiseTexture->image_2d(GL_TEXTURE_2D, 0, GL_RED, config_.width, config_.height,
		0, GL_RED, GL_UNSIGNED_BYTE, rnd.data());

	BOOST_LOG_TRIVIAL(warning) << "generated noise texture for input "
							   << inputConfig.id;

	framebufferSized = true;

	return noiseTexture;
}

shared_ptr<gl::texture> texture_engine::checker_texture_handler(const input_config &inputConfig,
																bool &skipTextureOptions,
																bool &skipCache,
																bool &framebufferSized)
{
	stringstream ss(inputConfig.source);
	int size = 0;
	ss >> size;
	if (ss.fail()) size = 10;

	// A checkerboard texture
	auto checkerTexture = make_shared<gl::texture>(GL_TEXTURE_2D);
	checkerTexture->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	checkerTexture->parameter(GL_TEXTURE_SWIZZLE_G, GL_RED);

	// Generate the checkerboard
	vector<unsigned char> chk(config_.width * config_.height);
	for (int i = 0; i < config_.width; ++i)
		for (int j = 1; j < config_.height; ++j)
			chk[j * config_.height + i] = ((i / size) % 2 == 0) ^ ((j / size) % 2 == 0) ? 255 : 0;

	// Load it
	checkerTexture->image_2d(GL_TEXTURE_2D, 0, GL_RED, config_.width, config_.height,
		0, GL_RED, GL_UNSIGNED_BYTE, chk.data());

	BOOST_LOG_TRIVIAL(warning) << "generated " << size << "x" << size
							   <<" checker texture for input "
							   << inputConfig.id;

	framebufferSized = true;

	return checkerTexture;
}

texture_engine::texture_engine(context_config &config)
	: config_(config)
{
	register_handler("texture", bind(&texture_engine::soil_texture_handler, this, _1, _2, _3, _4));
	register_handler("noise", bind(&texture_engine::noise_texture_handler, this, _1, _2, _3, _4));
	register_handler("checker", bind(&texture_engine::checker_texture_handler, this, _1, _2, _3, _4));
}

void texture_engine::init()
{
	// Prepare the empty texture, a nice magenta checkerboard
	if (empty_texture_) return;

	empty_texture_ = make_shared<gl::texture>(GL_TEXTURE_2D);

	// Generate the checkerboard
	int width = 32, height = 32,
		size = 16;
	vector<unsigned char> chk(width * height);
	for (int i = 0; i < width; ++i)
		for (int j = 1; j < height; ++j)
			chk[j * height + i] = ((i / size) % 2 == 0) ^ ((j / size) % 2 == 0) ? 255 : 0;

	// Load it and set parameters
	empty_texture_->parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	empty_texture_->parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	empty_texture_->parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	empty_texture_->parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	empty_texture_->parameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
	empty_texture_->image_2d(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
		GL_UNSIGNED_BYTE, chk.data());
}

void texture_engine::clear(bool framebufferSizeChange)
{
	if (!framebufferSizeChange)
	{
		// Clear all textures
		input_textures_.clear();
	}
	else
	{
		// Clear only framebuffer textures
		vector<string> clearTextures;

		// Find framebuffer textures
		for (auto &pair : input_textures_)
		{
			if (get<1>(pair.second))
			{
				clearTextures.push_back(pair.first);
			}
		}

		// Clear textures
		for (auto &obsoleteId : clearTextures)
		{
			input_textures_.erase(obsoleteId);
		}
	}
}

gl::texture &texture_engine::input_texture(const input_config &inputConfig)
{
	if (!inputConfig.enabled())
	{
		return *empty_texture_;
	}

	auto it = input_textures_.find(inputConfig.id);
	if (it == input_textures_.end())
	{
		auto handlerIt = handlers_.find(inputConfig.type);
		if (handlerIt != handlers_.end())
		{
			bool skipTextureOptions = false,
				 skipCache = false,
				 framebufferSized = false;
			auto texture = handlerIt->second(inputConfig,
											 skipTextureOptions,
											 skipCache,
											 framebufferSized);

			if (texture)
			{
				if (!skipTextureOptions)
					apply_texture_options(inputConfig, *texture);

				if (!skipCache)
					input_textures_.insert(make_pair(inputConfig.id,
						make_tuple(texture, framebufferSized)));
				else
					return *texture;
			}
			else
			{
				BOOST_LOG_TRIVIAL(error) << "failed loading " <<
					inputConfig.type << " texture for input " << inputConfig.id;

				if (!skipCache)
					input_textures_.insert(make_pair(inputConfig.id,
						make_tuple(empty_texture_, false)));
				else
					return *empty_texture_;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << "unknown texture type "
				<< inputConfig.type << " for input " << inputConfig.id;
		}
	}

	return *get<0>(input_textures_.find(inputConfig.id)->second);
}

void texture_engine::apply_texture_options(const input_config &inputConfig, gl::texture &texture)
{
	GLint minFilter = inputConfig.min_filter;

	texture.parameter(GL_TEXTURE_MIN_FILTER, minFilter);
	texture.parameter(GL_TEXTURE_MAG_FILTER, inputConfig.mag_filter);
	texture.parameter(GL_TEXTURE_WRAP_S, inputConfig.wrap);
	texture.parameter(GL_TEXTURE_WRAP_T, inputConfig.wrap);

	if (minFilter > GL_LINEAR)
	{
		BOOST_LOG_TRIVIAL(debug) << "generating mipmaps for " << inputConfig.id;
		texture.generate_mipmap();
	}
}

void texture_engine::register_handler(const string &name,
									  input_handler handler)
{
	if (name.empty())
		throw std::runtime_error("Input handler name cannot be empty.");

	BOOST_LOG_TRIVIAL(debug) << "registered " << name << " texture handler";
	handlers_.insert(make_pair(name, handler));
}
