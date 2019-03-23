#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>


static PyObject *marshal = NULL;
static PyObject *marshal_loads = NULL;


int LLVMFuzzerInitialize(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    Py_Initialize();

    if (PyErr_Occurred()) {
        fprintf(stderr, "error: failed to initialize Python:\n");
        PyErr_Print();
        abort();
    }

    marshal = PyImport_ImportModule("marshal");
    if (marshal == NULL) {
        fprintf(stderr, "error: failed to import marshal module:\n");
        PyErr_Print();
        abort();
    }

    marshal_loads = PyObject_GetAttrString(marshal, "loads");
    if (marshal_loads == NULL) {
        fprintf(stderr, "error: failed to get marshal.loads:\n");
        PyErr_Print();
        abort();
    }

    return 0;
}

int FuzzerEntrypoint(const uint8_t *Data, size_t Size)
{
    assert(!PyErr_Occurred());

    PyObject *result = PyObject_CallFunction(marshal_loads,
                                             "s#",
                                             (const char *)Data,
                                             (int)Size);
    if (result == NULL) {
        //fprintf(stderr, "error: failed to call marshal.loads:\n");
        //PyErr_Print();
        PyErr_Clear();
    }
    Py_XDECREF(result);

    return 0;
}
