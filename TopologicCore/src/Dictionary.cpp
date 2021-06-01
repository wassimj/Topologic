#include "Dictionary.h"

#include <algorithm>
#include <stdexcept>

namespace TopologicCore
{

    void Dictionary::Add(const std::string& key, const Attribute::Ptr& value)
    {
        if (ContainsKey(key))
            throw std::invalid_argument("std::string already exists");

        (*this)[key] = value;
    }

    void Dictionary::Add(const std::pair<std::string, Attribute::Ptr>& pair)
    {
        if (ContainsKey(pair.first))
            throw std::invalid_argument("std::string already exists");

        (*this)[pair.first] = pair.second;
    }


    void Dictionary::Clear()
    {
        this->clear();
    }


    bool Dictionary::ContainsKey(const std::string& key) const
    {
        return this->find(key) != this->end();
    }


    /*bool Dictionary::ContainsValue(const Attribute::Ptr& value) const
    {
        auto it = std::find_if(this->begin(), this->end(),
            [&value](auto& entry) {return *entry.second == *value; }
        );

        return it != this->end();
    }*/


    bool Dictionary::Contains(const std::pair<std::string, Attribute::Ptr>& pair) const
    {
        auto iter = this->find(pair.first);
        return iter != this->end() && pair.second == iter->second;
    }



    /*int Dictionary::EnsureCapacity(int capacity)
    {
        this->reserve(capacity);
        return capacity;
    }*/


    bool Dictionary::Remove(const std::string& key)
    {
        if (ContainsKey(key))
        {
            this->erase(key);
            return true;
        }
        return false;
    }


    bool Dictionary::Remove(const std::pair<std::string, Attribute::Ptr>& pair)
    {
        auto iter = this->find(pair.first);

        if (iter != this->end()) {
            if (iter->second == pair.second)
            {
                this->erase(iter);
                return true;
            }
        }

        return false;
    }


    bool Dictionary::TryAdd(const std::string& key, const Attribute::Ptr& value)
    {
        if (ContainsKey(key))
            return false;

        (*this)[key] = value;
        return true;
    }


    bool Dictionary::TryGetValue(const std::string& key, const Attribute::Ptr& value)
    {
        if (ContainsKey(key))
        {
            *value = *(*this)[key];
        }

        return false;
    }


    std::list<std::string> Dictionary::Keys()
    {
        std::list<std::string> keys;

        for (auto iter : *this)
            keys.push_back(iter.first);

        return keys;
    }


    std::list<Attribute::Ptr> Dictionary::Values()
    {
        std::list<Attribute::Ptr> values;

        for (auto iter : *this)
            values.push_back(iter.second);

        return values;
    }


    void Dictionary::CopyTo(std::pair<std::string, Attribute::Ptr> target[], int index, int len) const
    {
        if (len - index < this->size())
            throw std::invalid_argument("Array is not big enough");

        for (auto iter : *this)
            target[index++] = std::make_pair(iter.first, iter.second);
    }

    Dictionary Dictionary::ByKeysValues(const std::list<std::string>& keys, std::list<Attribute::Ptr>& values)
    {
        if (keys.size() != values.size())
            throw std::invalid_argument("Keys and values have a different size");

        Dictionary dict;
        auto key = keys.begin();
        auto val = values.begin();

        for (; key != keys.end() && val != values.end(); key++, val++)
            dict[*key] = *val;

        return dict;
    }

}