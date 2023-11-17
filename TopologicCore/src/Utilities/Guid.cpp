#include "Utilities/Guid.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <objbase.h>

#elif defined(__linux__) || defined(__unix__)
#include <uuid/uuid.h>

#elif defined(__APPLE__)
#include <CoreFoundation/CFUUID.h>

#endif

namespace
{
    // converts a single hex char to a number (0 - 15)
    unsigned char hexDigitToChar(char ch)
    {
        // 0-9
        if (ch > 47 && ch < 58)
            return ch - 48;

        // a-f
        if (ch > 96 && ch < 103)
            return ch - 87;

        // A-F
        if (ch > 64 && ch < 71)
            return ch - 55;

        return 0;
    }

    bool isValidHexChar(char ch)
    {
        // 0-9
        if (ch > 47 && ch < 58)
            return true;

        // a-f
        if (ch > 96 && ch < 103)
            return true;

        // A-F
        if (ch > 64 && ch < 71)
            return true;

        return false;
    }

    // converts the two hexadecimal characters to an unsigned char (a byte)
    unsigned char hexPairToChar(char a, char b)
    {
        return hexDigitToChar(a) * 16 + hexDigitToChar(b);
    }
}

namespace TopologicUtilities
{
    // overload << so that it's easy to convert to a string
    std::ostream &operator<<(std::ostream &s, const Guid &guid)
    {
        std::ios_base::fmtflags f(s.flags()); // politely don't leave the ostream in hex mode
        s << std::hex << std::setfill('0')
          << std::setw(2) << (int)guid._bytes[0]
          << std::setw(2) << (int)guid._bytes[1]
          << std::setw(2) << (int)guid._bytes[2]
          << std::setw(2) << (int)guid._bytes[3]
          << "-"
          << std::setw(2) << (int)guid._bytes[4]
          << std::setw(2) << (int)guid._bytes[5]
          << "-"
          << std::setw(2) << (int)guid._bytes[6]
          << std::setw(2) << (int)guid._bytes[7]
          << "-"
          << std::setw(2) << (int)guid._bytes[8]
          << std::setw(2) << (int)guid._bytes[9]
          << "-"
          << std::setw(2) << (int)guid._bytes[10]
          << std::setw(2) << (int)guid._bytes[11]
          << std::setw(2) << (int)guid._bytes[12]
          << std::setw(2) << (int)guid._bytes[13]
          << std::setw(2) << (int)guid._bytes[14]
          << std::setw(2) << (int)guid._bytes[15];
        s.flags(f);
        return s;
    }

    bool operator<(const Guid &lhs, const Guid &rhs)
    {
        return lhs.bytes() < rhs.bytes();
    }

    bool Guid::isValid() const
    {
        Guid empty;
        return *this != empty;
    }

    // convert to string using std::snprintf() and std::string
    std::string Guid::str() const
    {
        char one[10], two[6], three[6], four[6], five[14];

        snprintf(one, 10, "%02x%02x%02x%02x",
                 _bytes[0], _bytes[1], _bytes[2], _bytes[3]);
        snprintf(two, 6, "%02x%02x",
                 _bytes[4], _bytes[5]);
        snprintf(three, 6, "%02x%02x",
                 _bytes[6], _bytes[7]);
        snprintf(four, 6, "%02x%02x",
                 _bytes[8], _bytes[9]);
        snprintf(five, 14, "%02x%02x%02x%02x%02x%02x",
                 _bytes[10], _bytes[11], _bytes[12], _bytes[13], _bytes[14], _bytes[15]);
        const std::string sep("-");
        std::string out(one);

        out += sep + two;
        out += sep + three;
        out += sep + four;
        out += sep + five;

        return out;
    }

    // conversion operator for std::string
    Guid::operator std::string() const
    {
        return str();
    }

    // Access underlying bytes
    const std::array<unsigned char, 16> &Guid::bytes() const
    {
        return _bytes;
    }

    // create a guid from vector of bytes
    Guid::Guid(const std::array<unsigned char, 16> &bytes) : _bytes(bytes)
    {
    }

    // create a guid from vector of bytes
    Guid::Guid(std::array<unsigned char, 16> &&bytes) : _bytes(std::move(bytes))
    {
    }

    // create a guid from string
    Guid::Guid(const std::string &fromString)
    {
        char charOne = '\0';
        char charTwo = '\0';
        bool lookingForFirstChar = true;
        unsigned nextByte = 0;

        for (const char &ch : fromString)
        {
            if (ch == '-')
                continue;

            if (nextByte >= 16 || !isValidHexChar(ch))
            {
                // Invalid string so bail
                zeroify();
                return;
            }

            if (lookingForFirstChar)
            {
                charOne = ch;
                lookingForFirstChar = false;
            }
            else
            {
                charTwo = ch;
                auto byte = hexPairToChar(charOne, charTwo);
                _bytes[nextByte++] = byte;
                lookingForFirstChar = true;
            }
        }

        // if there were fewer than 16 bytes in the string then guid is bad
        if (nextByte < 16)
        {
            zeroify();
            return;
        }
    }

    // create empty guid
    Guid::Guid() : _bytes{{0}}
    {
    }

    // set all bytes to zero
    void Guid::zeroify()
    {
        std::fill(_bytes.begin(), _bytes.end(), static_cast<unsigned char>(0));
    }

    // overload equality operator
    bool Guid::operator==(const Guid &other) const
    {
        return _bytes == other._bytes;
    }

    // overload inequality operator
    bool Guid::operator!=(const Guid &other) const
    {
        return !((*this) == other);
    }

    // member swap function
    void Guid::swap(Guid &other)
    {
        _bytes.swap(other._bytes);
    }

// This is the windows version
#ifdef _WIN32
    Guid newGuid()
    {
        GUID newId;
        CoCreateGuid(&newId);

        std::array<unsigned char, 16> bytes =
            {
                (unsigned char)((newId.Data1 >> 24) & 0xFF),
                (unsigned char)((newId.Data1 >> 16) & 0xFF),
                (unsigned char)((newId.Data1 >> 8) & 0xFF),
                (unsigned char)((newId.Data1) & 0xff),

                (unsigned char)((newId.Data2 >> 8) & 0xFF),
                (unsigned char)((newId.Data2) & 0xff),

                (unsigned char)((newId.Data3 >> 8) & 0xFF),
                (unsigned char)((newId.Data3) & 0xFF),

                (unsigned char)newId.Data4[0],
                (unsigned char)newId.Data4[1],
                (unsigned char)newId.Data4[2],
                (unsigned char)newId.Data4[3],
                (unsigned char)newId.Data4[4],
                (unsigned char)newId.Data4[5],
                (unsigned char)newId.Data4[6],
                (unsigned char)newId.Data4[7]};

        return Guid{std::move(bytes)};
    }

/* This is the linux friendly implementation, but it could work on other
 * systems that have libuuid available
 */
#elif defined(__linux__) || defined(__unix__)
    Guid newGuid()
    {
        std::array<unsigned char, 16> data;
        static_assert(std::is_same<unsigned char[16], uuid_t>::value, "Wrong type!");
        uuid_generate(data.data());
        return Guid{std::move(data)};
    }

// This is the mac os version
#elif defined(__APPLE__)
    Guid newGuid()
    {
        auto newId = CFUUIDCreate(NULL);
        auto bytes = CFUUIDGetUUIDBytes(newId);
        CFRelease(newId);

        std::array<unsigned char, 16> byteArray =
            {{bytes.byte0,
              bytes.byte1,
              bytes.byte2,
              bytes.byte3,
              bytes.byte4,
              bytes.byte5,
              bytes.byte6,
              bytes.byte7,
              bytes.byte8,
              bytes.byte9,
              bytes.byte10,
              bytes.byte11,
              bytes.byte12,
              bytes.byte13,
              bytes.byte14,
              bytes.byte15}};
        return Guid{std::move(byteArray)};
    }

#endif
} // namespace TopologicUtilities
