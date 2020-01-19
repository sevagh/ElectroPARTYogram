#ifndef ANIMALS_AS_METER_CIRCULARBUFFER_H
#define ANIMALS_AS_METER_CIRCULARBUFFER_H

// modified for Animals-as-Meter by Sevag
// the size should be a power of two to use efficient bitwise modulo &(size-1)
//=======================================================================
/** @file CircularBuffer.h
 *  @brief A class for calculating onset detection functions
 *  @author Adam Stark
 *  @copyright Copyright (C) 2008-2014  Queen Mary University of London
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
//=======================================================================

#include <cstddef>
#include <vector>

//=======================================================================
/** A circular buffer that allows you to add new samples to the end
 * whilst removing them from the beginning. This is implemented in an
 * efficient way which doesn't involve any memory allocation
 */
namespace circbuf {
class CircularBuffer {
public:
	std::vector<float> buffer;

	CircularBuffer(std::size_t size)
	    : buffer(std::vector<float>(size, 0.0f))
	    , writeIndex(0){};

	float& operator[](std::size_t i)
	{
		return buffer[(i + writeIndex) & (buffer.size() - 1)];
	}

	void addSampleToEnd(float v)
	{
		buffer[writeIndex] = v;
		writeIndex = (writeIndex + 1) & (buffer.size() - 1);
	}

private:
	std::size_t writeIndex;
};
} // namespace circbuf

#endif // ANIMALS_AS_METER_CIRCULARBUFFER_H
