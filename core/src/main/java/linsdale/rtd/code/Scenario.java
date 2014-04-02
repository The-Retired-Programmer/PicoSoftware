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
package linsdale.rtd.code;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;
import linsdale.rtd.core.api.Location;
import linsdale.rtd.core.api.RTABaseObject;

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Scenario extends RTABaseObject {

    // dimensions of the visible "playing surface" in metres (default is a 1km square with 0,0 in centre).
    public static final double EAST_DEFAULT = -500;
    public static final double WEST_DEFAULT = 500;
    public static final double NORTH_DEFAULT = 500;
    public static final double SOUTH_DEFAULT = -500;
    public static final double SCALE_DEFAULT = 1;
    public double east = EAST_DEFAULT;
    public double west = WEST_DEFAULT;
    public double north = NORTH_DEFAULT;
    public double south = SOUTH_DEFAULT;
    // and actual limits
    public double eastlimit = Double.MAX_VALUE;
    public double westlimit = -Double.MAX_VALUE;
    public double northlimit = Double.MAX_VALUE;
    public double southlimit = -Double.MAX_VALUE;
    public Location startLocation = new Location(0, 0);
    //
    public double scale = SCALE_DEFAULT; // number of pixels per metre (horizontal)
    //
    public int secondsperdisplay = 1; // the time step per display
    public double speedup = 1; // scaling factor < 1 is faster >1 is slower
    //
    public String firstMark = "";

    public Scenario() {
        super("scenario");
    }

    public double getEastLimit() {
        return eastlimit;
    }

    public double getWestLimit() {
        return westlimit;
    }

    public double getNorthLimit() {
        return northlimit;
    }

    public double getSouthLimit() {
        return southlimit;
    }

    public double getEast() {
        return east;
    }

    public double getWest() {
        return west;
    }

    public double getNorth() {
        return north;
    }

    public double getSouth() {
        return south;
    }

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

    public int getSecondsPerDisplay() {
        return secondsperdisplay;
    }

    public double getSpeedup() {
        return speedup;
    }

    public String getFirstMark() {
        return firstMark;
    }

    public Location getStartLocation() {
        return startLocation;
    }

    @Override
    public int setParameter(String key, String value) {
        try {
            switch (key) {
                case "east":
                    east = Double.parseDouble(value);
                    return PARAM_OK;
                case "west":
                    west = Double.parseDouble(value);
                    return PARAM_OK;
                case "north":
                    north = Double.parseDouble(value);
                    return PARAM_OK;
                case "south":
                    south = Double.parseDouble(value);
                    return PARAM_OK;
                case "eastlimit":
                    eastlimit = Double.parseDouble(value);
                    return PARAM_OK;
                case "westlimit":
                    westlimit = Double.parseDouble(value);
                    return PARAM_OK;
                case "northlimit":
                    northlimit = Double.parseDouble(value);
                    return PARAM_OK;
                case "southlimit":
                    southlimit = Double.parseDouble(value);
                    return PARAM_OK;
                case "zoom":
                    scale = Double.parseDouble(value);
                    return PARAM_OK;
                case "secondsperdisplay":
                    secondsperdisplay = Integer.parseInt(value);
                    return PARAM_OK;
                case "speedup":
                    speedup = Double.parseDouble(value);
                    return PARAM_OK;
                case "firstmark":
                    firstMark = value;
                    return PARAM_OK;
                case "startlocation":
                    startLocation = parseLocation(value);
                    return PARAM_OK;
                default:
                    return PARAM_BADKEY;
            }
        } catch (NumberFormatException numberFormatException) {
            return PARAM_BADVALUE;
        }
    }

    @Override
    public int checkParameter(String key, String value) {
        try {
            switch (key) {
                case "east":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "west":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "north":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "south":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "eastlimit":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "westlimit":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "northlimit":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "southlimit":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "zoom":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "secondsperdisplay":
                    Integer.parseInt(value);
                    return PARAM_OK;
                case "speedup":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "firstmark":
                    return PARAM_OK;
                case "startlocation":
                    parseLocation(value);
                    return PARAM_OK;
                default:
                    return PARAM_BADKEY;
            }
        } catch (NumberFormatException numberFormatException) {
            return PARAM_BADVALUE;
        }
    }
}
