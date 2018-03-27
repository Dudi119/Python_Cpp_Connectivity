#pragma once

#include <Python.h>
#include <type_traits>
#include <initializer_list>
#include <utility>
#include <string>
#include "CPythonObject.h"
#include "Common.h"

namespace pycppconn {

    template<typename ClassType, typename... Args>
    class CPythonConstructor {
    public:
        CPythonConstructor(CPythonConstructor &) = delete;
        CPythonConstructor &operator=(CPythonConstructor &) = delete;
        CPythonConstructor(CPythonConstructor &&obj) = delete;
        CPythonConstructor &operator=(CPythonConstructor &&obj) = delete;

        template<std::size_t... I>
        static int WrapperImpl(PyObject *self, PyObject *args, std::index_sequence<I...>) {
            std::initializer_list<const char *> formatList = {Object<typename base<Args>::Type>::Format...};
            std::string format;
            for (auto &subFormat : formatList)
                format += subFormat;

            char buffer[ObjectsPackSize<typename base<Args>::Type...>::value];
            {
                GilLock lock;
                CPYTHON_VERIFY(PyArg_ParseTuple(args, format.c_str(), (buffer +
                                                                       ObjectOffset<ObjectWrapper<typename base<Args>::Type, I>, ObjectWrapper<typename base<Args>::Type, I>...>::value)...),
                               "Invalid argument was provided");
            }
            new((char*)self + sizeof(PyObject))ClassType(std::forward<Args>(Object<typename base<Args>::Type>::GetTyped(
                    buffer +
                    ObjectOffset<ObjectWrapper<typename base<Args>::Type, I>, ObjectWrapper<typename base<Args>::Type, I>...>::value))...);


            ObjectWrapper<int, 0>::MultiDestructors(ObjectWrapper<typename base<Args>::Type, I>::Destructor(
                    buffer + ObjectOffset<ObjectWrapper<typename base<Args>::Type, I>,
                                                                                                                          ObjectWrapper<typename base<Args>::Type, I>...>::value)...);
            return 0;
        }

        static int Wrapper(PyObject *self, PyObject *args, PyObject *kwargs) {
            WrapperImpl(self, args, std::make_index_sequence<sizeof...(Args)>{});
        }

    private:
        CPythonConstructor(){
            static_assert(typeid(typename std::enable_if<std::is_constructible<ClassType, Args...>::type::value, bool>::type) == typeid(bool), "Type dosn't supports the associated constructor prototype.");
        };
    };

}