#pragma once

#include <Python.h>

namespace sweetPy {

    struct GilLock
    {
    public:
        GilLock() { m_state = PyGILState_Ensure(); }
        ~GilLock() { PyGILState_Release(m_state); }
    private:
        PyGILState_STATE m_state;
    };

    struct Yield
    {
    public:
        Yield()
        {
            if(m_save == nullptr && PyGILState_Check())
                m_save = PyEval_SaveThread();
        }
        ~Yield()
        {
            if(m_save)
            {
                PyEval_RestoreThread(m_save);
                m_save = nullptr;
            }
        }
    private:
        thread_local static PyThreadState* m_save;
    };
}

