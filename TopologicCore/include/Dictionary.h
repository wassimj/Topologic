#pragma once

#include <map>
#include <list>
#include <string>
#include <memory>

#include "Utilities.h"
#include "Attribute.h"

namespace TopologicCore
{
    class TOPOLOGIC_API Dictionary :
        public std::map <std::string, Attribute::Ptr>
    {
    public:
        void Add(const std::string& key, const Attribute::Ptr& value);
        void Add(const std::pair<std::string, Attribute::Ptr>& pair);

        void Clear();

        bool ContainsKey(const std::string& key) const;
        // bool ContainsValue(const Attribute::Ptr& value) const;
        bool Contains(const std::pair<std::string, Attribute::Ptr>& pair) const;

        //int EnsureCapacity(int capacity);

        bool Remove(const std::string& key);
        bool Remove(const std::pair<std::string, Attribute::Ptr>& pair);

        bool TryAdd(const std::string& key, const Attribute::Ptr& Value);
        bool TryGetValue(const std::string& key, const Attribute::Ptr& Value);

        size_t Count() { return this->size(); }

        std::list<std::string> Keys();
        std::list<Attribute::Ptr> Values();

        void CopyTo(std::pair<std::string, Attribute::Ptr> target[], int index, int len) const;

        auto GetEnumerator() { return this->begin(); }

        bool IsReadOnly() { return false; }
        bool IsFixedSize() { return false; }

        static Dictionary ByKeysValues(const std::list<std::string>& keys, std::list< Attribute::Ptr>& values);
        Attribute::Ptr ValueAtKey(const std::string& key) { return (*this)[key]; }


        /*
        GetEnumerator
        GetObjectData
        OnDeserialization
        TrimExcess
        ICollection.IsSynchronized
        ICollection.SyncRoot
        */

    };
}
