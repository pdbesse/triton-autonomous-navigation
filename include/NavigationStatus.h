
#pragma once

/*
 * NavigationStatus
 * ----------------
 * Represents the outcome of a navigation update.
 *
 * enum class provides strongly typed named values.
 * Unlike a traditional enum, its members do not
 * implicitly convert to integers.
 *
 * This makes navigation outcomes easier to interpret
 * and prevents callers from accidentally treating
 * different status types as interchangeable.
 */
enum class NavigationStatus {
    /*
     * Navigation advanced successfully, but at least
     * one waypoint remains.
     */
    InProgress,

    /*
     * Every waypoint in the route has been reached.
     */
    Completed,

    /*
     * Navigation cannot continue because the vessel
     * has no forward speed while waypoints remain.
     */
    Stalled,

    /*
     * Navigation was given invalid parameters.
     *
     * The controller currently uses exceptions for
     * invalid elapsed time. We will define a consistent
     * error-handling policy as we integrate statuses.
     */
    InvalidInput
};
