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
package linsdale.rtd.mark;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import linsdale.nbpcg.annotations.RegisterLog;
import linsdale.rtd.core.api.Location;
import linsdale.rtd.core.api.RTAObject;
import linsdale.rtd.core.api.Direction;
import linsdale.rtd.core.api.Polar;
import linsdale.rtd.code.DefFileModel;
import linsdale.nbpcg.supportlib.OutputReporter;

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@RegisterLog("linsdale.rta.mark")
public class Mark extends RTAObject {

    // parameters which can be set via the definition file
    private Location pos = new Location();
    private boolean downwindLaylines = false;
    private boolean windwardLaylines = false;
    private double laylineLength = 0;
    private Color colour = Color.RED;
    private Color laylineColour = Color.black;
    private boolean leavetoport = true;
    private String nextMark = "";
    //
    private Location laylineBase;
    //
    private final double size = 1; // set up as 1 metre diameter object

    public Mark(String name, OutputReporter reporter, DefFileModel dfm) {
        super(name, reporter, dfm);
        setLaylineBase();
    }

    public boolean toPort() {
        return leavetoport;
    }

    public Location getLocation() {
        return pos;
    }

    public Polar getTruePolar(Location boatpos) { 
        return new Polar(pos, boatpos);
    }
    
    public Polar getCoursePolar(Location boatpos, Direction boatDirection, boolean reachdownwind, double boatoffset, double distance) {   // actually is the location to pass the mark
        Polar boatmove = new Polar(distance, boatDirection); // this the potential boat movement during direction change
        Polar tomark = new Polar(pos, boatpos);
        tomark.subtract(boatmove);
        Direction w = dfm.getWind().getFlow(pos).getDirection();
        int relative = tomark.absAngleDiff(w);
        if (relative <= 45) {
            return tomark.add(new Polar((size + boatoffset) * 2, w.getDegrees() + (leavetoport ? 90 : -90)));
        } else if (relative >= 135 && reachdownwind) {
            return tomark.add(new Polar((size + boatoffset) * 2, w.getDegrees() + 180 + (leavetoport ? 90 : -90)));
        } else {
            return tomark.add(new Polar((size + boatoffset) * Math.sqrt(2), boatDirection.getDegrees() + (leavetoport ? 90 : -90)));
        }
    }

    public String nextMark() {
        return nextMark;
    }

    @Override
    public void timerAdvance(int time) {
        setLaylineBase();
    }

    private void setLaylineBase() {
        // calculate the target point as the present time for current wind direction
        Direction windDirection = dfm.getWind().getFlow(pos).getDirection();
        Polar toPt;
        if (downwindLaylines) {
            toPt = new Polar(2 * size, windDirection.getDegrees()
                    + (leavetoport ? -90 : +90));
        } else {
            toPt = new Polar(2 * size, windDirection.getDegrees()
                    + (leavetoport ? 90 : -90));
        }
        laylineBase = toPt.polar2Location(pos);
    }

    @Override
    public void draw(Graphics2D g2D, double pixelsPerMetre) {
        double radius;
        if (pixelsPerMetre > 6) {
            // if it will be visible then draw circle to scale
            radius = size / 2 * pixelsPerMetre;
        } else {
            radius = 3; // create visible object
        }
        Ellipse2D s = new Ellipse2D.Double(-radius, -radius, radius * 2, radius * 2);
        AffineTransform xform = g2D.getTransform();
        g2D.translate(pos.x, pos.y);
        g2D.scale(1 / pixelsPerMetre, 1 / pixelsPerMetre);
        g2D.setColor(colour);
        g2D.draw(s);
        g2D.fill(s);
        g2D.setTransform(xform);
        // now draw the laylines - this are scale independent and set to 1 pixel line
        Direction windDirection = dfm.getWind().getFlow(pos).getDirection();
        if (windwardLaylines) {
            pixelLine(g2D, laylineBase, new Polar(laylineLength,
                    windDirection.getDegrees() + 135), laylineColour, pixelsPerMetre);
            pixelLine(g2D, laylineBase, new Polar(laylineLength,
                    windDirection.getDegrees() - 135), laylineColour, pixelsPerMetre);
        }
        if (downwindLaylines) {
            pixelLine(g2D, laylineBase, new Polar(laylineLength,
                    windDirection.getDegrees() + 45), laylineColour, pixelsPerMetre);
            pixelLine(g2D, laylineBase, new Polar(laylineLength,
                    windDirection.getDegrees() - 45), laylineColour, pixelsPerMetre);
        }
    }

    private void pixelLine(Graphics2D g2D, Location laylineBase, Polar line,
            Color laylineColour, double pixelsPerMetre) {
        BasicStroke stroke = new BasicStroke((float) (1f / pixelsPerMetre));
        g2D.setStroke(stroke);
        g2D.setColor(laylineColour);
        Location end = line.polar2Location(laylineBase);
        g2D.draw(new Line2D.Double(laylineBase.x, laylineBase.y, end.x, end.y));
    }

    @Override
    public int setParameter(String key, String value) {
        try {
            switch (key) {
                case "location":
                    pos = parseLocation(value);
                    setLaylineBase();
                    return PARAM_OK;
                case "windwardlaylines":
                    windwardLaylines = parseYesNo(value);
                    setLaylineBase();
                    return PARAM_OK;
                case "downwindlaylines":
                    downwindLaylines = parseYesNo(value);
                    setLaylineBase();
                    return PARAM_OK;
                case "laylinelength":
                    laylineLength = Double.parseDouble(value);
                    return PARAM_OK;
                case "laylinecolour":
                    laylineColour = parseColour(value);
                    return PARAM_OK;
                case "colour":
                    colour = parseColour(value);
                    return PARAM_OK;
                case "leavetoport":
                    leavetoport = parseYesNo(value);
                    setLaylineBase();
                    return PARAM_OK;
                case "nextmark":
                    nextMark = value;
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
                case "location":
                    parseLocation(value);
                    return PARAM_OK;
                case "windwardlaylines":
                    parseYesNo(value);
                    return PARAM_OK;
                case "downwindlaylines":
                    parseYesNo(value);
                    return PARAM_OK;
                case "laylinelength":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "laylinecolour":
                    parseColour(value);
                    return PARAM_OK;
                case "colour":
                    Color chk = parseColour(value);
                    return PARAM_OK;
                case "leavetoport":
                    parseYesNo(value);
                    return PARAM_OK;
                case "nextmark":
                    return PARAM_OK;
                default:
                    return PARAM_BADKEY;
            }
        } catch (NumberFormatException numberFormatException) {
            return PARAM_BADVALUE;
        }
    }
}
