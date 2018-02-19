#pragma once

#include <typeinfo>
#include <type_traits>
#include <vector>
#include <memory>
#include <Python.h>
#include <structmember.h>
#include "CPythonFunction.h"
#include "CPythonMember.h"
#include "CPyModuleContainer.h"
#include "CPythonModule.h"
#include "CPythonMetaClass.h"

namespace pycppconn{


    struct TypeState{
    public:
        TypeState(const std::string& name, const std::string& doc):
                Name(name), Doc(doc), PyType(nullptr){}
        ~TypeState(){
            delete[] PyType->tp_methods;
            delete[] PyType->tp_members;
        }
    public:
        std::string Name;
        std::string Doc;
        std::unique_ptr<PyTypeObject> PyType;
    };



    template<typename T, typename Type = typename std::remove_const<typename std::remove_pointer<
            typename std::remove_reference<T>::type>::type>::type>
    class CPythonClass{
    public:
        CPythonClass(CPythonModule& module, const std::string& name, const std::string& doc)
        :m_module(module), m_typeState(new TypeState(name, doc)){
            m_typeState->PyType.reset(new PyTypeObject{
                    PyVarObject_HEAD_INIT(&CPythonMetaClass::GetMetaType(), 0)
                    m_typeState->Name.c_str(), /* tp_name */
                    sizeof(Type) + sizeof(PyObject),/* tp_basicsize */
                    0,                         /* tp_itemsize */
                    NULL,                      /* tp_dealloc */
                    0,                         /* tp_print */
                    0,                         /* tp_getattr */
                    0,                         /* tp_setattr */
                    0,                         /* tp_compare */
                    0,                         /* tp_repr */
                    0,                         /* tp_as_number */
                    0,                         /* tp_as_sequence */
                    0,                         /* tp_as_mapping */
                    0,                         /* tp_hash */
                    0,                         /* tp_call */
                    0,                         /* tp_str */
                    0,                         /* tp_getattro */
                    0,                         /* tp_setattro */
                    0,                         /* tp_as_buffer */
                    Py_TPFLAGS_HAVE_CLASS |
                    Py_TPFLAGS_HEAPTYPE,       /* tp_flags */
                    m_typeState->Doc.c_str(),  /* tp_doc */
                    0,                         /* tp_traverse */
                    0,                         /* tp_clear */
                    0,                         /* tp_richcompare */
                    0,                         /* tp_weaklistoffset */
                    0,                         /* tp_iter */
                    0,                         /* tp_iternext */
                    NULL,                      /* tp_methods */
                    NULL,                      /* tp_members */
                    0,                         /* tp_getset */
                    0,                         /* tp_base */
                    0,                         /* tp_dict */
                    0,                         /* tp_descr_get */
                    0,                         /* tp_descr_set */
                    0,                         /* tp_dictoffset */
                    NULL,                      /* tp_init */
                    0,                         /* tp_alloc */
                    NULL,                      /* tp_new */
            });
            Py_IncRef((PyObject*)m_typeState->PyType.get()); //Making sure the true owner of the type is CPythonClass
        }
        ~CPythonClass(){
            AddMembers();
            AddMethods();
            {
                GilLock lock;
                PyType_Ready(m_typeState->PyType.get());
            }
            m_module.AddType(std::move(m_typeState));
        }

        template<typename X, typename std::enable_if<std::is_member_function_pointer<X>::value, bool>::type = true>
        void AddMethod(const std::string& name, const std::string& doc, X&& memberFunction){
            typedef CPythonFunction<Type, X> CPyFuncType;
            m_cPythonFunctions.emplace_back(new CPyFuncType(name, doc, memberFunction));
            CPyModuleContainer::Instance().AddMethod(GenerateMethodId<CPyFuncType>(), m_cPythonFunctions.back());
        }

        template<typename MemberType>
        void AddMember(const std::string& name, MemberType Type::* member, const std::string& doc){
            m_cPythonMembers.emplace_back( new CPythonMember<Type, MemberType>(name, member, doc));
        }

    private:
        template<typename CPythonFunctionType>
        static int GenerateMethodId(){
            return typeid(CPythonFunctionType).hash_code();
        }

        void AddMethods(){
            PyMethodDef* methods = new PyMethodDef[m_cPythonFunctions.size() + 1]; //spare space for sentinal
            m_typeState->PyType->tp_methods = methods;
            for(const auto& method : m_cPythonFunctions){
                *methods = *method->ToPython();
                methods++;
            }
            *methods = {NULL, NULL, 0, NULL};
        }

        void AddMembers(){
            PyMemberDef* members = new PyMemberDef[m_cPythonMembers.size() + 1]; //spare space for sentinal
            m_typeState->PyType->tp_members = members;
            for(const auto& member : m_cPythonMembers){
                *members = *member->ToPython();
                members++;
            }
            *members = {NULL, 0, 0, 0, NULL};
        }

        static int SetAttribute(PyObject *, PyObject *, PyObject *){

        }

    private:
        std::vector<std::shared_ptr<ICPythonFunction>> m_cPythonFunctions;
        std::vector<std::unique_ptr<ICPythonMember>> m_cPythonMembers;
        std::unique_ptr<TypeState> m_typeState;
        CPythonModule& m_module;
    };
}
