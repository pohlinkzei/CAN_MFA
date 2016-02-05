/*
 * navigation.h
 *
 * Created: 05.02.2016 21:01:17
 *  Author: Hubert
 */ 


#ifndef NAVIGATION_H_
#define NAVIGATION_H_

// taken from navit - which will talk with this device...
enum nav_status {
	status_invalid = -2,		/*!< Status is unknown. The {@code nav_status} attribute will never return this
								     value but code that listens to changes to this attribute may use this value
								     as a placeholder until the first actual status has been obtained. */
	status_no_route = -1,		/*!< No route was found */
	status_no_destination = 0,	/*!< No destination set, not routing */
	status_position_wait = 1,	/*!< Destination is set but current position is unknown */
	status_calculating = 2,		/*!< A new route is being calculated and turn instructions are being generated */
	status_recalculating = 3,	/*!< The existing route is being recalculated, along with its turn instructions.
								     Note that as the vehicle follows a route, status will flip between
								     {@code status_routing} and {@code status_recalculating} with every position
								     update. */
	status_routing = 4,			/*!< A route with turn instructions has been calculated and the user is being
								     guided along it */
};



#endif /* NAVIGATION_H_ */