
#ifndef __VSTREAM_H__
#define __VSTREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) int vstream_open( void* parameter );
__declspec(dllexport) int vstream_read(char* buffer, int max_length);
__declspec(dllexport) int vstream_close( void );

#ifdef __cplusplus
}
#endif

#endif /* __BBM_H__ */
