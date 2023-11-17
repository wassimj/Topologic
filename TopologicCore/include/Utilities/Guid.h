/* This is a cross-platform implementation of UUID.
 * Using a big library like boost to create an UUID is not suitable,
 * So, the implementation is referenced from https://github.com/graeme-hill/crossguid
 * repository in which is under the MIT license.
 */

#pragma once

#include <functional>
#include <iostream>
#include <array>
#include <sstream>
#include <string>
#include <utility>
#include <iomanip>

namespace TopologicUtilities
{
    /* Class to represent a GUID/UUID. Each instance acts as a wrapper around a
     * 16 byte value that can be passed around by value. It also supports
     * conversion to string (via the stream operator <<) and conversion from a
     * string via constructor.
     */
    class Guid
    {
    public:
        explicit Guid(const std::array<unsigned char, 16> &bytes);
        explicit Guid(std::array<unsigned char, 16> &&bytes);

        explicit Guid(const std::string &fromString);
        Guid();

        Guid(const Guid &other) = default;
        Guid &operator=(const Guid &other) = default;
        Guid(Guid &&other) = default;
        Guid &operator=(Guid &&other) = default;

        bool operator==(const Guid &other) const;
        bool operator!=(const Guid &other) const;

        std::string str() const;
        operator std::string() const;
        const std::array<unsigned char, 16> &bytes() const;
        void swap(Guid &other);
        bool isValid() const;

    private:
        void zeroify();

        // actual data
        std::array<unsigned char, 16> _bytes;

        // make the << operator a friend so it can access _bytes
        friend std::ostream &operator<<(std::ostream &s, const Guid &guid);
        friend bool operator<(const Guid &lhs, const Guid &rhs);
    };

    Guid newGuid();

} // namespace TopologicUtilities
