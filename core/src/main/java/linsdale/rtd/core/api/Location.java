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
package linsdale.rtd.core.api;

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

    public double x = 0;
    public double y = 0;

    public Location() {
        this.x = 0;
        this.y = 0;
    }

    public Location(double x, double y) {
        this.x = x;
        this.y = y;
    }

    public Location(Location pos) {
        x = pos.x;
        y = pos.y;
    }

    public void set(Location pos) {
        x = pos.x;
        y = pos.y;
    }
}
