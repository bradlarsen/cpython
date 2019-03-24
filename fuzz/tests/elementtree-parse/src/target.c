#include <Python.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


static PyObject *_elementtree_parse = NULL;
static PyObject *StringIO = NULL;

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

    PyObject *_elementtree = NULL;
    PyObject *XMLParser = NULL;
    PyObject *XMLParser_instance = NULL;
    PyObject *io = NULL;

    _elementtree = PyImport_ImportModule("_elementtree");
    if (_elementtree == NULL) {
        fprintf(stderr, "error: failed to import _elementtree:\n");
        PyErr_Print();
        abort();
    }

    XMLParser = PyObject_GetAttrString(_elementtree, "XMLParser");
    if (XMLParser == NULL) {
        fprintf(stderr, "error: failed to get _elementtree.Parser:\n");
        PyErr_Print();
        abort();
    }

    XMLParser_instance = PyObject_CallObject(XMLParser, NULL);
    if (XMLParser_instance == NULL) {
        fprintf(stderr, "error: failed to create XMLParser instance:\n");
        PyErr_Print();
        abort();
    }

    _elementtree_parse =
        PyObject_GetAttrString(XMLParser_instance, "_parse_whole");
    if (_elementtree_parse == NULL) {
        fprintf(stderr, "error: failed to get _elementtree.XMLParser()._parse_whole method:\n");
        PyErr_Print();
        abort();
    }

    io = PyImport_ImportModule("io");
    if (io == NULL) {
        fprintf(stderr, "error: failed to import io:\n");
        PyErr_Print();
        abort();
    }

    StringIO = PyObject_GetAttrString(io, "StringIO");
    if (StringIO == NULL) {
        fprintf(stderr, "error: failed to get io.StringIO:\n");
        PyErr_Print();
        abort();
    }

    Py_DECREF(_elementtree);
    Py_DECREF(XMLParser);
    Py_DECREF(XMLParser_instance);
    Py_DECREF(io);

    assert(!PyErr_Occurred());

    return 0;
}

int FuzzerEntrypoint(const uint8_t *Data, size_t Size)
{
    const char *buffer = (const char *)Data;
    size_t buffer_size = Size;

    assert(!PyErr_Occurred());

    PyObject *result = NULL;
    PyObject *input = NULL;

    input = PyObject_CallFunction(StringIO, "s#", buffer, (int)buffer_size);
    if (input == NULL) {
        //fprintf(stderr,
        //        "error: failed to create StringIO object:\n");
        //PyErr_Print();
        PyErr_Clear();
        goto finish;
    }

    result = PyObject_CallFunction(_elementtree_parse, "O", input);
    if (!result) {
        //fprintf(stderr,
        //        "error: failed to call _elementtree.XMLParser._parse:\n");
        //PyErr_Print();
        PyErr_Clear();
        goto finish;
    }

finish:
    Py_XDECREF(input);
    Py_XDECREF(result);

    return 0;
}
