#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

    int DLL_EXPORT LogIt(int key_stroke, char *file);
    void DLL_EXPORT HideMe();
    int DLL_EXPORT MainLoop();
    void DLL_EXPORT TestDLL();
    void DLL_EXPORT StartMainLoop();

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
