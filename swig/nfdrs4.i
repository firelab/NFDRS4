// nfdrs.i

%module nfdrs4
%{
#include "../lib/NFDRS4/include/nfdrs4.h"
%}
%{
#include "../lib/NFDRS4/include/deadfuelmoisture.h"
%}
%{
#include "../lib/NFDRS4/include/livefuelmoisture.h"
%}
%{
#include "../lib/NFDRS4/include/dfmcalcstate.h"
%}
%{
#include "../lib/NFDRS4/include/lfmcalcstate.h"
%}
%{
#include "../lib/NFDRS4/include/nfdrs4calcstate.h"
%}
%{
#include "../lib/utctime/include/utctime.h"
%}
%include typemaps.i
%include "std_vector.i"
%include "std_string.i"
#%include "std_iostream.i"
%include "windows.i"

//ignore for now:
%ignore operator <<;
%ignore operator >>;

#ifdef SWIGPYTHON
typedef float FP_STORAGE_TYPE;
typedef long time_t;

%typemap(out) DoubleArray * {
  int len=$1->length, i;

  $result = PyList_New(len);

  for (i = 0; i < len; i++) {
    PyObject *o = PyFloat_FromDouble($1->arrayPtr[i]);
    PyList_SetItem($result,i,o);
  }
}

%extend DeadFuelMoisture {
   %rename (Const_assign_DeadFuelMoisture) operator= ( const DeadFuelMoisture & ) ;
}
#endif

#ifdef SWIGPHP
%typemap(out) DoubleArray *{
   int len=$1->length, i;

   //while ($1[len])
   //     len++;

   array_init(return_value);

   for (i=0;i<len;i++){
        add_next_index_double(return_value, $1->arrayPtr[i]);
   }

   delete[] $1->arrayPtr;
   free($1);
}
   
#endif

%include "../lib/NFDRS4/include/deadfuelmoisture.h"
%include "../lib/NFDRS4/include/livefuelmoisture.h"
%include "../lib/NFDRS4/include/dfmcalcstate.h"
%include "../lib/NFDRS4/include/lfmcalcstate.h"
%include "../lib/NFDRS4/include/nfdrs4calcstate.h"
//%include "../lib/NFDRS4/include/station.h"
%include "../lib/NFDRS4/include/nfdrs4.h"
%include "../lib/utctime/include/utctime.h"
