#pragma once

#include "Platform.h"

namespace SysCommon
{
	/**
	 * An event object is a synchronization object whose state can be explicitly set to signaled by
	 * calling the signal() function. Event objects can be used across threads to indicate that a
	 * particular event has occurred.
	 * <p/>
	 * Threads may wait for events to be signaled by calling the waitFor() method.
	 */
	class Event
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			NATIVE_EVENT event;
			String name;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * Creates an event in the specified initial state.
			 *
			 * @param initialState whether to create the event in a signaled state.
			 * @param name An identifying name for the event
			 */
			Event( bool initialState, const String& name );
			virtual ~Event();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Puts the event in a signaled state, waking up any threads waiting on it. The event
			 * will remain in a signaled state until the clear() method is called.
			 */
			void signal();

			/**
			 * Clears the event and puts it into a non-signaled state.
			 */
			void clear();

			/**
			 * Waits for this event to enter a signaled state. If the event is already in a signaled
			 * state, then the method will return immediately, otherwise the method will block
			 * indefinitely until the event is signaled.
			 *
			 * @return a WaitResult enumeration value indicating the success of this function. As
			 * 		   this method waits indefinitely the method will either return WR_SUCCEEDED if
			 * 		   the event is signaled, or WR_ABANDONED if the event is destroyed before it
			 * 		   is signaled
			 */
			WaitResult waitFor();

			/**
			 * Waits for this event to enter a signaled state. If the event is already in a signaled
			 * state, then the method will return immediately, otherwise the method will block
			 * until either the event is signaled, or the specified timeout period has elapsed.
			 *
			 * @param timeout the maximum time period, in milliseconds, that this method should
			 *        wait for the event to be signaled
			 * @return a WaitResult enumeration value indicating the success of this function. The
			 *         method will return:
			 *         <ul>
			 *         	<li>WR_SUCCEEDED if the event is signaled within the prescribed time
			 *         	period</li>
			 *         	<li>WR_TIMEOUT if the timeout period elapses before the event is
			 *         	signaled</li>
			 *         	<li>WR_ABANDONED if the event is destroyed before it is signaled</li>
			 *         </ul>
			 */
			WaitResult waitFor( unsigned long timeout );

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};
}

