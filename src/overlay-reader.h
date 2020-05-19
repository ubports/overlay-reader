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

#ifndef OVERLAY_READER_H
#define OVERLAY_READER_H

#include "arcsParser/ResourcesParser.h"
#include "arcsParser/ResourcesParserInterpreter.h"
#include "axmlParser/AxmlParser.h"

namespace overlayreader {

std::string FindPackage(const std::string &target);

std::string GetConfigByPath(const std::string &path,
                            const std::string &config,
                            const std::string &default_value);

std::string GetConfigByPath(const std::string &path,
                            const std::string &config,
                            const std::string &subtype,
                            const std::string &default_value);

std::string GetConfigByTarget(const std::string &target,
                              const std::string &config,
                              const std::string &default_value);

std::string GetConfigByTarget(const std::string &target,
                              const std::string &config,
                              const std::string &subtype,
                              const std::string &default_value);
}

#endif /*OVERLAY_READER_H*/
