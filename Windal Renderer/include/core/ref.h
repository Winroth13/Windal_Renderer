#pragma once

/*
* 
*  Simple implementation to skip having to write out
*  std::shared_ptr<T>. Thanks to the Cherno for this code!
* 
*/

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}