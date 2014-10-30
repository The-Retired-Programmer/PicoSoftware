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
package uk.org.rlinsdale.racetrainingdemonstrator.core;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.Location;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.Element;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.BADKEY;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.OK;

/**
 * The Core Information to describe the Simulation.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class ScenarioElement extends Element {

    // dimensions of the visible "playing surface" in metres (default is a 1km square with 0,0 in centre).
    /**
     * Default eastern edge of the "playing surface" in metres.
     */
    public static final double EAST_DEFAULT = -500;

    /**
     * Default western edge of the "playing surface" in metres.
     */
    public static final double WEST_DEFAULT = 500;

    /**
     * Default northern edge of the "playing surface" in metres.
     */
    public static final double NORTH_DEFAULT = 500;

    /**
     * Default southern edge of the "playing surface" in metres.
     */
    public static final double SOUTH_DEFAULT = -500;

    /**
     * Default scaling factor
     */
    public static final double SCALE_DEFAULT = 1;

    /**
     * Actual eastern edge of the "playing surface" in metres.
     */
    public double east = EAST_DEFAULT;

    /**
     * Actual western edge of the "playing surface" in metres.
     */
    public double west = WEST_DEFAULT;

    /**
     * Actual northern edge of the "playing surface" in metres.
     */
    public double north = NORTH_DEFAULT;

    /**
     * Actual southern edge of the "playing surface" in metres.
     */
    public double south = SOUTH_DEFAULT;
    // and actual limits

    /**
     * Maximum eastern edge of the "playing surface" in metres.
     */
    public double eastlimit = Double.MAX_VALUE;

    /**
     * Maximum western edge of the "playing surface" in metres.
     */
    public double westlimit = -Double.MAX_VALUE;

    /**
     * Maximum northern edge of the "playing surface" in metres.
     */
    public double northlimit = Double.MAX_VALUE;

    /**
     * Maximum southern edge of the "playing surface" in metres.
     */
    public double southlimit = -Double.MAX_VALUE;

    /**
     * The Start location
     */
    public Location startLocation = new Location(0, 0);
    //

    /**
     * The default scale factor (pixels per metre)
     */
    public double scale = SCALE_DEFAULT; // number of pixels per metre (horizontal)
    //

    /**
     * Time step - seconds per display repaint
     */
    public int secondsperdisplay = 1; // the time step per display

    /**
     * Time Scaling - less than 1 is faster ; greater than 1 is slower
     */
    public double speedup = 1;
    //

    /**
     * The first mark name
     */
    public String firstMark = "";

    public ScenarioElement() {
        super("scenario");
    }

    /**
     * Get the East Limit
     *
     * @return the east limit
     */
    public double getEastLimit() {
        return eastlimit;
    }

    /**
     * Get the West Limit
     *
     * @return the west limit
     */
    public double getWestLimit() {
        return westlimit;
    }

    /**
     * Get the North Limit
     *
     * @return the north limit
     */
    public double getNorthLimit() {
        return northlimit;
    }

    /**
     * Get the South Limit
     *
     * @return the south limit
     */
    public double getSouthLimit() {
        return southlimit;
    }

    /**
     * Get the East edge
     *
     * @return the east edge
     */
    public double getEast() {
        return east;
    }

    /**
     * Get the West edge
     *
     * @return the west edge
     */
    public double getWest() {
        return west;
    }

    /**
     * Get the North edge
     *
     * @return the north edge
     */
    public double getNorth() {
        return north;
    }

    /**
     * Get the South edge
     *
     * @return the south edge
     */
    public double getSouth() {
        return south;
    }

    /**
     * Draw the ScenarioElement on the display canvas.
     *
     * @param g2D the 2D graphics object
     * @return the scale factor (pixels/metre)
     */
    public double draw(Graphics2D g2D) {
        // set transform
        double hoffset = -west;
        double voffset = north;
        g2D.transform(new AffineTransform(scale, 0, 0, -scale, hoffset * scale, voffset * scale));
        // if limits set then darken limit areas
        if (westlimit > west) {
            Shape s = new Rectangle2D.Double(west, south,
                    westlimit - west, north - south);
            g2D.setColor(Color.darkGray);
            g2D.draw(s);
            g2D.fill(s);
        }
        if (eastlimit < east) {
            Shape s = new Rectangle2D.Double(eastlimit, south,
                    east - eastlimit, north - south);
            g2D.setColor(Color.darkGray);
            g2D.draw(s);
            g2D.fill(s);
        }
        if (northlimit < north) {
            Shape s = new Rectangle2D.Double(west, northlimit,
                    east - west, north - northlimit);
            g2D.setColor(Color.darkGray);
            g2D.draw(s);
            g2D.fill(s);
        }
        if (southlimit > south) {
            Shape s = new Rectangle2D.Double(west, south,
                    east - west, southlimit - south);
            g2D.setColor(Color.darkGray);
            g2D.draw(s);
            g2D.fill(s);
        }
        return scale;
    }

    /**
     * Get the display rate.
     *
     * @return display rate (seconds/display repaint)
     */
    public int getSecondsPerDisplay() {
        return secondsperdisplay;
    }

    /**
     * Get the speed up.
     *
     * @return the speed up (less than 1 is faster ; greater than 1 is slower)
     */
    public double getSpeedup() {
        return speedup;
    }

    /**
     * Get the name of the first mark.
     *
     * @return the mark name
     */
    public String getFirstMark() {
        return firstMark;
    }

    /**
     * Get the start location.
     *
     * @return the start location
     */
    public Location getStartLocation() {
        return startLocation;
    }

    @Override
    protected Status setParameter(KeyPair kp) {
        try {
            switch (kp.key) {
                case "east":
                    east = Double.parseDouble(kp.value);
                    return OK;
                case "west":
                    west = Double.parseDouble(kp.value);
                    return OK;
                case "north":
                    north = Double.parseDouble(kp.value);
                    return OK;
                case "south":
                    south = Double.parseDouble(kp.value);
                    return OK;
                case "eastlimit":
                    eastlimit = Double.parseDouble(kp.value);
                    return OK;
                case "westlimit":
                    westlimit = Double.parseDouble(kp.value);
                    return OK;
                case "northlimit":
                    northlimit = Double.parseDouble(kp.value);
                    return OK;
                case "southlimit":
                    southlimit = Double.parseDouble(kp.value);
                    return OK;
                case "zoom":
                    scale = Double.parseDouble(kp.value);
                    return OK;
                case "secondsperdisplay":
                    secondsperdisplay = Integer.parseInt(kp.value);
                    return OK;
                case "speedup":
                    speedup = Double.parseDouble(kp.value);
                    return OK;
                case "firstmark":
                    firstMark = kp.value;
                    return OK;
                case "startlocation":
                    startLocation = parseLocation(kp.value);
                    return OK;
                default:
                    return BADKEY;
            }
        } catch (NumberFormatException numberFormatException) {
            return BADVALUE;
        }
    }
}
