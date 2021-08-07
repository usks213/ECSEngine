/*****************************************************************//**
 * \file   ConstexprMap.h
 * \brief  メタマップ
 * 
 * \author USAMI KOSHI
 * \date   2021/08/03
 *********************************************************************/
#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <string_view>
#include <algorithm>


//template<typename Key, typename Value, size_t Size>
//class ConstexprMap
//{
//private:
//    static_assert(std::is_invocable_v<std::less<>, Key, Key>, "Key must be < comparable");
//    using value_type = std::pair<Key, Value>;
//    using container_type = std::array<value_type, Size>;
//    container_type _arr;
//
//    template<size_t... Is>
//    constexpr ConstexprMap(const value_type(&arr)[Size], std::index_sequence<Is...>) noexcept :
//        _arr{ {{arr[Is].first, arr[Is].second}...} }
//    {
//        for (size_t i = 0; i < _arr.size() - 1; ++i) {
//            for (size_t j = 0; j < _arr.size() - 1 - i; ++j) {
//                if (_arr[j + 1].first < _arr[j].first) {
//                    auto tmp = _arr[j];
//                    _arr[j].first = _arr[j + 1].first;
//                    _arr[j].second = _arr[j + 1].second;
//                    _arr[j + 1].first = tmp.first;
//                    _arr[j + 1].second = tmp.second;
//                }
//            }
//        }
//    }
//
//public:
//    template<typename K, typename V, size_t N>
//    friend constexpr auto make_map(const std::pair<K, V>(&arr)[N]);
//
//    constexpr auto begin() const noexcept
//    {
//        return _arr.begin();
//    }
//
//    constexpr auto end() const noexcept
//    {
//        return _arr.end();
//    }
//
//    constexpr auto find(const Key& key) const noexcept -> typename container_type::const_iterator
//    {
//        auto left = begin();
//        auto right = end();
//        while (left < right) {
//            const auto mid = left + (right - left) / 2;
//            if (mid->first < key) {
//                left = mid + 1;
//            }
//            else {
//                right = mid;
//            }
//        }
//
//        if (left != end() && left->first == key) {
//            return left;
//        }
//
//        return _arr.end();
//    }
//};
//
//template<typename Key, typename Value, size_t N>
//constexpr auto make_map(const std::pair<Key, Value>(&arr)[N])
//{
//    ConstexprMap<Key, Value, N> result{ arr, std::make_index_sequence<N>{} };
//    return result;
//}


template<typename Key, typename Value, size_t Size>
class ConstexprMap
{
private:
    static_assert(std::is_invocable_v<std::less<>, Key, Key>, "Key must be < comparable");
    using value_type = std::pair<Key, Value>;
    using container_type = std::array<value_type, Size>;
    container_type _arr;

    template<size_t... Is>
    constexpr ConstexprMap(const value_type(&arr)[Size], std::index_sequence<Is...>) noexcept :
        _arr{ {{arr[Is].first, arr[Is].second}...} }
    {
        for (size_t i = 0; i < _arr.size() - 1; ++i) {
            for (size_t j = 0; j < _arr.size() - 1 - i; ++j) {
                if (_arr[j + 1].first < _arr[j].first) {
                    auto tmp = _arr[j];
                    _arr[j].first = _arr[j + 1].first;
                    _arr[j].second = _arr[j + 1].second;
                    _arr[j + 1].first = tmp.first;
                    _arr[j + 1].second = tmp.second;
                }
            }
        }
    }

public:
    constexpr ConstexprMap() noexcept 
    {
        for (size_t i = 0; i < _arr.size(); ++i)
        {
            _arr[i].first = 0;
        }
    }

    template<typename K, typename V, size_t N>
    friend constexpr auto make_map(const std::pair<K, V>(&arr)[N]);

    constexpr auto begin() const noexcept
    {
        return _arr.begin();
    }

    constexpr auto end() const noexcept
    {
        return _arr.end();
    }

    constexpr auto find(const Key& key) const noexcept -> typename container_type::const_iterator
    {
        auto left = begin();
        auto right = end();
        while (left < right) {
            const auto mid = left + (right - left) / 2;
            if (mid->first < key) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }

        if (left != end() && left->first == key) {
            return left;
        }

        return _arr.end();
    }

    constexpr void push(const Key& key, const Value& value) noexcept
    {
        _arr.back().first = key;
        _arr.back().second = value;
        for (size_t i = 0; i < _arr.size() - 1; ++i) {
            for (size_t j = 0; j < _arr.size() - 1 - i; ++j) {
                if (_arr[j + 1].first < _arr[j].first) {
                    auto tmp = _arr[j];
                    _arr[j].first = _arr[j + 1].first;
                    _arr[j].second = _arr[j + 1].second;
                    _arr[j + 1].first = tmp.first;
                    _arr[j + 1].second = tmp.second;
                }
            }
        }
    }
};

template<typename Key, typename Value, size_t N>
constexpr auto make_map(const std::pair<Key, Value>(&arr)[N])
{
    ConstexprMap<Key, Value, N> result{ arr, std::make_index_sequence<N>{} };
    return result;
}