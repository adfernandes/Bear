/*  Copyright (C) 2012-2020 by László Nagy
    This file is part of Bear.

    Bear is a tool to generate compilation database for clang tooling.

    Bear is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bear is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "libsys/Path.h"

#include <algorithm>
#include <numeric>

namespace {

    std::list<std::string> split_by(const std::string& input, const char sep)
    {
        std::list<std::string> result;
        // make an early return if there is no chance to have something.
        if (input.empty()) {
            return result;
        }
        // otherwise start to collect the elements into result.
        std::string::size_type previous = 0;
        do {
            const std::string::size_type current = input.find(sep, previous);
            result.emplace_back(input.substr(previous, current - previous));
            previous = (current != std::string::npos) ? current + 1 : current;
        } while (previous != std::string::npos);

        return result;
    }

    std::string join_with(const std::list<std::string>& input, const char sep)
    {
        // make an early return if there is no chance to have something.
        if (input.empty()) {
            return "";
        }
        // otherwise start to collect the elements into result.
        return std::accumulate(std::next(input.begin()), input.end(),
            *input.begin(),
            [&sep](std::string acc, const std::string& item) {
                return std::move(acc) + sep + item;
            });
    }
}

namespace sys::path {

    std::list<std::string> split(const std::string& input)
    {
        return split_by(input, sys::path::OS_PATH_SEPARATOR);
    }

    std::string join(const std::list<std::string>& input)
    {
        return join_with(input, sys::path::OS_PATH_SEPARATOR);
    }

    bool is_absolute(const std::string& path) {
        return (!path.empty()) && (path[0] == OS_SEPARATOR);
    }

    bool is_relative(const std::string& path) {
        return !is_absolute(path);
    }

    std::string relative(const std::string& path, const std::string &start)
    {
        auto path_elements = split_by(path, OS_SEPARATOR);
        path_elements.remove(".");

        auto start_elements = split_by(start, OS_SEPARATOR);
        start_elements.remove(".");
        // remove the common root directories.
        const size_t common = std::min(path_elements.size(), start_elements.size());
        for (size_t idx = 0; idx < common; ++idx) {
            if (path_elements.front() == start_elements.front()) {
                path_elements.pop_front();
                start_elements.pop_front();
                continue;
            }
            break;
        }
        // insert as many `..` as the length of the start has now.
        std::fill_n(
                std::front_inserter(path_elements),
                start_elements.size(),
                std::string(".."));

        return join_with(path_elements, OS_SEPARATOR);
    }

    std::string basename(const std::string& path)
    {
        if (auto pos = path.rfind(OS_SEPARATOR); pos != std::string::npos) {
            return path.substr(pos + 1);
        }
        return path;
    }

    std::string concat(const std::string& dir, const std::string& file) {
        return dir + OS_SEPARATOR + file;
    }
}
