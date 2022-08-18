/*
 * Copyright 2014-2017 Richard Linsdale.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package uk.theretiredprogrammer.racetrainingdemonstrator.core.api;

/**
 * A location is a relative position from a logical origin
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
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
