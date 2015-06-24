/*
 * The contents of this file are subject to the terms of the Common Development 
 * and Distribution License (the "License"). You may not use this file except in 
 * compliance with the License. You can obtain a copy of the license at 
 * SysCommon/license.html or http://www.sun.com/cddl/cddl.html. See the License 
 * for the specific language governing permissions and limitations under the 
 * License.
 * 
 * When distributing Covered Code, include this CDDL HEADER in each file and 
 * include the License file at SysCommon/license.html.
 * If applicable, add the following below this CDDL HEADER, with the fields 
 * enclosed by brackets "[]" replaced with your own identifying information: 
 * Portions Copyright [yyyy] [name of copyright owner]
 */

/**
 * The contents of this file are only ever included if the DEBUG pre-processor symbol is defined.
 * This file should only be included in .cpp files, and only after all nominal headers have been
 * included.
 */
#ifdef DEBUG

	// Redefine new keyword to provide better memory leak reports
	#if _WIN32 || _WIN64
		#ifndef DBG_NEW      
			#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
			#define new DBG_NEW   
		#endif
	#endif

#endif

