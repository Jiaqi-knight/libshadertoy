#ifndef _SHADERTOY_TEXTURE_ENGINE_HPP_
#define _SHADERTOY_TEXTURE_ENGINE_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * Handler callback definition for the texture engine. Instances of this method
 * are called when a texture is required for an input in a rendering context.
 * 
 * `inputConfig` is the configuration object for the given input.
 * `skipTextureOptions` should be set to true if no wrapping or filtering
 * options should be automatically applied. `skipCache` should be set to true if
 * the method should be invoked each time a texture for this input is requested.
 */
typedef std::function<std::shared_ptr<oglplus::Texture>(
	const InputConfig &inputConfig,
	bool &skipTextureOptions,
	bool &skipCache)> InputHandler;

/**
 * @brief      Represents the engine responsible for loading input textures for
 *             the current rendering context.
 */
class TextureEngine
{
	/// Context configuration reference
	ContextConfig &config;

	/// Stateless OpenGL context
	oglplus::Context gl;

	/// Input texture state
	std::map<std::string, std::shared_ptr<oglplus::Texture>> inputTextures;

	/// The empty texture
	std::shared_ptr<oglplus::Texture> emptyTexture;

	/// Registered texture handlers
	std::map<std::string, InputHandler> handlers;

	// Default texture handlers
	std::shared_ptr<oglplus::Texture> SOILTextureHandler(const InputConfig &inputConfig,
		bool &skipTextureOptions,
		bool &skipCache);
	std::shared_ptr<oglplus::Texture> NoiseTextureHandler(const InputConfig &inputConfig,
		bool &skipTextureOptions,
		bool &skipCache);
	std::shared_ptr<oglplus::Texture> CheckerTextureHandler(const InputConfig &inputConfig,
		bool &skipTextureOptions,
		bool &skipCache);

protected:
	/**
	 * @brief      Applies filtering and wrapping options of a given input onto
	 *             an already allocated OpenGL texture object.
	 *
	 * @param[in]  inputConfig  The input configuration for this texture
	 * @param      texture      The texture to apply the options to
	 */
	void ApplyTextureOptions(const InputConfig &inputConfig,
							 std::shared_ptr<oglplus::Texture> &texture);

public:
	/**
	 * @brief      Initialize a new instance of the texture engine class.
	 *
	 * @param      config  Context configuration to use for this engine.
	 */
	TextureEngine(ContextConfig &config);

	/**
	 * @brief      Initialize this texture engine
	 */
	void Initialize();

	/**
	 * @brief      Clears all loaded textures.
	 */
	void ClearState();

	/**
	 * @brief      Get the texture for the given input
	 *
	 * @param[in]  inputConfig  The input configuration
	 *
	 * @return     A texture instance to be used for rendering. If the actual
	 *             input configuration is invalid, an empty texture is returned.
	 */
	oglplus::Texture &GetInputTexture(const InputConfig &inputConfig);

	/**
	 * @brief      Registers a texture type handler with the given name.
	 *
	 * @param[in]  name     Name (type ID) of the texture handler
	 * @param[in]  handler  Handler function. See InputHandler for more details.
	 */
	void RegisterHandler(const std::string &name, InputHandler handler);
};

}

#endif /* _SHADERTOY_TEXTURE_ENGINE_HPP_ */
