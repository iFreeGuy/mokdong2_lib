#pragma once

const int default_buffer_size = 655360 + 2;

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT        
//#define _WIN32_WINNT 0x0500		// Window 2000 and over
#define _WIN32_WINNT 0x0A00			// Window 10
#endif 

#ifndef BOOST_BIND_GLOBAL_PLACEHOLDERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#endif

#if (_MSC_VER >= 1915)
#define no_init_all deprecated
#endif