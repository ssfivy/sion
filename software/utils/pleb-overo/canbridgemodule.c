#include "Python.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#include "packet.h"
#include "socket.h"
#include "input.h"
#include "output.h"

/* Configuration - This should be moved to a new Python type initialiser */ 
static char	*localip = "192.168.0.10";
static int	localport = 3491;
static char*	remoteip = "192.168.0.20";
static int	remoteport = 3490;

//static char	*localip = "127.0.0.1";
//static int	localport = 3491;
//static char*	remoteip = "127.0.0.1";
//static int	remoteport = 3490;

static PyObject *ErrorObject;

/* Prototypes */ 
static PyObject *canbridge_output_send(PyObject *self, PyObject *args);
static PyObject *canbridge_input_recv(PyObject *self, PyObject *args);
static PyObject *canbridge_output_queue(PyObject *self, PyObject *args);
static PyObject *canbridge_input_dequeue(PyObject *self, PyObject *args);

/* Method Table */ 
static PyMethodDef can_methods[] = {
    {"outputSend",  (PyCFunction)canbridge_output_send, METH_VARARGS, "Send enqueued packets"},
    {"inputRecv",   (PyCFunction)canbridge_input_recv,  METH_VARARGS, "Receive packets"},
    {"outputQueue",  (PyCFunction)canbridge_output_queue, METH_VARARGS, "Enqueue new packets"},
    {"inputDequeue",  (PyCFunction)canbridge_input_dequeue, METH_VARARGS, "Dequeue received packets"},
    {NULL, (PyCFunction)NULL, 0, NULL}        /* Sentinel */
};

/* Implementation */ 
static PyObject *
canbridge_output_send(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) return NULL;
    
    output_send(); 
    
    return Py_BuildValue("");
}

static PyObject *
canbridge_input_recv(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) return NULL;
    
    input_recv(); 
    
    return Py_BuildValue("");
}

static PyObject *
canbridge_output_queue(PyObject *self, PyObject *args)
{
    union pkt_any out; 

    out.sample.type = PKT_SAMPLE; 

    if (!PyArg_ParseTuple(args, "(IKi)", 
        &out.sample.can.id, 
        (uint64_t*)&out.sample.can.data, 
        &out.sample.can.length))
        return NULL;

    output_queue(&out); 

    return Py_BuildValue("");
}

static PyObject *
canbridge_input_dequeue(PyObject *self, PyObject *args)
{
    union pkt_any in; 
    int ret; 
    
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    ret = input_dequeue(&in); 
 
    if(ret == 0)
        return Py_BuildValue("(IKi)", 
            in.sample.can.id, 
            *(uint64_t*)in.sample.can.data, 
            in.sample.can.length);
    else
        return Py_BuildValue(""); 
}
    
    
static char canbridge_module_documentation[] = 
""
;


    
PyMODINIT_FUNC
initcanbridge(void)
{
        PyObject *m, *d;

        /* Create the module and add the functions */
        m = Py_InitModule4("canbridge", can_methods,
                canbridge_module_documentation,
                (PyObject*)NULL,PYTHON_API_VERSION);

        /* Add some symbolic constants to the module */
        d = PyModule_GetDict(m);
        ErrorObject = PyString_FromString("canbridge.error");
        PyDict_SetItemString(d, "error", ErrorObject);

        /* XXXX Add constants here */

        /* This should be done on initialisation ofa  type so that
            we can have multiple CAN interfaces */ 
        if (socket_init(localip, localport, remoteip, remoteport))
            Py_FatalError("Couldn't initialise socket"); 
        else{
            input_init();
            output_init();
        }
        
        /* Check for errors */
        if (PyErr_Occurred())
                Py_FatalError("can't initialize module canbridge");
}
