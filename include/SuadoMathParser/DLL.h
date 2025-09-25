#ifndef SMP_DLL_H_
#define SMP_DLL_H_

#ifdef _WIN32
	#if defined(SMP_IMPORT)
		#define SMP_API __declspec(dllimport)
	#elif defined(SMP_EXPORT)
		#define SMP_API __declspec(dllexport)
	#endif
#else
	#if defined(SMP_EXPORT)
		#define SMP_API __attribute__((visibility("default")))
	#endif
#endif

#ifndef SMP_API
#define SMP_API
#endif

#endif // SMP_DLL_H_