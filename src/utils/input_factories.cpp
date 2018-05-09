#include <algorithm>
#include <sstream>

#include <boost/filesystem.hpp>
#include <epoxy/gl.h>

#include "shadertoy/gl.hpp"

#include "shadertoy/utils/input_factories.hpp"

#include "shadertoy/inputs/exr_input.hpp"
#include "shadertoy/inputs/jpeg_input.hpp"
#include "shadertoy/inputs/soil_input.hpp"

#include "shadertoy/inputs/checker_input.hpp"
#include "shadertoy/inputs/noise_input.hpp"

using namespace shadertoy;
using namespace shadertoy::utils;

std::string file_ext(const std::string &path)
{
	boost::filesystem::path p(path);
	auto ext(p.extension().string());
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext;
}

bool soil_input_factory::supported(const std::map<std::string, std::string> &spec) const
{
	auto ext(file_ext(spec.at("")));
	return inputs::soil_input::supported() &&
		(ext == ".bmp" || ext == ".png" || ext == ".jpg" || ext == ".tga" || ext == ".dds" || ext == ".psd" || ext == ".hdr");
}

std::shared_ptr<inputs::basic_input> soil_input_factory::create(const std::map<std::string, std::string> &spec) const
{
	return std::make_shared<inputs::soil_input>(spec.at(""));
}

soil_input_factory::soil_input_factory()
	: type_name_("file")
{}

bool jpeg_input_factory::supported(const std::map<std::string, std::string> &spec) const
{
	auto ext(file_ext(spec.at("")));
	return inputs::jpeg_input::supported() &&
		(ext == ".jpg" || ext == ".jpeg");
}

std::shared_ptr<inputs::basic_input> jpeg_input_factory::create(const std::map<std::string, std::string> &spec) const
{
	return std::make_shared<inputs::jpeg_input>(spec.at(""));
}

jpeg_input_factory::jpeg_input_factory()
	: type_name_("file")
{}

bool exr_input_factory::supported(const std::map<std::string, std::string> &spec) const
{
	auto ext(file_ext(spec.at("")));
	return inputs::exr_input::supported() && ext == ".exr";
}

std::shared_ptr<inputs::basic_input> exr_input_factory::create(const std::map<std::string, std::string> &spec) const
{
	return std::make_shared<inputs::exr_input>(spec.at(""));
}

exr_input_factory::exr_input_factory()
	: type_name_("file")
{}

int get_int(const std::map<std::string, std::string> &spec, const std::string &key, int def)
{
	auto it = spec.find(key);
	if (it != spec.end())
	{
		std::istringstream iss(it->second);
		int result;
		iss >> result;
		if (!iss.fail())
			return result;
	}
	
	return def;
}

std::shared_ptr<inputs::basic_input> noise_input_factory::create(const std::map<std::string, std::string> &spec) const
{
	return std::make_shared<inputs::noise_input>(make_size(rsize(get_int(spec, "width", 128),
																 get_int(spec, "height", 128))));
}

noise_input_factory::noise_input_factory()
	: type_name_("noise")
{}

std::shared_ptr<inputs::basic_input> checker_input_factory::create(const std::map<std::string, std::string> &spec) const
{
	return std::make_shared<inputs::checker_input>(make_size(rsize(get_int(spec, "width", 128),
																   get_int(spec, "height", 128))),
												   get_int(spec, "size", 16));
}

checker_input_factory::checker_input_factory()
	: type_name_("checker")
{}
