#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <numpy/arrayobject.h>

#include "data_types.h"
#include "cluster_reader.h"


// Create a custom numpy data type that should reflect
// our cluster data type. 
// TODO! Update with the actual cluster data type
static PyArray_Descr *cluster_dt() {
    PyObject *dtype_dict;
    PyArray_Descr *dtype;
    dtype_dict = Py_BuildValue(
        "[(s, s),(s, s)]",
        "first", "i4", "second", "i4");
    PyArray_DescrConverter(dtype_dict, &dtype);
    Py_DECREF(dtype_dict);
    return dtype;
}


// Structure holding our cluster reader class
typedef struct {
    PyObject_HEAD FILE *fp;
} ClusterFileReader;


// Constructor: sets the fp to NULL then tries to open the file 
// raises python exception if something goes wrong
// returned object should mean file is open and ready to read
static int ClusterFileReader_init(ClusterFileReader *self, PyObject *args,
                                  PyObject *kwds) {
    self->fp = NULL;

    // Parse file name 
    const char *fname = NULL;
    if (!PyArg_ParseTuple(args, "s", &fname))
        return -1;

    self->fp = fopen(fname, "rb");

    // Raise python exception using information from errno
    if (self->fp == NULL) {
        PyErr_SetFromErrnoWithFilename(PyExc_OSError, fname);
        return -1;
    }

    //Success
    return 0;
}

// Close file on clearing the object
static int
ClusterFileReader_clear(ClusterFileReader *self)
{
    if(self->fp){
        printf("Closing file\n");
        fclose(self->fp);
        self->fp = NULL;
    }  
    return 0;
}

// Custom destructor, not sure this is needed
// might be an easier way to access the fclose on clear
static void
ClusterFileReader_dealloc(ClusterFileReader *self)
{
    PyObject_GC_UnTrack(self);
    ClusterFileReader_clear(self);
    Py_TYPE(self)->tp_free((PyObject *) self);
}


// read method 
static PyObject *
ClusterFileReader_read(ClusterFileReader *self, PyObject *args,
                                  PyObject *kwds)
{

    const int ndim = 1;
    Py_ssize_t size = 0;
    if (!PyArg_ParseTuple(args, "|n", &size))
        return NULL;

    npy_intp dims[] = {size};

    //Create an uninitialized numpy array
    PyArray_Descr *dtype = cluster_dt();
    PyObject *clusters = PyArray_SimpleNewFromDescr(ndim, dims, dtype);
    PyArray_FILLWBYTE((PyArrayObject *)clusters, 0); //zero initialization can be removed later
    
    
    //Get a pointer to the array memory
    void* buf = PyArray_DATA((PyArrayObject *)clusters);

    // Call the standalone C code to read clusters from file
    // Here goes the looping, removing frame numbers etc. 
    int64_t n_read = read_clusters(self->fp, size, buf);

    //TODO! All kinds of error checking here!!! Resize array etc.

    return clusters;
}

// List all methods in our ClusterFileReader class
static PyMethodDef ClusterFileReader_methods[] = {
    {"read", (PyCFunction) ClusterFileReader_read, METH_VARARGS,
     "Read clusters"
    },
    {NULL}  /* Sentinel */
};


// Class defenition
static PyTypeObject ClusterFileReaderType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "creader.ClusterFileReader",
    .tp_doc = PyDoc_STR("ClusterFileReader implemented in C"),
    .tp_basicsize = sizeof(ClusterFileReader),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
    .tp_new = PyType_GenericNew,
    .tp_clear = (inquiry) ClusterFileReader_clear,
    .tp_dealloc = (destructor) ClusterFileReader_dealloc,
    .tp_init = (initproc)ClusterFileReader_init,
    .tp_methods = ClusterFileReader_methods,
};

//------------------------------------------- Module stuff from here

// Docstring
static char module_docstring[] =
    "C functions to read cluster files";

// This is the module itself
static PyMethodDef module_methods[] = {
  {NULL, NULL, 0, NULL}};

static struct PyModuleDef creader_def = {PyModuleDef_HEAD_INIT, "creader",
                                         module_docstring, -1, module_methods};

PyMODINIT_FUNC PyInit_creader(void) {
    PyObject *m;
    if (PyType_Ready(&ClusterFileReaderType) < 0)
        return NULL;
    m = PyModule_Create(&creader_def);
    if (m == NULL)
        return NULL;

    import_array();

    Py_INCREF(&ClusterFileReaderType);
    if (PyModule_AddObject(m, "ClusterFileReader",
                           (PyObject *)&ClusterFileReaderType) < 0) {
        Py_DECREF(&ClusterFileReaderType);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}

