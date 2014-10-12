/*
 * Copyright (C) 2014 Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package uk.org.rlinsdale.racetrainingdemonstrator.core.api;

/**
 * A location is a relative position from a logical origin
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Location {
    // locations are relative
    // measured from a logical origin (so x and y are signed)
    // y +ve is North; x +ve is East
    // units are in metres

    /**
     * The horizontal position (ie east west)
     */
    public double x = 0;

    /**
     * the vertical position (ie north south)
     */
    public double y = 0;

    /**
     * Constructor.
     */
    public Location() {
        this.x = 0;
        this.y = 0;
    }

    /**
     * Constructor.
     *
     * @param x the horizonal coordinate
     * @param y the vertical coordinate
     */
    public Location(double x, double y) {
        this.x = x;
        this.y = y;
    }

    /**
     * Constructor.
     *
     * @param pos the location (used to clone this location)
     */
    public Location(Location pos) {
        x = pos.x;
        y = pos.y;
    }

    /**
     * Set the location.
     *
     * @param pos the location (to copy coordinates from)
     */
    public void set(Location pos) {
        x = pos.x;
        y = pos.y;
    }
}
