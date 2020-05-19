/*
 * Copyright 2020 UBports foundation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Erfan Abdi <erfangplus@gmail.com>
 */

#include "overlay-reader.h"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace overlayreader {

std::string OverlaySuffix = ".apk";

std::string overlay_paths[] = {
    "/system/vendor/overlay/",
    "/vendor/overlay",
    "/system/vendor/overlay-subdir/",
    "/system/product/overlay/",
    "/product/overlay",
    "/system/overlay"};

bool CheckTarget(const std::string &target, const char *path) {
	int err = 0;
	zip *rro_apk = zip_open(path, 0, &err);
	if (err)
		return false;

	zip_file *rro_manifest = zip_fopen(rro_apk, "AndroidManifest.xml", 0);
	if (!rro_manifest) {
		zip_close(rro_apk);
		return false;
	}

	struct zip_stat st;
	zip_stat_init(&st);
	zip_stat(rro_apk, "AndroidManifest.xml", 0, &st);
	char *contents = new char[st.size];
	char *outbuf;
	size_t outsize;

	zip_fread(rro_manifest, contents, st.size);
	err = AxmlToXml(&outbuf, &outsize, contents, st.size);
	zip_fclose(rro_manifest);
	zip_close(rro_apk);
	if (err != 0) 
		return false;
	std::string xmlTargetPackage = "android:targetPackage=\"" + target + "\"";
	if (strstr(outbuf, xmlTargetPackage.c_str()) != NULL)
		return true;

	return false;
}

bool EndsWithSuffix(const char *string) {
	string = strrchr(string, '.');
	if (string != NULL)
		return (strcmp(string, OverlaySuffix.c_str()) == 0);
	return false;
}

std::string FindPackage(const std::string &target) {
	for (const std::string &dir_path : overlay_paths) {
		if (!exists(dir_path))
			continue;
		directory_iterator end_itr;
		for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {
			if (! is_directory(itr->status())) {
				path path_found = itr->path();
				if (EndsWithSuffix(itr->path().filename().c_str())) {
					if (CheckTarget(target, path_found.c_str()))
						return std::string(path_found.c_str());
				}
			}
		}
	}
	return "NULL";
}

std::string GetConfigByPath(const std::string &path,
							const std::string &config,
							const std::string &subtype,
							const std::string &default_value)
{
	ResourcesParser parser;
	std::string value = default_value;
	int err = 0;
	bool isfile = false;

	zip *rro_apk = zip_open(path.c_str(), 0, &err);
	if (err)
		return value;

	zip_file *rro_arsc = zip_fopen(rro_apk, "resources.arsc", 0);
	if (!rro_arsc) {
		zip_close(rro_apk);
		return value;
	}
	parser.SetResourcesZip(rro_arsc);
	parser.SetupResourcesParser();
	ResourcesParserInterpreter interpreter(&parser);
	zip_fclose(rro_arsc);

	value = interpreter.parserName(config, subtype, default_value, isfile);
	if (isfile)
	{
		zip_file *fraw = zip_fopen(rro_apk, value.c_str(), 0);
		if (!fraw) {
			zip_close(rro_apk);
			return default_value;
		}
		struct zip_stat st;
		zip_stat_init(&st);
		zip_stat(rro_apk, value.c_str(), 0, &st);

		char *contents = new char[st.size];
		char *outbuf;
		size_t outsize;

		zip_fread(fraw, contents, st.size);
		if (strcmp(strrchr(value.c_str(), '.'), ".xml") == 0) {
			int ret = AxmlToXml(&outbuf, &outsize, contents, st.size);
			if (ret == 0)
				value = outbuf;
			else
				value = default_value;
		} else {
			value = contents;
		}
		zip_fclose(fraw);
	}

	zip_close(rro_apk);
	return value;
}

std::string GetConfigByPath(const std::string &path,
							const std::string &config,
							const std::string &default_value)
{
	return GetConfigByPath(path, config, "", default_value);
}

std::string GetConfigByTarget(const std::string &target,
							  const std::string &config,
							  const std::string &subtype,
							  const std::string &default_value)
{
	std::string path = FindPackage(target);
	if (path != "NULL")
		return GetConfigByPath(path, config, subtype, default_value);
	else
		return default_value;
}

std::string GetConfigByTarget(const std::string &target,
							  const std::string &config,
							  const std::string &default_value)
{
	return GetConfigByTarget(target, config, "", default_value);
}
}