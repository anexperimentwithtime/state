// Copyright (C) 2025 Ian Torres <iantorres@outlook.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#ifndef AEWT_VERSION_HPP
#define AEWT_VERSION_HPP

namespace aewt::version {
    /**
     * Get major
     *
     * @return int
     */
    unsigned int get_major();

    /**
     * Get minor
     *
     * @return int
     */
    unsigned int get_minor();

    /**
     * Get patch
     *
     * @return int
     */
    unsigned int get_patch();
} // namespace aewt::version

#endif  // AEWT_VERSION_HPP
