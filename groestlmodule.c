#include <Python.h>

#include "sph_groestl.h"
#include "sph_sha2.h"

void groestlmyr_hash(const char* input, int length, char* output)
{
    /* FIXME: length is ignored */

    sph_groestl512_context ctx_groestl;
    sph_sha256_context ctx_sha2;

    uint32_t hash[16];

    sph_groestl512_init(&ctx_groestl);
    sph_groestl512(&ctx_groestl, input, 80);
    sph_groestl512_close(&ctx_groestl, (void*) hash);

    sph_sha256_init(&ctx_sha2);
    sph_sha256(&ctx_sha2, hash, 64);
    sph_sha256_close(&ctx_sha2, (void*) hash);

    memcpy(output, hash, 32);
}


static void GroestlCoinHash(const char *input, int length, char *output)
{
    uint32_t hashA[16], hashB[16];
    
    sph_groestl512_context ctx_groestl[2];

    sph_groestl512_init(&ctx_groestl[0]);
    sph_groestl512 (&ctx_groestl[0], input, length);
    sph_groestl512_close(&ctx_groestl[0], hashA);
    
    sph_groestl512_init(&ctx_groestl[1]);
    sph_groestl512 (&ctx_groestl[1], hashA, 64);
    sph_groestl512_close(&ctx_groestl[1], hashB);
    
    memcpy(output, hashB, 32);
}

static PyObject *groestlcoin_gethash(PyObject *self, PyObject *args)
{
    char *output;
    PyObject *value;
#if PY_MAJOR_VERSION >= 3
    PyBytesObject *input;
#else
    PyStringObject *input;
#endif
    int length;
    if (!PyArg_ParseTuple(args, "Si", &input, &length))
        return NULL;
    Py_INCREF(input);
    output = PyMem_Malloc(32);

#if PY_MAJOR_VERSION >= 3
    GroestlCoinHash((char *)PyBytes_AsString((PyObject*) input), length, output);
#else
    GroestlCoinHash((char *)PyString_AsString((PyObject*) input), length, output);
#endif
    Py_DECREF(input);
#if PY_MAJOR_VERSION >= 3
    value = Py_BuildValue("y#", output, 32);
#else
    value = Py_BuildValue("s#", output, 32);
#endif
    PyMem_Free(output);
    return value;
}

static PyObject *groestlmyr_gethash(PyObject *self, PyObject *args)
{
    char *output;
    PyObject *value;
#if PY_MAJOR_VERSION >= 3
    PyBytesObject *input;
#else
    PyStringObject *input;
#endif
    int length;
    if (!PyArg_ParseTuple(args, "Si", &input, &length))
        return NULL;
    Py_INCREF(input);
    output = PyMem_Malloc(32);

#if PY_MAJOR_VERSION >= 3
    groestlmyr_hash((char *)PyBytes_AsString((PyObject*) input), length, output);
#else
    groestlmyr_hash((char *)PyString_AsString((PyObject*) input), length, output);
#endif
    Py_DECREF(input);
#if PY_MAJOR_VERSION >= 3
    value = Py_BuildValue("y#", output, 32);
#else
    value = Py_BuildValue("s#", output, 32);
#endif
    PyMem_Free(output);
    return value;
}

static PyMethodDef GroestlMethods[] = {
    { "getGroestlcoinHash", groestlcoin_gethash, METH_VARARGS, "Returns the groestlcoin hash" },
    { "getGroestlMyrHash", groestlmyr_gethash, METH_VARARGS, "Returns the Myriadcoin Groestl hash" },
    { NULL, NULL, 0, NULL }
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef GroestlModule = {
    PyModuleDef_HEAD_INIT,
    "groestl_hash",
    "...",
    -1,
    GroestlMethods
};

PyMODINIT_FUNC PyInit_groestl_hash(void) {
    return PyModule_Create(&GroestlModule);
}

#else

PyMODINIT_FUNC initgroestl_hash(void) {
    (void) Py_InitModule("groestl_hash", GroestlMethods);
}
#endif
